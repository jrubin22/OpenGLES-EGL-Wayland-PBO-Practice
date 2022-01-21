### PBO Example

#### Getting to run

- Make clean then make found correct libraries on my x86 machine
- Got to test run it and playaround with moving textured shape inside my window
- Going to continue on to trying to port to SOM by using wayland and egl


#### Port to SOM

##### Change findGL.mk and makefile to aarch64 instead of x86_64
###### findGL.mk
- Change GL_LIB_PATH to look in aarch64 instead of x86
- Add GLES3 and GLES2 header paths instead of just GL header path, and libwayland headers as well
- make new header variables
- add new error statements in case the headers aren't found
- Add new libpath for libGLESv2.so, libEGL.so, and all the libwaylands libraries to be found to be found
- change LIBDIR to aarch64-linux-gnu

##### Change C code to incorporate EGL and Wayland instead of GLUT
- First add in includes for GLES3, EGL, and Wayland

- GLUT not found on SOM, so will have to replace GLUT code with EGL and WL code

- define function prototypes for registry_global, initWaylandDisplay, initEGLDisplay, and initWindow

- define EGLDisplay and EGLSurface in global variables

- in main function define wl_display* wlDisplay, and call initWindow with width, height, and a reference to wlDisplay

- in displayCB, switch glutSwapBuffers to eglSwapBuffers

- comment out references to font and drawing letters, in drawstring and drawstring3d especially, need to get rid of all glut

###### Exchange glutMainLoop() for normal While() loop

	- Need to reformat loop so instead of glutCB loop, just uses normal while loop for egl
		* Add while input != stop
		* comment out glutMainLoop()
		* add clearshared mem after while loop exits
		* comment out all glutCB functions -> need to go back and add functionality for the changePBO mode
		* comment out displayCB function, and create new displayUpdate function to call inside main while loop, change glut calls to equivalent egl calls.  Comment out all timer calls -> will go back to try replacing these with egl timer calls
