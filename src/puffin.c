#ifdef _WIN32
    #include <GL/glew.h>
#elif __APPLE__
    #include <OpenGL/gl3.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#include "puffin.h"

#include "camera.c"
#include "mesh.c"
#include "texture.c"
#include "shader.c"
#include "helpers.c"
#include "matrix.c"
#include "loadobj.c"
#include "framebuffer.c"



