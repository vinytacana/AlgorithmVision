#include "SortSimulator.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <random>
#include <cmath>
#include <stdexcept>

SortSimulator::SortSimulator() { reset(); setupMesh(); }

SortSimulator::~SortSimulator() {
    if (vbo != 0) glDeleteBuffers(1, &vbo);
    if (vao != 0) glDeleteVertexArrays(1, &vao);
}

void SortSimulator::reset() {
    data.resize(arraySize);
    std::random_device rd;
    std::mt19937 rng(rd());

    if (distribution == RANDOM) {
        for (int i = 0; i < arraySize; ++i) data[i] = i + 1;
        std::shuffle(data.begin(), data.end(), rng);
    } else if (distribution == REVERSED) {
        for (int i = 0; i < arraySize; ++i) data[i] = arraySize - i;
    } else if (distribution == NEARLY_SORTED) {
        for (int i = 0; i < arraySize; ++i) data[i] = i + 1;
        std::uniform_int_distribution<int> dist(0, arraySize - 1);
        for (int i = 0; i < arraySize / 10; ++i) std::swap(data[dist(rng)], data[dist(rng)]);
    } else if (distribution == FEW_UNIQUE) {
        int uniqueCount = 5;
        for (int i = 0; i < arraySize; ++i) data[i] = ((i % uniqueCount) + 1) * (arraySize / uniqueCount);
        std::shuffle(data.begin(), data.end(), rng);
    }
    comparisons = 0; writes = 0; isSorting = false; isFinished = false; isFinishing = false;
    finishIdx = -1; clearVisuals();
    b_i = 0; b_j = 0; while(!q_stack.empty()) q_stack.pop();
    q_stack.push({0, arraySize - 1}); q_partitioning = false;
    m_size = 1; m_l = 0; m_merging = false;
}

void SortSimulator::clearVisuals() { comp1 = comp2 = write1 = write2 = pivotIdx = rangeL = rangeR = -1; }

void SortSimulator::setupMesh() {
    float vertices[] = { 0,0,0, 1,0,0, 1,1,0, 0,1,0 };
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void SortSimulator::step() {
    if (isFinishing) { if (finishIdx < arraySize - 1) finishIdx++; else { isFinishing = false; isFinished = true; } return; }
    if (!isSorting || isFinished) return;
    clearVisuals();
    if (selectedAlgo == BUBBLE_SORT) stepBubble();
    else if (selectedAlgo == QUICK_SORT) stepQuick();
    else if (selectedAlgo == MERGE_SORT) stepMerge();
}

void SortSimulator::stepBubble() {
    if (b_i < arraySize - 1) {
        if (b_j < arraySize - b_i - 1) {
            comp1 = b_j; comp2 = b_j + 1; comparisons++;
            if (data[b_j] > data[b_j + 1]) { std::swap(data[b_j], data[b_j + 1]); write1 = b_j; write2 = b_j + 1; writes++; }
            b_j++;
        } else { b_j = 0; b_i++; }
    } else startFinishAnim();
}

void SortSimulator::stepQuick() {
    if (!q_partitioning) {
        if (q_stack.empty()) { startFinishAnim(); return; }
        auto range = q_stack.top(); q_stack.pop(); q_low = range.first; q_high = range.second;
        if (q_low < q_high) { q_pivot_val = data[q_high]; pivotIdx = q_high; q_i = q_low - 1; q_j = q_low; q_partitioning = true; rangeL = q_low; rangeR = q_high; }
        else stepQuick();
    } else {
        pivotIdx = q_high; rangeL = q_low; rangeR = q_high;
        if (q_j < q_high) {
            comp1 = q_j; comp2 = q_high; comparisons++;
            if (data[q_j] < q_pivot_val) { q_i++; std::swap(data[q_i], data[q_j]); write1 = q_i; write2 = q_j; writes++; }
            q_j++;
        } else { std::swap(data[q_i + 1], data[q_high]); write1 = q_i + 1; write2 = q_high; writes++;
            int p = q_i + 1; q_stack.push({p + 1, q_high}); q_stack.push({q_low, p - 1}); q_partitioning = false; }
    }
}

void SortSimulator::stepMerge() {
    if (!m_merging) {
        if (m_size < arraySize) {
            if (m_l < arraySize - 1) {
                m_mid = std::min(m_l + m_size - 1, arraySize - 1); m_right = std::min(m_l + 2 * m_size - 1, arraySize - 1);
                m_temp.clear(); m_i = m_l; m_j = m_mid + 1; m_k = m_l; m_merging = true; rangeL = m_l; rangeR = m_right;
            } else { m_l = 0; m_size *= 2; stepMerge(); }
        } else startFinishAnim();
    } else {
        rangeL = m_l; rangeR = m_right;
        if (m_i <= m_mid && m_j <= m_right) {
            comp1 = m_i; comp2 = m_j; comparisons++;
            if (data[m_i] <= data[m_j]) m_temp.push_back(data[m_i++]); else m_temp.push_back(data[m_j++]);
        } else if (m_i <= m_mid) m_temp.push_back(data[m_i++]); else if (m_j <= m_right) m_temp.push_back(data[m_j++]);
        else { for (int idx = 0; idx < (int)m_temp.size(); idx++) data[m_l + idx] = m_temp[idx];
            writes += m_temp.size(); m_l += 2 * m_size; m_merging = false; write1 = m_l - 2 * m_size; write2 = m_right; return; }
        write1 = m_l + (int)m_temp.size() - 1;
    }
}

void SortSimulator::startFinishAnim() { isSorting = false; isFinishing = true; finishIdx = 0; clearVisuals(); }

void SortSimulator::render(Shader& shader, int screenWidth, int screenHeight) {
    shader.use();
    glm::mat4 projection = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight, -1.0f, 1.0f);
    shader.setMat4("projection", projection);
    
    glBindVertexArray(vao);
    
    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f;
    float radiusFactor = std::min(screenWidth, screenHeight) * 0.4f;

    for (int i = 0; i < (int)data.size(); ++i) {
        glm::mat4 model = glm::mat4(1.0f);
        
        if (renderMode == BARS) {
            float b_width = (float)screenWidth / data.size();
            float h_scale = (float)screenHeight / data.size();
            model = glm::translate(model, glm::vec3(i * b_width, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(std::max(1.0f, b_width - 0.5f), data[i] * h_scale, 1.0f));
        } else if (renderMode == DOTS) {
            float dotX = ((float)i / data.size()) * screenWidth; 
            float dotY = ((float)data[i] / data.size()) * screenHeight;
            model = glm::translate(model, glm::vec3(dotX, dotY, 0.0f)); 
            model = glm::scale(model, glm::vec3(3.0f, 3.0f, 1.0f));
        } else if (renderMode == CIRCULAR) {
            float angle = ((float)i / data.size()) * 2.0f * (float)M_PI; 
            float radius = ((float)data[i] / data.size()) * radiusFactor;
            model = glm::translate(model, glm::vec3(centerX, centerY, 0.0f)); 
            model = glm::rotate(model, angle, glm::vec3(0, 0, 1)); 
            model = glm::scale(model, glm::vec3(2.0f, radius, 1.0f));
        }
        
        shader.setMat4("model", model);

        int st = 0;
        if (i <= finishIdx) st = 5; 
        else if (i == comp1 || i == comp2) st = 1; 
        else if (i == write1 || i == write2) st = 2; 
        else if (i == pivotIdx) st = 3; 
        else if (i >= rangeL && i <= rangeR) st = 4;
        
        shader.setInt("state", st);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
}

void SortSimulator::setAlgorithm(Algorithm algorithm) {
    if (selectedAlgo != algorithm) {
        selectedAlgo = algorithm;
        reset();
    }
}

Algorithm SortSimulator::getAlgorithm() const { return selectedAlgo; }

void SortSimulator::setRenderMode(RenderMode mode) { renderMode = mode; }

RenderMode SortSimulator::getRenderMode() const { return renderMode; }

void SortSimulator::setDistribution(ArrayDistribution dist) {
    if (distribution != dist) {
        distribution = dist;
        reset();
    }
}

ArrayDistribution SortSimulator::getDistribution() const { return distribution; }

void SortSimulator::setArraySize(int size) {
    if (size <= 0) {
        throw std::invalid_argument("arraySize must be positive");
    }
    if (arraySize != size) {
        arraySize = size;
        reset();
    }
}

int SortSimulator::getArraySize() const { return arraySize; }

void SortSimulator::toggleSorting() {
    if (isFinished) {
        reset();
    }
    isSorting = !isSorting;
}

bool SortSimulator::isSortingActive() const { return isSorting; }

bool SortSimulator::isFinishingAnimation() const { return isFinishing; }

bool SortSimulator::hasFinished() const { return isFinished; }

long long SortSimulator::getComparisons() const { return comparisons; }

long long SortSimulator::getWrites() const { return writes; }
