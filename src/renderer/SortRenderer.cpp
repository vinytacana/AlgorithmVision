#include "SortRenderer.h"

#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

SortRenderer::SortRenderer() {
    setupMesh();
}

SortRenderer::~SortRenderer() {
    if (vbo != 0) glDeleteBuffers(1, &vbo);
    if (vao != 0) glDeleteVertexArrays(1, &vao);
}

void SortRenderer::setupMesh() {
    const float vertices[] = {0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0};
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void SortRenderer::render(const SortEngine& engine, Shader& shader, int screenWidth, int screenHeight) {
    if (screenWidth <= 0 || screenHeight <= 0) return;

    shader.use();
    const float viewportWidth = static_cast<float>(screenWidth);
    const float viewportHeight = static_cast<float>(screenHeight);
    const glm::mat4 projection = glm::ortho(0.0f, viewportWidth, 0.0f, viewportHeight, -1.0f, 1.0f);
    shader.setMat4("projection", projection);

    glBindVertexArray(vao);

    const auto& data = engine.getData();
    const auto& visualState = engine.getVisualState();
    const float centerX = viewportWidth / 2.0f;
    const float centerY = viewportHeight / 2.0f;
    const float radiusFactor = std::min(viewportWidth, viewportHeight) * 0.4f;

    for (int i = 0; i < static_cast<int>(data.size()); ++i) {
        glm::mat4 model = glm::mat4(1.0f);

        if (engine.getRenderMode() == BARS) {
            const float barWidth = viewportWidth / static_cast<float>(data.size());
            const float heightScale = viewportHeight / static_cast<float>(data.size());
            model = glm::translate(model, glm::vec3(i * barWidth, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(std::max(1.0f, barWidth - 0.5f), data[i] * heightScale, 1.0f));
        } else if (engine.getRenderMode() == DOTS) {
            const float dotX = (static_cast<float>(i) / static_cast<float>(data.size())) * viewportWidth;
            const float dotY = (static_cast<float>(data[i]) / static_cast<float>(data.size())) * viewportHeight;
            model = glm::translate(model, glm::vec3(dotX, dotY, 0.0f));
            model = glm::scale(model, glm::vec3(3.0f, 3.0f, 1.0f));
        } else if (engine.getRenderMode() == CIRCULAR) {
            const float angle = (static_cast<float>(i) / static_cast<float>(data.size())) * 2.0f * static_cast<float>(M_PI);
            const float radius = (static_cast<float>(data[i]) / static_cast<float>(data.size())) * radiusFactor;
            model = glm::translate(model, glm::vec3(centerX, centerY, 0.0f));
            model = glm::rotate(model, angle, glm::vec3(0, 0, 1));
            model = glm::scale(model, glm::vec3(2.0f, radius, 1.0f));
        }

        shader.setMat4("model", model);

        int state = 0;
        if (i <= visualState.finishIdx) state = 5;
        else if (i == visualState.comp1 || i == visualState.comp2) state = 1;
        else if (i == visualState.write1 || i == visualState.write2) state = 2;
        else if (i == visualState.pivotIdx) state = 3;
        else if (i >= visualState.rangeL && i <= visualState.rangeR) state = 4;

        shader.setInt("state", state);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
}
