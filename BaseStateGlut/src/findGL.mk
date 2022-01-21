# Find OpenGL Header files
GL_HEADER_PATH=/usr/include/GL
GLES3_HEADER_PATH=/usr/include/GLES3
GLES2_HEADER_PATH=/usr/include/GLES2
EGL_HEADER_PATH=/usr/include/EGL

# find libGL, libGLU, libglut, libXi, 
GL_LIB_PATH=/usr/lib/x86_64-linux-gnu

GLHEADER   := $(shell find -L $(GL_HEADER_PATH) -name gl.h -print 2>/dev/null)
GLES3HEADER   := $(shell find -L $(GLES3_HEADER_PATH) -name gl32.h -print 2>/dev/null)
GLES3HEADER   := $(shell find -L $(GLES3_HEADER_PATH) -name gl3ext.h -print 2>/dev/null)
GLUHEADER  := $(shell find -L $(GL_HEADER_PATH) -name glu.h -print 2>/dev/null)
GLUTHEADER := $(shell find -L $(GL_HEADER_PATH) -name glut.h -print 2>/dev/null)
EGLHEADER := $(shell find -L $(EGL_HEADER_PATH) -name egl.h -print 2>/dev/null)


ifeq ("$(GLHEADER)","")
    $(error ERROR - gl.h not found)
else
    $(info INFO - gl.h found at $(GLHEADER))
endif
ifeq ("$(GLUHEADER)","")
    $(error ERROR - glu.h not found)
else
    $(info INFO - glu.h found at $(GLUHEADER))
endif
ifeq ("$(GLUTHEADER)","")
    $(error ERROR - glut.h not found)
else
    $(info INFO - glut.h found at $(GLUTHEADER))
endif

# All tests pass so include head path
INC += -I$(GL_HEADER_PATH)
INC += -I$(GLES3_HEADER_PATH)
INC += -I$(GLES2_HEADER_PATH)
INC += -I$(EGL_HEADER_PATH)


GLLIB   := $(shell find -L $(GL_LIB_PATH) -name libGL.so  -print 2>/dev/null)
GLULIB  := $(shell find -L $(GL_LIB_PATH) -name libGLU.so -print 2>/dev/null)
GLUTLIB := $(shell find -L $(GL_LIB_PATH) -name libglut.so -print 2>/dev/null)
GLES2LIB := $(shell find -L $(GL_LIB_PATH) -name libGLESv2.so -print 2>/dev/null)
EGLLIB := $(shell find -L $(GL_LIB_PATH) -name libEGL.so -print 2>/dev/null)

ifeq ("$(GLLIB)","")
    $(error ERROR - libGL.so not found)
else
    $(info INFO - libGL.so found at $(GLLIB))
endif
ifeq ("$(GLULIB)","")
    $(error ERROR - libGLU.so not found)
else
    $(info INFO - libGLU.so found at $(GLULIB))
endif
ifeq ("$(GLUTLIB)","")
    $(error ERROR - libglut.so not found)
else
    $(info INFO - libglut.so found at $(GLUTLIB))
endif
# Find OpenGL Libraries

# All tests pass so include library directory
LIBDIR += -L/usr/lib/x86_64-linux-gnu