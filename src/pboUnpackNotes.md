# pboUnpack Notes
## Installation
### The Linux Makefile

The initial archive comes with a Linux Makefile that has nothing in it. I create a Makefile that works and a file `findGL.mk` that finds the openGL header and library files on your machine.

Things to look at in `findGL.mk`:

* `GL_HEADER_PATH`: This is defined for my machine, you might have to look on yours
* `GL_LIB_PATH`: This is definted for my machine, you might have to look on yours 


### glGenBuffers
This code uses the openGL command glGenBuffers. To work, this command requires the 
glext.h header, which in turn requires the variable `#define GL_GLEXT_PROTOTYPES`
be defined. The code below DID NOT work, and make threw an error claiming glGenBuffers
is not declared in scope.

```cpp
#include <GL/glut.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include "glExtension.h"                        // glInfo struct
#include "Timer.h"

```

It turns out that while `glExension.h` does define `GL_GLEXT_PROTOTYPES` this definition needs to occurs before `glut.h` is included. So, this works...

```cpp
#define GL_GLEXT_PROTOTYPES

#include <GL/glut.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include "glExtension.h"                        // glInfo struct
#include "Timer.h"
```

And this works...

```cpp
#include "glExtension.h"                        // glInfo struct
#include <GL/glut.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include "Timer.h"
```

Super annoying.
