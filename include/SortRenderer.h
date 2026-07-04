#ifndef SORT_RENDERER_H
#define SORT_RENDERER_H

#include "SortEngine.h"
#include "Shader.h"

class SortRenderer {
public:
    SortRenderer();
    ~SortRenderer();
    SortRenderer(const SortRenderer&) = delete;
    SortRenderer& operator=(const SortRenderer&) = delete;

    void render(const SortEngine& engine, Shader& shader, int screenWidth, int screenHeight);

private:
    unsigned int vao = 0;
    unsigned int vbo = 0;

    void setupMesh();
};

#endif
