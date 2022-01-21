///////////////////////////////////////////////////////////////////////////////
// main.cpp
// ========
// testing Pixel Buffer Object for unpacking (uploading) pixel data to PBO
// using GL_ARB_pixel_buffer_object extension
// It uses 2 PBOs to optimize uploading pipeline; application to PBO, and PBO to
// texture object.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2007-10-22
// UPDATED: 2018-09-04
///////////////////////////////////////////////////////////////////////////////

#include "glExtension.h"                        // glInfo struct
//#include <GL/glut.h>


//GLES, EGL, WL includes



#include <EGL/egl.h>
#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>
#include <GLES2/gl2ext.h>

#include <GL/glu.h>


#include <wayland-client.h>
#include <wayland-egl.h>
#include <wayland-cursor.h>


#include <cassert>


#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include "Timer.h"

// GLUT CALLBACK functions ////////////////////////////////////////////////////
/*
void displayCB();
void reshapeCB(int w, int h);
void timerCB(int millisec);
void idleCB();
void keyboardCB(unsigned char key, int x, int y);
void mouseCB(int button, int stat, int x, int y);
void mouseMotionCB(int x, int y);
*/

// CALLBACK function when exit() called ///////////////////////////////////////
//void exitCB();

void initGL();
void displayUpdate();
//int  initGLUT(int argc, char **argv);

void registry_global(void* data, struct wl_registry* registry, uint32_t id, const char* interface, uint32_t version);
void initWaylandDisplay(struct wl_display** wlDisplay, struct wl_surface** wlSurface);
void initEGLDisplay(EGLNativeDisplayType nativeDisplay, EGLNativeWindowType nativeWindow, EGLDisplay* eglDisp, EGLSurface* eglSurf);
void initWindow(GLint width, GLint height, struct wl_display** wlDisplay);

bool initSharedMem();
void clearSharedMem();
void initLights();
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
void updatePixels(GLubyte* dst, int size);

//void drawString(const char *str, int x, int y, float color[4], void *font);
//void drawString3D(const char *str, float pos[3], float color[4], void *font);
void drawString(const char *str, int x, int y, float color[4]);
void drawString3D(const char *str, float pos[3], float color[4]);


void showInfo();
void showTransferRate();
void printTransferRate();


// constants
const int    SCREEN_WIDTH    = 800;
const int    SCREEN_HEIGHT   = 600;
const float  CAMERA_DISTANCE = 2.0f;
const int    TEXT_WIDTH      = 8;
const int    TEXT_HEIGHT     = 13;
const int    IMAGE_WIDTH     = 2048;
const int    IMAGE_HEIGHT    = 2048;
const int    CHANNEL_COUNT   = 4;
const int    DATA_SIZE       = IMAGE_WIDTH * IMAGE_HEIGHT * CHANNEL_COUNT;
const GLenum PIXEL_FORMAT    = GL_BGRA;

// global variables
//void *font = GLUT_BITMAP_8_BY_13;
GLuint pboIds[2];                   // IDs of PBO
GLuint textureId;                   // ID of texture
GLubyte* imageData = 0;             // pointer to texture buffer
int screenWidth;
int screenHeight;
bool mouseLeftDown;
bool mouseRightDown;
float mouseX, mouseY;
float cameraAngleX;
float cameraAngleY;
float cameraDistance;
bool pboSupported;
int pboMode=0;
int drawMode = 0;
Timer timer, t1, t2;
float copyTime, updateTime;

EGLDisplay eglDisplay;
EGLSurface eglSurface;

struct WaylandGlobals {
    struct wl_compositor* compositor;
    struct wl_shell* shell;
    struct wl_seat *seat;
    struct wl_keyboard *keyboard;
    struct wl_pointer *pointer;
};




///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    initSharedMem();

    char input[16] = {0};
    int num = 0;


    // register exit callback
    //atexit(exitCB);

    // init GLUT and GL
    //initGLUT(argc, argv);
    
    struct wl_display* wlDisplay;
    initWindow(SCREEN_WIDTH,SCREEN_HEIGHT, &wlDisplay);

    initGL();

    // get OpenGL extensions
    glExtension& ext = glExtension::getInstance();
    ext.printExtensions();

    //pboSupported = ext.isSupported("GL_ARB_pixel_buffer_object");
    pboSupported = true;
    if(pboSupported)
    {
        std::cout << "Video card supports GL_ARB_pixel_buffer_object." << std::endl;
    }
    else
    {
        std::cout << "[ERROR] Video card does not supports GL_ARB_pixel_buffer_object." << std::endl;
    }

    // init 2 texture objects
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, IMAGE_WIDTH, IMAGE_HEIGHT, 0, PIXEL_FORMAT, GL_UNSIGNED_BYTE, (GLvoid*)imageData);
    glBindTexture(GL_TEXTURE_2D, 0);

    if(pboSupported)
    {
        // create 2 pixel buffer objects, you need to delete them when program exits.
        // glBufferData() with NULL pointer reserves only memory space.
        glGenBuffers(2, pboIds);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[0]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[1]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }


    // start timer, the elapsed time will be used for updateVertices()
    //timer.start();

    setCamera(-1.0f, -1.0f, -1.0f, 0, 0, 0);

    while(strcmp(input,"stop") !=0)
    {


        wl_display_dispatch_pending(wlDisplay);
        

        //glClearColor(1.0f,0.3f,0.1f,1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);
        //eglSwapBuffers(eglDisplay, eglSurface);


        displayUpdate();

        //eglSwapBuffers(eglDisplay,eglSurface);


        /*if (num >= 5) {
            memset(input, 0x00, 16);
            printf("if you want to close window,input 'stop'.\n please input string:");
           std::cin >> input;
        }
        num++;
        */
    }

    
    clearSharedMem();


    // the last GLUT call (LOOP)
    // window will be shown and display callback is triggered by events
    // NOTE: this call never return main().
    //glutMainLoop(); /* Start GLUT event-processing loop */



    return 0;
}

/*///////////////////////////////////////////////////////////////////////////////
// initialize GLUT for windowing
///////////////////////////////////////////////////////////////////////////////
int initGLUT(int argc, char **argv)
{
    // GLUT stuff for windowing
    // initialization openGL window.
    // it is called before any other GLUT routine
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_ALPHA); // display mode

    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);    // window size

    glutInitWindowPosition(100, 100);                   // window location

    // finally, create a window with openGL context
    // Window will not displayed until glutMainLoop() is called
    // it returns a unique ID
    int handle = glutCreateWindow(argv[0]);     // param is the title of window

    // register GLUT callback functions
    glutDisplayFunc(displayCB);
    //glutTimerFunc(33, timerCB, 33);             // redraw only every given millisec
    glutIdleFunc(idleCB);                       // redraw only every given millisec
    glutReshapeFunc(reshapeCB);
    glutKeyboardFunc(keyboardCB);
    glutMouseFunc(mouseCB);
    glutMotionFunc(mouseMotionCB);

    return handle;
}

*/




// Bind the basic interface
void registry_global(void* data, struct wl_registry* registry,
                            uint32_t id, const char* interface, uint32_t version)
{
    printf("interface = %s\n",interface);
    struct WaylandGlobals* globals = (struct WaylandGlobals *)data;
    if (strcmp(interface, "wl_compositor") == 0) {
        globals->compositor = (struct wl_compositor *)wl_registry_bind(registry, id, &wl_compositor_interface, 1);
    } else if (strcmp(interface, "wl_shell") == 0) {
        globals->shell = (struct wl_shell *)wl_registry_bind(registry, id, &wl_shell_interface, 1);
    }
}
const struct wl_registry_listener registry_listener = { registry_global, NULL };


// Wayland Init
void initWaylandDisplay(struct wl_display** wlDisplay, struct wl_surface** wlSurface)
{
    struct WaylandGlobals globals = {0};
    // Link wayland service
    *wlDisplay = wl_display_connect(NULL);
    assert(*wlDisplay != NULL);
    //Get all the basic interfaces provided by the server
    struct wl_registry* registry = wl_display_get_registry(*wlDisplay);
    wl_registry_add_listener(registry, &registry_listener, (void *) &globals);
    printf("wl_registry_add_listener : OK\n");

    wl_display_dispatch(*wlDisplay);
    wl_display_roundtrip(*wlDisplay);
    assert(globals.compositor);
    assert(globals.shell);
    // Create surface
    *wlSurface = wl_compositor_create_surface(globals.compositor);
    assert(*wlSurface != NULL);

    struct wl_shell_surface* shellSurface = wl_shell_get_shell_surface(globals.shell, *wlSurface);
    wl_shell_surface_set_toplevel(shellSurface);
}

// EGL Init
void initEGLDisplay(EGLNativeDisplayType nativeDisplay, EGLNativeWindowType nativeWindow,
                    EGLDisplay* eglDisp, EGLSurface* eglSurf)
{
    EGLint number_of_config;
    // Choose the optimal configuration based on the specified attributes
    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };
    /*EGLint config_attribs2[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };*/

    const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };

    // Get the foreground window and initialize Display
    *eglDisp = eglGetDisplay(nativeDisplay);
    assert(*eglDisp != EGL_NO_DISPLAY);
    EGLBoolean initialized = eglInitialize(*eglDisp, NULL, NULL);
    assert(initialized == EGL_TRUE);

    EGLConfig configs[1];
    //EGLConfig configs2[1];

    eglChooseConfig(*eglDisp, config_attribs, configs, 1, &number_of_config);
    assert(number_of_config);
    // Create EGL context with configuration
    EGLContext eglContext = eglCreateContext(*eglDisp, configs[0], EGL_NO_CONTEXT, context_attribs);
    // Create Surface with selected translation configuration
    *eglSurf = eglCreateWindowSurface(*eglDisp, configs[0], nativeWindow, NULL);
    assert(*eglSurf != EGL_NO_SURFACE);
    switch( eglGetError())
    {
        case EGL_SUCCESS:
            std::cout<<"NO ERROR"<<std::endl;
            break;
        case EGL_BAD_MATCH:
            std::cout<<"ERROR IS: EGL_BAD_MATCH"<<std::endl;
        break;
        case EGL_BAD_NATIVE_WINDOW:
            std::cout<<"ERROR IS: EGL_BAD_NATIVE_WINDOW"<<std::endl;
        break;

        case EGL_BAD_ALLOC:
            std::cout<<"ERROR IS: EGL_BAD_ALLOC"<<std::endl;
        break;
    }

    EGLBoolean makeCurrent = eglMakeCurrent(*eglDisp, *eglSurf, *eglSurf, eglContext);
    assert(makeCurrent == EGL_TRUE);
    std::cout<<"Make Current Error Check is: ";
    switch( eglGetError())
    {
        case EGL_SUCCESS:
            std::cout<<"NO ERROR"<<std::endl;
            break;
        case EGL_BAD_MATCH:
            std::cout<<"ERROR IS: EGL_BAD_MATCH"<<std::endl;
        break;
        case EGL_BAD_NATIVE_WINDOW:
            std::cout<<"ERROR IS: EGL_BAD_NATIVE_WINDOW"<<std::endl;
        break;

        case EGL_BAD_ALLOC:
            std::cout<<"ERROR IS: EGL_BAD_ALLOC"<<std::endl;
        break;
    }

   /* eglChooseConfig(*eglDisp, config_attribs2, configs2, 1, &number_of_config);
    assert(number_of_config);
    EGLContext eglcontext2 = eglCreateContext(*eglDisp, configs2[0], EGL_NO_CONTEXT,context_attribs);
    */

}


void initWindow(GLint width, GLint height, struct wl_display** wlDisplay)
{
    struct wl_surface* wlSurface;
    initWaylandDisplay(wlDisplay, &wlSurface);

    struct wl_egl_window* wlEglWindow = wl_egl_window_create(wlSurface, width, height);
    assert(wlEglWindow != NULL);

    initEGLDisplay((EGLNativeDisplayType) *wlDisplay, (EGLNativeWindowType) wlEglWindow, &eglDisplay, &eglSurface);
}

///////////////////////////////////////////////////////////////////////////////
// initialize OpenGL
// disable unused features
///////////////////////////////////////////////////////////////////////////////
void initGL()
{
    //@glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT
    glShadeModel(GL_FLAT);                      // shading mathod: GL_SMOOTH or GL_FLAT
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment

    // enable /disable features
    //@glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    //@glEnable(GL_LIGHTING);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);

     // track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glClearColor(0, 0, 0, 0);                   // background color
    glClearStencil(0);                          // clear stencil buffer
    glClearDepth(1.0f);                         // 0 is near, 1 is far
    glDepthFunc(GL_LEQUAL);

    //@initLights();
}



///////////////////////////////////////////////////////////////////////////////
// write 2d text using GLUT
// The projection matrix must be set to orthogonal before call this function.
///////////////////////////////////////////////////////////////////////////////
//void drawString(const char *str, int x, int y, float color[4], void *font)
void drawString(const char *str, int x, int y, float color[4])
{
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
    glDisable(GL_TEXTURE_2D);

    glColor4fv(color);          // set text color
    glRasterPos2i(x, y);        // place text position

    // loop all characters in the string
    while(*str)
    /*{
        glutBitmapCharacter(font, *str);
        ++str;
    }*/

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopAttrib();
}



///////////////////////////////////////////////////////////////////////////////
// draw a string in 3D space
///////////////////////////////////////////////////////////////////////////////
//void drawString3D(const char *str, float pos[3], float color[4], void *font)
void drawString3D(const char *str, float pos[3], float color[4])
{
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
    glDisable(GL_TEXTURE_2D);

    glColor4fv(color);          // set text color
    glRasterPos3fv(pos);        // place text position

    // loop all characters in the string
    /*while(*str)
    {
        glutBitmapCharacter(font, *str);
        ++str;
    }*/

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopAttrib();
}



///////////////////////////////////////////////////////////////////////////////
// initialize global variables
///////////////////////////////////////////////////////////////////////////////
bool initSharedMem()
{
    screenWidth = SCREEN_WIDTH;
    screenHeight = SCREEN_HEIGHT;

    mouseLeftDown = mouseRightDown = false;
    mouseX = mouseY = 0;

    cameraAngleX = cameraAngleY = 0;
    cameraDistance = CAMERA_DISTANCE;

    drawMode = 0; // 0:fill, 1: wireframe, 2:points

    // allocate texture buffer
    imageData = new GLubyte[DATA_SIZE];
    memset(imageData, 0, DATA_SIZE);

    return true;
}



///////////////////////////////////////////////////////////////////////////////
// clean up shared memory
///////////////////////////////////////////////////////////////////////////////
void clearSharedMem()
{
    // deallocate texture buffer
    delete [] imageData;
    imageData = 0;

    // clean up texture
    glDeleteTextures(1, &textureId);

    // clean up PBOs
    if(pboSupported)
    {
        glDeleteBuffers(2, pboIds);
    }
}



///////////////////////////////////////////////////////////////////////////////
// initialize lights
///////////////////////////////////////////////////////////////////////////////
void initLights()
{
    // set up light colors (ambient, diffuse, specular)
    GLfloat lightKa[] = {.2f, .2f, .2f, 1.0f};  // ambient light
    GLfloat lightKd[] = {.7f, .7f, .7f, 1.0f};  // diffuse light
    GLfloat lightKs[] = {1, 1, 1, 1};           // specular light
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

    // position the light
    float lightPos[4] = {0, 0, 20, 1}; // positional light
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_LIGHT0);                        // MUST enable each light source after configuration
}



///////////////////////////////////////////////////////////////////////////////
// set camera position and lookat direction
///////////////////////////////////////////////////////////////////////////////
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(posX, posY, posZ, targetX, targetY, targetZ, 0, 1, 0); // eye(x,y,z), focal(x,y,z), up(x,y,z)
}



///////////////////////////////////////////////////////////////////////////////
// copy an image data to texture buffer
///////////////////////////////////////////////////////////////////////////////
void updatePixels(GLubyte* dst, int size)
{
    static int color = 0;
    std::cout<<"DST: "<<(int)(*dst)<<std::endl;
    if(!dst)
        return;

    int* ptr = (int*)dst;

    // copy 4 bytes at once
    for(int i = 0; i < IMAGE_HEIGHT; ++i)
    {
        for(int j = 0; j < IMAGE_WIDTH; ++j)
        {
            *ptr = color;
            ++ptr;
        }
        color += 257;
    }
    ++color;            // scroll down
}



///////////////////////////////////////////////////////////////////////////////
// display info messages
///////////////////////////////////////////////////////////////////////////////
void showInfo()
{
    // backup current model-view matrix
    glPushMatrix();                     // save current modelview matrix
    glLoadIdentity();                   // reset modelview matrix

    // set to 2D orthogonal projection
    glMatrixMode(GL_PROJECTION);     // switch to projection matrix
    glPushMatrix();                  // save current projection matrix
    glLoadIdentity();                // reset projection matrix
    gluOrtho2D(0, screenWidth, 0, screenHeight); // set to orthogonal projection

    float color[4] = {1, 1, 1, 1};

    std::stringstream ss;
    ss << "PBO: ";
    if(pboMode == 0)
        ss << "off" << std::ends;
    else if(pboMode == 1)
        ss << "1 PBO" << std::ends;
    else if(pboMode == 2)
        ss << "2 PBOs" << std::ends;

    //drawString(ss.str().c_str(), 1, screenHeight-TEXT_HEIGHT, color, font);
    drawString(ss.str().c_str(), 1, screenHeight-TEXT_HEIGHT, color);
    ss.str(""); // clear buffer

    ss << std::fixed << std::setprecision(3);
    ss << "Updating Time: " << updateTime << " ms" << std::ends;
    //drawString(ss.str().c_str(), 1, screenHeight-(2*TEXT_HEIGHT), color, font);
    drawString(ss.str().c_str(), 1, screenHeight-(2*TEXT_HEIGHT), color);
    ss.str("");

    ss << "Copying Time: " << copyTime << " ms" << std::ends;
    //drawString(ss.str().c_str(), 1, screenHeight-(3*TEXT_HEIGHT), color, font);
    drawString(ss.str().c_str(), 1, screenHeight-(3*TEXT_HEIGHT), color);
    ss.str("");

    ss << "Press SPACE key to toggle PBO on/off." << std::ends;
    //drawString(ss.str().c_str(), 1, 1, color, font);
    drawString(ss.str().c_str(), 1, 1, color);

    // unset floating format
    ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);

    // restore projection matrix
    glPopMatrix();                   // restore to previous projection matrix

    // restore modelview matrix
    glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
    glPopMatrix();                   // restore to previous modelview matrix
}



///////////////////////////////////////////////////////////////////////////////
// display transfer rates
///////////////////////////////////////////////////////////////////////////////
void showTransferRate()
{
    static Timer timer;
    static int count = 0;
    static std::stringstream ss;

    // update fps every second
    ++count;
    double elapsedTime = timer.getElapsedTime();
    if(elapsedTime > 1.0)
    {
        ss.str("");
        ss << std::fixed << std::setprecision(1);
        ss << "Transfer Rate: " << (count / elapsedTime) * DATA_SIZE / (1024 * 1024) << " MB" << std::ends; // update fps string
        ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
        count = 0;                      // reset counter
        timer.start();                  // restart timer
    }

    // backup current model-view matrix
    glPushMatrix();                     // save current modelview matrix
    glLoadIdentity();                   // reset modelview matrix

    // set to 2D orthogonal projection
    glMatrixMode(GL_PROJECTION);        // switch to projection matrix
    glPushMatrix();                     // save current projection matrix
    glLoadIdentity();                   // reset projection matrix
    //gluOrtho2D(0, IMAGE_WIDTH, 0, IMAGE_HEIGHT); // set to orthogonal projection
    gluOrtho2D(0, screenWidth, 0, screenHeight); // set to orthogonal projection

    float color[4] = {1, 1, 0, 1};
    //drawString(ss.str().c_str(), 200, 286, color, font);
    drawString(ss.str().c_str(), 200, 286, color);

    // restore projection matrix
    glPopMatrix();                      // restore to previous projection matrix

    // restore modelview matrix
    glMatrixMode(GL_MODELVIEW);         // switch to modelview matrix
    glPopMatrix();                      // restore to previous modelview matrix
}



///////////////////////////////////////////////////////////////////////////////
// print transfer rates
///////////////////////////////////////////////////////////////////////////////
void printTransferRate()
{
    const double INV_MEGA = 1.0 / (1024 * 1024);
    static Timer timer;
    static int count = 0;
    static std::stringstream ss;
    double elapsedTime;

    // loop until 1 sec passed
    ++count;
    elapsedTime = timer.getElapsedTime();
    if(elapsedTime > 1.0)
    {
        std::cout << std::fixed << std::setprecision(1);
        std::cout << "Transfer Rate: " << (count / elapsedTime) * DATA_SIZE * INV_MEGA << " MB/s. (" << count / elapsedTime << " FPS)\n";
        std::cout << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
        count = 0;                      // reset counter
        timer.start();                  // restart timer
    }
}



///////////////////////////////////////////////////////////////////////////////
// set projection matrix as orthogonal
///////////////////////////////////////////////////////////////////////////////
void toOrtho()
{
    // set viewport to be the entire window
    glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

    // set orthographic viewing frustum
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, screenWidth, 0, screenHeight, -1, 1);

    // switch to modelview matrix in order to set scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



///////////////////////////////////////////////////////////////////////////////
// set the projection matrix as perspective
///////////////////////////////////////////////////////////////////////////////
void toPerspective()
{
    // set viewport to be the entire window
    glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

    // set perspective viewing frustum
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (float)(screenWidth)/screenHeight, 1.0f, 1000.0f); // FOV, AspectRatio, NearClip, FarClip

    // switch to modelview matrix in order to set scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

///=============================================================================
///  REPlACE DISPLAYCB()
///=============================================================================
   
void displayUpdate()
{
    static int index = 0;
    int nextIndex = 0;                  // pbo index used for next frame
    if(pboMode > 0)
    {

        // "index" is used to copy pixels from a PBO to a texture object
        // "nextIndex" is used to update pixels in a PBO
        if(pboMode == 1)
        {
            // In single PBO mode, the index and nextIndex are set to 0
            index = nextIndex = 0;
        }
        else if(pboMode == 2)
        {
            // In dual PBO mode, increment current index first then get the next index
            index = (index + 1) % 2;
            nextIndex = (index + 1) % 2;
        }

        // start to copy from PBO to texture object ///////
        //t1.start();

        // bind the texture and PBO
        glBindTexture(GL_TEXTURE_2D, textureId);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[index]);

        // copy pixels from PBO to texture object
        // Use offset instead of ponter.
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, PIXEL_FORMAT, GL_UNSIGNED_BYTE, 0);

        // measure the time copying data from PBO to texture object
        ///t1.stop();
        //copyTime = t1.getElapsedTimeInMilliSec();
        ///////////////////////////////////////////////////


        // start to modify pixel values ///////////////////
        //t1.start();

        // bind PBO to update pixel values
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[nextIndex]);

        // map the buffer object into client's memory
        // Note that glMapBuffer() causes sync issue.
        // If GPU is working with this buffer, glMapBuffer() will wait(stall)
        // for GPU to finish its job. To avoid waiting (stall), you can call
        // first glBufferData() with NULL pointer before glMapBuffer().
        // If you do that, the previous data in PBO will be discarded and
        // glMapBuffer() returns a new allocated pointer immediately
        // even if GPU is still working with the previous data.
        glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
        if(ptr)
        {
            // update data directly on the mapped buffer
            updatePixels(ptr, DATA_SIZE);
            glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);  // release pointer to mapping buffer
        }

        // measure the time modifying the mapped buffer
        //t1.stop();
        //updateTime = t1.getElapsedTimeInMilliSec();
        ///////////////////////////////////////////////////

        // it is good idea to release PBOs with ID 0 after use.
        // Once bound with 0, all pixel operations behave normal ways.
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }
    else
    {
        ///////////////////////////////////////////////////
        // start to copy pixels from system memory to textrure object
        //t1.start();

        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, PIXEL_FORMAT, GL_UNSIGNED_BYTE, (GLvoid*)imageData);

        //t1.stop();
        //copyTime = t1.getElapsedTimeInMilliSec();
        ///////////////////////////////////////////////////


        // start to modify pixels /////////////////////////
        //t1.start();
        updatePixels(imageData, DATA_SIZE);
        //t1.stop();
        //updateTime = t1.getElapsedTimeInMilliSec();
        ///////////////////////////////////////////////////
    }


    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // save the initial ModelView matrix before modifying ModelView matrix
    glPushMatrix();

    // tramsform camera
    glTranslatef(0, 0, -cameraDistance);
    glRotatef(cameraAngleX, 1, 0, 0);   // pitch
    glRotatef(cameraAngleY, 0, 1, 0);   // heading

    // draw a point with texture
    glBindTexture(GL_TEXTURE_2D, textureId);
    glColor4f(1, 1, 1, 1);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);   glVertex3f( 1.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);   glVertex3f( 1.0f,  1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f,  1.0f, 0.0f);
    glEnd();


    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // draw info messages
    //showInfo();
    //showTransferRate();
    //printTransferRate();

    glPopMatrix();

    //glutSwapBuffers();
    eglSwapBuffers(eglDisplay,eglSurface);
}


//=============================================================================
// CALLBACKS
//=============================================================================
/*
void displayCB()
{
    static int index = 0;
    int nextIndex = 0;                  // pbo index used for next frame

    if(pboMode > 0)
    {
        // "index" is used to copy pixels from a PBO to a texture object
        // "nextIndex" is used to update pixels in a PBO
        if(pboMode == 1)
        {
            // In single PBO mode, the index and nextIndex are set to 0
            index = nextIndex = 0;
        }
        else if(pboMode == 2)
        {
            // In dual PBO mode, increment current index first then get the next index
            index = (index + 1) % 2;
            nextIndex = (index + 1) % 2;
        }

        // start to copy from PBO to texture object ///////
        t1.start();

        // bind the texture and PBO
        glBindTexture(GL_TEXTURE_2D, textureId);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[index]);

        // copy pixels from PBO to texture object
        // Use offset instead of ponter.
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, PIXEL_FORMAT, GL_UNSIGNED_BYTE, 0);

        // measure the time copying data from PBO to texture object
        t1.stop();
        copyTime = t1.getElapsedTimeInMilliSec();
        ///////////////////////////////////////////////////


        // start to modify pixel values ///////////////////
        t1.start();

        // bind PBO to update pixel values
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[nextIndex]);

        // map the buffer object into client's memory
        // Note that glMapBuffer() causes sync issue.
        // If GPU is working with this buffer, glMapBuffer() will wait(stall)
        // for GPU to finish its job. To avoid waiting (stall), you can call
        // first glBufferData() with NULL pointer before glMapBuffer().
        // If you do that, the previous data in PBO will be discarded and
        // glMapBuffer() returns a new allocated pointer immediately
        // even if GPU is still working with the previous data.
        glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
        if(ptr)
        {
            // update data directly on the mapped buffer
            updatePixels(ptr, DATA_SIZE);
            glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);  // release pointer to mapping buffer
        }

        // measure the time modifying the mapped buffer
        t1.stop();
        updateTime = t1.getElapsedTimeInMilliSec();
        ///////////////////////////////////////////////////

        // it is good idea to release PBOs with ID 0 after use.
        // Once bound with 0, all pixel operations behave normal ways.
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }
    else
    {
        ///////////////////////////////////////////////////
        // start to copy pixels from system memory to textrure object
        t1.start();

        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, PIXEL_FORMAT, GL_UNSIGNED_BYTE, (GLvoid*)imageData);

        t1.stop();
        copyTime = t1.getElapsedTimeInMilliSec();
        ///////////////////////////////////////////////////


        // start to modify pixels /////////////////////////
        t1.start();
        updatePixels(imageData, DATA_SIZE);
        t1.stop();
        updateTime = t1.getElapsedTimeInMilliSec();
        ///////////////////////////////////////////////////
    }


    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // save the initial ModelView matrix before modifying ModelView matrix
    glPushMatrix();

    // tramsform camera
    glTranslatef(0, 0, -cameraDistance);
    glRotatef(cameraAngleX, 1, 0, 0);   // pitch
    glRotatef(cameraAngleY, 0, 1, 0);   // heading

    // draw a point with texture
    glBindTexture(GL_TEXTURE_2D, textureId);
    glColor4f(1, 1, 1, 1);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);   glVertex3f( 1.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);   glVertex3f( 1.0f,  1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f,  1.0f, 0.0f);
    glEnd();

    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // draw info messages
    showInfo();
    //showTransferRate();
    printTransferRate();

    glPopMatrix();

    //glutSwapBuffers();
    eglSwapBuffers(eglDisplay,eglSurface);

}
*/

/*
void reshapeCB(int width, int height)
{
    screenWidth = width;
    screenHeight = height;
    toPerspective();
}


void timerCB(int millisec)
{
    glutTimerFunc(millisec, timerCB, millisec);
    glutPostRedisplay();
}


void idleCB()
{
    glutPostRedisplay();
}


void keyboardCB(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 27: // ESCAPE
        exit(0);
        break;

    case ' ':
        if(pboSupported)
        {
            ++pboMode;
            pboMode %= 3;
        }
        std::cout << "PBO mode: " << pboMode << std::endl;
        break;

    case 'd': // switch rendering modes (fill -> wire -> point)
    case 'D':
        ++drawMode;
        drawMode %= 3;
        if(drawMode == 0)        // fill mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
        }
        else if(drawMode == 1)  // wireframe mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        else                    // point mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        break;

    default:
        ;
    }
}


void mouseCB(int button, int state, int x, int y)
{
    mouseX = x;
    mouseY = y;

    if(button == GLUT_LEFT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseLeftDown = true;
        }
        else if(state == GLUT_UP)
            mouseLeftDown = false;
    }

    else if(button == GLUT_RIGHT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseRightDown = true;
        }
        else if(state == GLUT_UP)
            mouseRightDown = false;
    }
}


void mouseMotionCB(int x, int y)
{
    if(mouseLeftDown)
    {
        cameraAngleY += (x - mouseX);
        cameraAngleX += (y - mouseY);
        mouseX = x;
        mouseY = y;
    }
    if(mouseRightDown)
    {
        cameraDistance -= (y - mouseY) * 0.2f;
        if(cameraDistance < 2.0f)
            cameraDistance = 2.0f;

        mouseY = y;
    }
}



void exitCB()
{
    clearSharedMem();
}
*/