#ifndef SORT_SIMULATOR_H
#define SORT_SIMULATOR_H

#include <vector>
#include <stack>
#include <utility>
#include "Shader.h"

enum Algorithm { BUBBLE_SORT, QUICK_SORT, MERGE_SORT };
enum RenderMode { BARS, DOTS, CIRCULAR };
enum ArrayDistribution { RANDOM, REVERSED, NEARLY_SORTED, FEW_UNIQUE };

class SortSimulator {
public:
    SortSimulator();
    ~SortSimulator();
    SortSimulator(const SortSimulator&) = delete;
    SortSimulator& operator=(const SortSimulator&) = delete;

    void reset();
    void step();
    void render(Shader& shader, int screenWidth, int screenHeight);

    void setAlgorithm(Algorithm algorithm);
    Algorithm getAlgorithm() const;

    void setRenderMode(RenderMode mode);
    RenderMode getRenderMode() const;

    void setDistribution(ArrayDistribution dist);
    ArrayDistribution getDistribution() const;

    void setArraySize(int size);
    int getArraySize() const;

    void toggleSorting();
    bool isSortingActive() const;
    bool isFinishingAnimation() const;
    bool hasFinished() const;

    long long getComparisons() const;
    long long getWrites() const;

private:
    std::vector<int> data;
    unsigned int vao = 0;
    unsigned int vbo = 0;
    int arraySize = 100;

    int comp1 = -1;
    int comp2 = -1;
    int write1 = -1;
    int write2 = -1;
    int pivotIdx = -1;
    int rangeL = -1;
    int rangeR = -1;
    int finishIdx = -1;
    long long comparisons = 0;
    long long writes = 0;
    bool isSorting = false;
    bool isFinished = false;
    bool isFinishing = false;

    Algorithm selectedAlgo = BUBBLE_SORT;
    RenderMode renderMode = BARS;
    ArrayDistribution distribution = RANDOM;

    // Estados internos Bubble, Quick, Merge
    int b_i = 0, b_j = 0;
    std::stack<std::pair<int, int>> q_stack;
    int q_low = 0, q_high = 0, q_pivot_val = 0, q_i = 0, q_j = 0;
    bool q_partitioning = false;
    int m_size = 1, m_l = 0, m_i = 0, m_j = 0, m_k = 0, m_mid = 0, m_right = 0;
    std::vector<int> m_temp;
    bool m_merging = false;

    void clearVisuals();
    void setupMesh();
    void stepBubble();
    void stepQuick();
    void stepMerge();
    void startFinishAnim();
};

#endif
