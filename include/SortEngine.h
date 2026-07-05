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
    enum class QuickPhase {
        SELECT_RANGE,
        PARTITION
    };

    enum class MergePhase {
        SELECT_RUN,
        MERGE_VALUES,
        COPY_BACK
    };

    std::vector<int> data;
    int arraySize = 100;           // tamanho "vivo" (pode encolher em Stalin/Thanos)
    int configuredArraySize = 100; // tamanho escolhido pelo usuario; usado no reset()
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
    QuickPhase q_phase = QuickPhase::SELECT_RANGE;
    int m_size = 1;
    int m_l = 0;
    int m_i = 0;
    int m_j = 0;
    int m_mid = 0;
    int m_right = 0;
    std::vector<int> m_temp;
    MergePhase m_phase = MergePhase::SELECT_RUN;
    int m_copyIdx = 0;
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
    bool kw_sortingRuns = false;
    bool kw_mergeSourceReady = false;
    int kw_sortRunIndex = 0;
    int kw_sortI = 0;
    int kw_sortJ = 0;
    int kw_sortKey = 0;
    bool kw_sortInserting = false;
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
    bool bg_shuffling = false;
    int bg_shuffleIndex = -1;
    std::vector<int> bg_swapTargets;
    int st_scanIndex = 1;

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
    void stepKWayRunSort();
    void stepKWayMerge();
    void stepMiracle();
    void initializeSleep();
    void stepSleep();
    void prepareBogoShuffle();
    void stepBogo();
    void stepStalin();
    void stepThanos();
    void startFinishAnim();
};

#endif
