#ifndef GL_LOADER_H
#define GL_LOADER_H

#if __has_include(<glad/gl.h>)
#include <glad/gl.h>
#define ALGORITHM_VISION_HAS_GLAD 1
#else
#include <GL/glew.h>
#define ALGORITHM_VISION_HAS_GLAD 0
#endif

#endif
