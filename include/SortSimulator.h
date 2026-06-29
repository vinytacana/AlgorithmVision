#ifndef SORT_SIMULATOR_H
#define SORT_SIMULATOR_H

#include <vector>
#include <stack>
#include <utility>
#include "Shader.h"

enum Algorithm { BUBBLE_SORT, QUICK_SORT, MERGE_SORT, INSERTION_SORT, SELECTION_SORT, SHELL_SORT };
enum RenderMode { BARS, DOTS, CIRCULAR };
enum ArrayDistribution { RANDOM, REVERSED, NEARLY_SORTED, FEW_UNIQUE };

class SortSimulator {
public:
    explicit SortSimulator(bool initializeGraphics = true);
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
    void setData(const std::vector<int>& values);
    const std::vector<int>& getData() const;

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

    // Estados internos Bubble, Quick, Merge, Insertion, Selection, Shell
    int b_i = 0, b_j = 0;
    std::stack<std::pair<int, int>> q_stack;
    int q_low = 0, q_high = 0, q_pivot_val = 0, q_i = 0, q_j = 0;
    bool q_partitioning = false;
    int m_size = 1, m_l = 0, m_i = 0, m_j = 0, m_k = 0, m_mid = 0, m_right = 0;
    std::vector<int> m_temp;
    bool m_merging = false;
    int ins_i = 1;
    int ins_j = 0;
    int ins_key = 0;
    bool ins_inserting = false;
    int sel_i = 0;
    int sel_j = 1;
    int sel_min = 0;
    int sh_gap = 0;
    int sh_i = 0;
    int sh_j = 0;
    int sh_temp = 0;
    bool sh_inserting = false;

    void clearVisuals();
    void resetAlgorithmState();
    void setupMesh();
    void stepBubble();
    void stepQuick();
    void stepMerge();
    void stepInsertion();
    void stepSelection();
    void stepShell();
    void startFinishAnim();
};

#endif
