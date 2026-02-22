#ifndef SORT_SIMULATOR_H
#define SORT_SIMULATOR_H

#include <vector>
#include <stack>
#include "Shader.h"

enum Algorithm { BUBBLE_SORT, QUICK_SORT, MERGE_SORT };
enum RenderMode { BARS, DOTS, CIRCULAR };
enum ArrayDistribution { RANDOM, REVERSED, NEARLY_SORTED, FEW_UNIQUE };

class SortSimulator {
public:
    std::vector<int> data;
    unsigned int VAO, VBO;
    int arraySize = 100;
    
    int comp1 = -1, comp2 = -1, write1 = -1, write2 = -1, pivotIdx = -1, rangeL = -1, rangeR = -1, finishIdx = -1;
    long long comparisons = 0, writes = 0;
    bool isSorting = false, isFinished = false, isFinishing = false;
    
    Algorithm selectedAlgo = BUBBLE_SORT;
    RenderMode renderMode = BARS;
    ArrayDistribution distribution = RANDOM;

    SortSimulator();
    void reset();
    void step();
    void render(Shader& shader, int screenWidth, int screenHeight);

private:
    // Estados internos Bubble, Quick, Merge
    int b_i = 0, b_j = 0;
    std::stack<std::pair<int, int>> q_stack;
    int q_low, q_high, q_pivot_val, q_i, q_j;
    bool q_partitioning = false;
    int m_size = 1, m_l = 0, m_i, m_j, m_k, m_mid, m_right;
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
