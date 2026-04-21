#ifndef GL_LOADER_H
#define GL_LOADER_H

#if defined(ALGORITHM_VISION_USE_GLAD)
#include <glad/gl.h>
#define ALGORITHM_VISION_HAS_GLAD 1
#else
#include <GL/glew.h>
#define ALGORITHM_VISION_HAS_GLAD 0
#endif

#endif
