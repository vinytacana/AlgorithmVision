#ifndef SORT_ENGINE_H
#define SORT_ENGINE_H

#include "AlgorithmRegistry.h"
#include "SortTypes.h"

#include <random>
#include <stack>
#include <utility>
#include <vector>

class SortEngine {
public:
    SortEngine();

    void reset();
    void step();

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
    const SortVisualState& getVisualState() const;

private:
    std::vector<int> data;
    int arraySize = 100;
    SortVisualState visualState;

    Algorithm selectedAlgo = BUBBLE_SORT;
    RenderMode renderMode = BARS;
    ArrayDistribution distribution = RANDOM;

    int b_i = 0;
    int b_j = 0;
    std::stack<std::pair<int, int>> q_stack;
    int q_low = 0;
    int q_high = 0;
    int q_pivot_val = 0;
    int q_i = 0;
    int q_j = 0;
    bool q_partitioning = false;
    int m_size = 1;
    int m_l = 0;
    int m_i = 0;
    int m_j = 0;
    int m_mid = 0;
    int m_right = 0;
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
    int hp_phase = 0;
    int hp_heapSize = 0;
    int hp_buildIndex = 0;
    int hp_extractIndex = 0;
    int hp_siftRoot = 0;
    bool hp_sifting = false;
    static constexpr int KW_RUNS = 4;
    bool kw_initialized = false;
    int kw_writeIndex = 0;
    std::vector<int> kw_source;
    std::vector<int> kw_output;
    std::vector<int> kw_runStart;
    std::vector<int> kw_runEnd;
    std::vector<int> kw_runCursor;
    bool sl_initialized = false;
    int sl_writeIndex = 0;
    std::vector<std::pair<int, int>> sl_events;
    std::mt19937 bogo_rng;

    void clearVisuals();
    void resetAlgorithmState();
    void stepBubble();
    void stepQuick();
    void stepMerge();
    void stepInsertion();
    void stepSelection();
    void stepShell();
    bool heapSiftDownStep(int heapSize, int& root);
    void stepHeap();
    void initializeKWayMerge();
    void stepKWayMerge();
    void stepMiracle();
    void initializeSleep();
    void stepSleep();
    void stepBogo();
    void startFinishAnim();
};

#endif
