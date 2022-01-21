# Find OpenGL Header files
GL_HEADER_PATH=/usr/include/GL
GLES3_HEADER_PATH=/usr/include/GLES3
GLES2_HEADER_PATH=/usr/include/GLES2
EGL_HEADER_PATH=/usr/include/EGL
WAYLAND_HEADER_PATH=/usr/include/


# find libGL, libGLU, libglut, libXi, 
GL_LIB_PATH=/usr/lib/aarch64-linux-gnu

GLHEADER   := $(shell find -L $(GL_HEADER_PATH) -name gl.h -print 2>/dev/null)
GLUHEADER  := $(shell find -L $(GL_HEADER_PATH) -name glu.h -print 2>/dev/null)
GLES3HEADER := $(shell find -L $(GLES3_HEADER_PATH) -name gl32.h -print 2>/dev/null)
GLES3EXTHEADER := $(shell find -L $(GLES3_HEADER_PATH) -name gl3ext.h -print 2>/dev/null)
EGLHEADER := $(shell find -L $(EGL_HEADER_PATH) -name egl.h -print 2>/dev/null)
WAYLANDCLIENTHEADER := $(shell find -L $(WAYLAND_HEADER_PATH) -name wayland-client.h -print 2>/dev/null)
WAYLANDCURSORHEADER := $(shell find -L $(WAYLAND_HEADER_PATH) -name wayland-cursor.h -print 2>/dev/null)
WAYLANDEGLHEADER := $(shell find -L $(WAYLAND_HEADER_PATH) -name wayland-egl.h -print 2>/dev/null)
WAYLANDSERVERHEADER := $(shell find -L $(WAYLAND_HEADER_PATH) -name wayland-server.h -print 2>/dev/null)


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

ifeq ("$(GLES3HEADER)","")
    $(error ERROR - gl32.h not found)
else
    $(info INFO - gl32.h found at $(GLES3HEADER))
endif

ifeq ("$(GLES3EXTHEADER)","")
    $(error ERROR - gl3ext.h not found)
else
    $(info INFO - gl3ext.h found at $(GLES3HEADER))
endif


ifeq ("$(EGLHEADER)","")
    $(error ERROR - egl.h not found)
else
    $(info INFO - egl.h found at $(EGLHEADER))
endif

ifeq ("$(WAYLANDCLIENTHEADER)","")
    $(error ERROR - wayland-client.h not found)
else
    $(info INFO - wayland-client.h found at $(WAYLANDCLIENTHEADER))
endif

ifeq ("$(WAYLANDCURSORHEADER)","")
    $(error ERROR - wayland-cursor.h not found)
else
    $(info INFO - wayland-cursor.h found at $(WAYLANDCURSORHEADER))
endif

ifeq ("$(WAYLANDEGLHEADER)","")
    $(error ERROR - wayland-egl.h not found)
else
    $(info INFO - wayland-egl.h found at $(WAYLANDEGLHEADER))
endif

ifeq ("$(WAYLANDSERVERHEADER)","")
    $(error ERROR - wayland-server.h not found)
else
    $(info INFO - wayland-server.h found at $(WAYLANDSERVERHEADER))
endif




# All tests pass so include head path
INC += -I$(GL_HEADER_PATH)


GLLIB   := $(shell find -L $(GL_LIB_PATH) -name libGL.so  -print 2>/dev/null)
GLULIB  := $(shell find -L $(GL_LIB_PATH) -name libGLU.so -print 2>/dev/null)
GLES2LIB := $(shell find -L $(GL_LIB_PATH) -name libGLESv2.so -print 2>/dev/null)
EGLLIB := $(shell find -L $(GL_LIB_PATH) -name libEGL.so -print 2>/dev/null)
WLCLIENTLIB := $(shell find -L $(GL_LIB_PATH) -name libwayland-client.so -print 2>/dev/null)
WLCURSORLIB := $(shell find -L $(GL_LIB_PATH) -name libwayland-cursor.so -print 2>/dev/null)
WLEGLLIB := $(shell find -L $(GL_LIB_PATH) -name libwayland-egl.so -print 2>/dev/null)
WLSERVERLIB := $(shell find -L $(GL_LIB_PATH) -name libwayland-server.so -print 2>/dev/null)



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
ifeq ("$(GLES2LIB)","")
    $(error ERROR - libGLESv2.so not found)
else
    $(info INFO - libGLESv2.so found at $(GLES2LIB))
endif
ifeq ("$(EGLLIB)","")
    $(error ERROR - libEGL.so not found)
else
    $(info INFO - libEGL.so found at $(EGLLIB))
endif
ifeq ("$(WLCLIENTLIB)","")
    $(error ERROR - libwayland-client.so not found)
else
    $(info INFO - libwayland-client.so found at $(WLCLIENTLIB))
endif
ifeq ("$(WLCURSORLIB)","")
    $(error ERROR - libwayland-cursor.so not found)
else
    $(info INFO - libwayland-cursor.so found at $(WLCURSORLIB))
endif
ifeq ("$(WLEGLLIB)","")
    $(error ERROR - libwayland-egl.so not found)
else
    $(info INFO - libwayland-egl.so found at $(WLEGLLIB))
endif
ifeq ("$(WLSERVERLIB)","")
    $(error ERROR - libwayland-server.so not found)
else
    $(info INFO - libwayland-server.so found at $(WLSERVERLIB))
endif





# Find OpenGL Libraries

# All tests pass so include library directory
LIBDIR += -L/usr/lib/aarch64-linux-gnu