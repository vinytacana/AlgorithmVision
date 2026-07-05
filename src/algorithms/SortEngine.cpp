#include "SortEngine.h"

#include "DataGenerator.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>

SortEngine::SortEngine() : bogo_rng(std::random_device{}()) {
    reset();
}

void SortEngine::reset() {
    arraySize = configuredArraySize;
    data = DataGenerator::generate(arraySize, distribution);
    resetAlgorithmState();
}

void SortEngine::resetAlgorithmState() {
    visualState = {};
    b_i = 0;
    b_j = 0;
    while (!q_stack.empty()) q_stack.pop();
    q_stack.push({0, arraySize - 1});
    q_phase = QuickPhase::SELECT_RANGE;
    m_size = 1;
    m_l = 0;
    m_phase = MergePhase::SELECT_RUN;
    m_copyIdx = 0;
    ins_i = 1;
    ins_j = 0;
    ins_key = 0;
    ins_inserting = false;
    sel_i = 0;
    sel_j = 1;
    sel_min = 0;
    sh_gap = std::max(1, arraySize / 2);
    sh_i = sh_gap;
    sh_j = sh_gap;
    sh_temp = 0;
    sh_inserting = false;
    hp_phase = 0;
    hp_heapSize = arraySize;
    hp_buildIndex = arraySize / 2 - 1;
    hp_extractIndex = arraySize - 1;
    hp_siftRoot = 0;
    hp_sifting = false;
    kw_initialized = false;
    kw_sortingRuns = false;
    kw_mergeSourceReady = false;
    kw_sortRunIndex = 0;
    kw_sortI = 0;
    kw_sortJ = 0;
    kw_sortKey = 0;
    kw_sortInserting = false;
    kw_writeIndex = 0;
    kw_source.clear();
    kw_output.clear();
    kw_runStart.clear();
    kw_runEnd.clear();
    kw_runCursor.clear();
    sl_initialized = false;
    sl_writeIndex = 0;
    sl_events.clear();
    bg_shuffling = false;
    bg_shuffleIndex = -1;
    bg_swapTargets.clear();
    st_scanIndex = 1;
}

void SortEngine::clearVisuals() {
    visualState.comp1 = -1;
    visualState.comp2 = -1;
    visualState.write1 = -1;
    visualState.write2 = -1;
    visualState.pivotIdx = -1;
    visualState.rangeL = -1;
    visualState.rangeR = -1;
}

void SortEngine::step() {
    if (visualState.isFinishing) {
        if (visualState.finishIdx < arraySize - 1) visualState.finishIdx++;
        else {
            visualState.isFinishing = false;
            visualState.isFinished = true;
        }
        return;
    }

    if (!visualState.isSorting || visualState.isFinished) return;

    clearVisuals();
    if (selectedAlgo == BUBBLE_SORT) stepBubble();
    else if (selectedAlgo == QUICK_SORT) stepQuick();
    else if (selectedAlgo == MERGE_SORT) stepMerge();
    else if (selectedAlgo == INSERTION_SORT) stepInsertion();
    else if (selectedAlgo == SELECTION_SORT) stepSelection();
    else if (selectedAlgo == SHELL_SORT) stepShell();
    else if (selectedAlgo == HEAP_SORT) stepHeap();
    else if (selectedAlgo == K_WAY_MERGE_SORT) stepKWayMerge();
    else if (selectedAlgo == MIRACLE_SORT) stepMiracle();
    else if (selectedAlgo == SLEEP_SORT) stepSleep();
    else if (selectedAlgo == BOGO_SORT) stepBogo();
    else if (selectedAlgo == STALIN_SORT) stepStalin();
    else if (selectedAlgo == THANOS_SORT) stepThanos();
}

void SortEngine::stepBubble() {
    if (b_i < arraySize - 1) {
        if (b_j < arraySize - b_i - 1) {
            visualState.comp1 = b_j;
            visualState.comp2 = b_j + 1;
            visualState.comparisons++;
            if (data[b_j] > data[b_j + 1]) {
                std::swap(data[b_j], data[b_j + 1]);
                visualState.write1 = b_j;
                visualState.write2 = b_j + 1;
                visualState.writes++;
            }
            b_j++;
        } else {
            b_j = 0;
            b_i++;
        }
    } else {
        startFinishAnim();
    }
}

void SortEngine::stepQuick() {
    if (q_phase == QuickPhase::SELECT_RANGE) {
        if (q_stack.empty()) {
            startFinishAnim();
            return;
        }
        const auto range = q_stack.top();
        q_stack.pop();
        q_low = range.first;
        q_high = range.second;
        if (q_low >= q_high) {
            return;
        }

        q_pivot_val = data[q_high];
        visualState.pivotIdx = q_high;
        q_i = q_low - 1;
        q_j = q_low;
        q_phase = QuickPhase::PARTITION;
        visualState.rangeL = q_low;
        visualState.rangeR = q_high;
        return;
    }

    visualState.pivotIdx = q_high;
    visualState.rangeL = q_low;
    visualState.rangeR = q_high;
    if (q_j < q_high) {
        visualState.comp1 = q_j;
        visualState.comp2 = q_high;
        visualState.comparisons++;
        if (data[q_j] < q_pivot_val) {
            q_i++;
            std::swap(data[q_i], data[q_j]);
            visualState.write1 = q_i;
            visualState.write2 = q_j;
            visualState.writes++;
        }
        q_j++;
        return;
    }

    std::swap(data[q_i + 1], data[q_high]);
    visualState.write1 = q_i + 1;
    visualState.write2 = q_high;
    visualState.writes++;
    const int p = q_i + 1;
    q_stack.push({p + 1, q_high});
    q_stack.push({q_low, p - 1});
    q_phase = QuickPhase::SELECT_RANGE;
}

void SortEngine::stepMerge() {
    if (m_phase == MergePhase::SELECT_RUN) {
        if (m_size >= arraySize) {
            startFinishAnim();
            return;
        }

        if (m_l >= arraySize - 1) {
            m_l = 0;
            m_size *= 2;
            return;
        }

        m_mid = std::min(m_l + m_size - 1, arraySize - 1);
        m_right = std::min(m_l + 2 * m_size - 1, arraySize - 1);
        m_temp.clear();
        m_i = m_l;
        m_j = m_mid + 1;
        m_copyIdx = 0;
        m_phase = MergePhase::MERGE_VALUES;
        visualState.rangeL = m_l;
        visualState.rangeR = m_right;
        return;
    }

    visualState.rangeL = m_l;
    visualState.rangeR = m_right;
    if (m_phase == MergePhase::COPY_BACK) {
        data[m_l + m_copyIdx] = m_temp[m_copyIdx];
        visualState.write1 = m_l + m_copyIdx;
        visualState.write2 = -1;
        visualState.pivotIdx = m_l + m_copyIdx;
        visualState.writes++;
        m_copyIdx++;
        if (m_copyIdx >= static_cast<int>(m_temp.size())) {
            m_l += 2 * m_size;
            m_phase = MergePhase::SELECT_RUN;
            m_copyIdx = 0;
        }
        return;
    }

    if (m_i <= m_mid && m_j <= m_right) {
        visualState.comp1 = m_i;
        visualState.comp2 = m_j;
        visualState.comparisons++;
        if (data[m_i] <= data[m_j]) m_temp.push_back(data[m_i++]);
        else m_temp.push_back(data[m_j++]);
    } else if (m_i <= m_mid) {
        m_temp.push_back(data[m_i++]);
    } else if (m_j <= m_right) {
        m_temp.push_back(data[m_j++]);
    } else {
        m_phase = MergePhase::COPY_BACK;
        m_copyIdx = 0;
        return;
    }
    visualState.write1 = m_l + static_cast<int>(m_temp.size()) - 1;
    visualState.write2 = -1;
}

void SortEngine::stepInsertion() {
    if (ins_i >= arraySize) {
        startFinishAnim();
        return;
    }

    if (!ins_inserting) {
        ins_key = data[ins_i];
        ins_j = ins_i - 1;
        visualState.pivotIdx = ins_i;
        ins_inserting = true;
    }

    visualState.pivotIdx = ins_i;
    if (ins_j >= 0) {
        visualState.comp1 = ins_j;
        visualState.comp2 = ins_j + 1;
        visualState.comparisons++;
        if (data[ins_j] > ins_key) {
            data[ins_j + 1] = data[ins_j];
            visualState.write1 = ins_j;
            visualState.write2 = ins_j + 1;
            visualState.writes++;
            ins_j--;
            return;
        }
    }

    data[ins_j + 1] = ins_key;
    visualState.write1 = ins_j + 1;
    visualState.writes++;
    ins_i++;
    ins_inserting = false;
}

void SortEngine::stepSelection() {
    if (sel_i >= arraySize - 1) {
        startFinishAnim();
        return;
    }

    visualState.rangeL = sel_i;
    visualState.rangeR = arraySize - 1;
    visualState.pivotIdx = sel_min;

    if (sel_j < arraySize) {
        visualState.comp1 = sel_min;
        visualState.comp2 = sel_j;
        visualState.comparisons++;
        if (data[sel_j] < data[sel_min]) {
            sel_min = sel_j;
            visualState.pivotIdx = sel_min;
        }
        sel_j++;
        return;
    }

    if (sel_min != sel_i) {
        std::swap(data[sel_i], data[sel_min]);
        visualState.write1 = sel_i;
        visualState.write2 = sel_min;
        visualState.writes++;
    }
    sel_i++;
    sel_min = sel_i;
    sel_j = sel_i + 1;
}

void SortEngine::stepShell() {
    if (sh_gap <= 0) {
        startFinishAnim();
        return;
    }

    if (sh_i >= arraySize) {
        sh_gap /= 2;
        if (sh_gap <= 0) {
            startFinishAnim();
            return;
        }
        sh_i = sh_gap;
        sh_inserting = false;
    }

    if (!sh_inserting) {
        sh_temp = data[sh_i];
        sh_j = sh_i;
        sh_inserting = true;
    }

    visualState.rangeL = 0;
    visualState.rangeR = arraySize - 1;
    visualState.pivotIdx = sh_i;

    if (sh_j >= sh_gap) {
        visualState.comp1 = sh_j - sh_gap;
        visualState.comp2 = sh_j;
        visualState.comparisons++;
        if (data[sh_j - sh_gap] > sh_temp) {
            data[sh_j] = data[sh_j - sh_gap];
            visualState.write1 = sh_j - sh_gap;
            visualState.write2 = sh_j;
            visualState.writes++;
            sh_j -= sh_gap;
            return;
        }
    }

    data[sh_j] = sh_temp;
    visualState.write1 = sh_j;
    visualState.writes++;
    sh_i++;
    sh_inserting = false;
}

bool SortEngine::heapSiftDownStep(int heapSize, int& root) {
    const int left = 2 * root + 1;
    const int right = left + 1;
    if (left >= heapSize) return true;

    int largest = root;
    visualState.comp1 = root;
    visualState.comp2 = left;
    visualState.comparisons++;
    if (data[left] > data[largest]) largest = left;

    if (right < heapSize) {
        visualState.comp1 = largest;
        visualState.comp2 = right;
        visualState.comparisons++;
        if (data[right] > data[largest]) largest = right;
    }

    visualState.rangeL = 0;
    visualState.rangeR = heapSize - 1;
    visualState.pivotIdx = root;

    if (largest == root) return true;

    std::swap(data[root], data[largest]);
    visualState.write1 = root;
    visualState.write2 = largest;
    visualState.writes++;
    root = largest;
    return false;
}

void SortEngine::stepHeap() {
    if (arraySize <= 1) {
        startFinishAnim();
        return;
    }

    if (hp_phase == 0) {
        if (hp_buildIndex < 0) {
            hp_phase = 1;
            hp_sifting = false;
            return;
        }
        if (!hp_sifting) {
            hp_siftRoot = hp_buildIndex;
            hp_heapSize = arraySize;
            hp_sifting = true;
        }
        if (heapSiftDownStep(hp_heapSize, hp_siftRoot)) {
            hp_sifting = false;
            hp_buildIndex--;
        }
        return;
    }

    if (hp_extractIndex <= 0) {
        startFinishAnim();
        return;
    }

    if (!hp_sifting) {
        std::swap(data[0], data[hp_extractIndex]);
        visualState.write1 = 0;
        visualState.write2 = hp_extractIndex;
        visualState.pivotIdx = hp_extractIndex;
        visualState.rangeL = 0;
        visualState.rangeR = hp_extractIndex;
        visualState.writes++;
        hp_heapSize = hp_extractIndex;
        hp_extractIndex--;
        hp_siftRoot = 0;
        hp_sifting = true;
        return;
    }

    if (heapSiftDownStep(hp_heapSize, hp_siftRoot)) {
        hp_sifting = false;
    }
}

void SortEngine::initializeKWayMerge() {
    kw_initialized = true;
    kw_sortingRuns = true;
    kw_mergeSourceReady = false;
    kw_sortRunIndex = 0;
    kw_sortInserting = false;
    kw_writeIndex = 0;
    kw_output.clear();
    kw_output.reserve(data.size());
    kw_runStart.clear();
    kw_runEnd.clear();
    kw_runCursor.clear();

    const int runs = std::min(KW_RUNS, arraySize);
    for (int run = 0; run < runs; ++run) {
        const int start = run * arraySize / runs;
        const int end = (run + 1) * arraySize / runs;
        kw_runStart.push_back(start);
        kw_runEnd.push_back(end);
        kw_runCursor.push_back(start);
    }
}

void SortEngine::stepKWayRunSort() {
    if (kw_sortRunIndex >= static_cast<int>(kw_runStart.size())) {
        kw_sortingRuns = false;
        kw_mergeSourceReady = false;
        return;
    }

    const int runStart = kw_runStart[kw_sortRunIndex];
    const int runEnd = kw_runEnd[kw_sortRunIndex];
    visualState.rangeL = runStart;
    visualState.rangeR = runEnd - 1;

    if (runEnd - runStart <= 1) {
        kw_sortRunIndex++;
        kw_sortInserting = false;
        return;
    }

    if (!kw_sortInserting) {
        if (kw_sortI < runStart + 1) kw_sortI = runStart + 1;
        if (kw_sortI >= runEnd) {
            kw_sortRunIndex++;
            kw_sortI = 0;
            kw_sortJ = 0;
            kw_sortInserting = false;
            return;
        }

        kw_sortKey = data[kw_sortI];
        kw_sortJ = kw_sortI - 1;
        kw_sortInserting = true;
        visualState.pivotIdx = kw_sortI;
    }

    visualState.pivotIdx = kw_sortI;
    if (kw_sortJ >= runStart) {
        visualState.comp1 = kw_sortJ;
        visualState.comp2 = kw_sortJ + 1;
        visualState.comparisons++;
        if (data[kw_sortJ] > kw_sortKey) {
            data[kw_sortJ + 1] = data[kw_sortJ];
            visualState.write1 = kw_sortJ;
            visualState.write2 = kw_sortJ + 1;
            visualState.writes++;
            kw_sortJ--;
            return;
        }
    }

    data[kw_sortJ + 1] = kw_sortKey;
    visualState.write1 = kw_sortJ + 1;
    visualState.write2 = -1;
    visualState.writes++;
    kw_sortI++;
    kw_sortInserting = false;
}

void SortEngine::stepKWayMerge() {
    if (!kw_initialized) {
        initializeKWayMerge();
        return;
    }
    if (kw_sortingRuns) {
        stepKWayRunSort();
        return;
    }
    if (!kw_mergeSourceReady) {
        kw_source = data;
        for (int run = 0; run < static_cast<int>(kw_runCursor.size()); ++run) {
            kw_runCursor[run] = kw_runStart[run];
        }
        kw_mergeSourceReady = true;
    }
    if (kw_writeIndex >= arraySize) {
        startFinishAnim();
        return;
    }

    int bestRun = -1;
    for (int run = 0; run < static_cast<int>(kw_runCursor.size()); ++run) {
        if (kw_runCursor[run] >= kw_runEnd[run]) continue;
        if (bestRun == -1) {
            bestRun = run;
            continue;
        }
        visualState.comp1 = kw_runCursor[bestRun];
        visualState.comp2 = kw_runCursor[run];
        visualState.comparisons++;
        if (kw_source[kw_runCursor[run]] < kw_source[kw_runCursor[bestRun]]) {
            bestRun = run;
        }
    }

    if (bestRun == -1) {
        startFinishAnim();
        return;
    }

    kw_output.push_back(kw_source[kw_runCursor[bestRun]]);
    kw_runCursor[bestRun]++;
    data[kw_writeIndex] = kw_output.back();
    visualState.write1 = kw_writeIndex;
    visualState.pivotIdx = kw_writeIndex;
    visualState.rangeL = kw_runStart[bestRun];
    visualState.rangeR = kw_runEnd[bestRun] - 1;
    visualState.writes++;
    kw_writeIndex++;
}

void SortEngine::stepMiracle() {
    if (std::is_sorted(data.begin(), data.end())) {
        startFinishAnim();
        return;
    }

    visualState.rangeL = 0;
    visualState.rangeR = arraySize - 1;
    visualState.pivotIdx = arraySize > 0 ? static_cast<int>(visualState.comparisons % arraySize) : -1;
    visualState.comparisons++;
}

void SortEngine::initializeSleep() {
    sl_initialized = true;
    sl_writeIndex = 0;
    sl_events.clear();
    sl_events.reserve(data.size());
    for (int i = 0; i < arraySize; ++i) sl_events.push_back({data[i], i});
    std::stable_sort(sl_events.begin(), sl_events.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.first != rhs.first) return lhs.first < rhs.first;
        return lhs.second < rhs.second;
    });
}

void SortEngine::stepSleep() {
    if (!sl_initialized) {
        initializeSleep();
        return;
    }
    if (sl_writeIndex >= arraySize) {
        startFinishAnim();
        return;
    }

    visualState.comp1 = sl_events[sl_writeIndex].second;
    visualState.write1 = sl_writeIndex;
    visualState.pivotIdx = sl_writeIndex;
    data[sl_writeIndex] = sl_events[sl_writeIndex].first;
    visualState.writes++;
    sl_writeIndex++;
}

void SortEngine::prepareBogoShuffle() {
    bg_swapTargets.resize(arraySize);
    std::iota(bg_swapTargets.begin(), bg_swapTargets.end(), 0);
    for (int i = arraySize - 1; i > 0; --i) {
        std::uniform_int_distribution<int> dist(0, i);
        bg_swapTargets[i] = dist(bogo_rng);
    }
    bg_shuffleIndex = arraySize - 1;
    bg_shuffling = true;
}

void SortEngine::stepBogo() {
    if (std::is_sorted(data.begin(), data.end())) {
        startFinishAnim();
        return;
    }

    visualState.rangeL = 0;
    visualState.rangeR = arraySize - 1;
    if (!bg_shuffling) {
        prepareBogoShuffle();
    }

    if (bg_shuffleIndex > 0) {
        const int swapTarget = bg_swapTargets[bg_shuffleIndex];
        std::swap(data[bg_shuffleIndex], data[swapTarget]);
        visualState.write1 = bg_shuffleIndex;
        visualState.write2 = swapTarget;
        visualState.pivotIdx = bg_shuffleIndex;
        visualState.writes++;
        bg_shuffleIndex--;
        return;
    }

    bg_shuffling = false;
    bg_shuffleIndex = -1;
    visualState.comparisons += std::max(0, arraySize - 1);
    if (std::is_sorted(data.begin(), data.end())) startFinishAnim();
}

void SortEngine::stepStalin() {
    if (arraySize <= 1 || st_scanIndex >= arraySize) {
        startFinishAnim();
        return;
    }

    visualState.rangeL = 0;
    visualState.rangeR = arraySize - 1;
    visualState.comp1 = st_scanIndex - 1;
    visualState.comp2 = st_scanIndex;
    visualState.pivotIdx = st_scanIndex;
    visualState.comparisons++;

    if (data[st_scanIndex] < data[st_scanIndex - 1]) {
        data.erase(data.begin() + st_scanIndex);
        arraySize = static_cast<int>(data.size());
        visualState.write1 = st_scanIndex;
        visualState.write2 = -1;
        visualState.writes++;
        if (st_scanIndex >= arraySize) {
            startFinishAnim();
        }
        return;
    }

    st_scanIndex++;
    if (st_scanIndex >= arraySize) {
        startFinishAnim();
    }
}

void SortEngine::stepThanos() {
    if (arraySize <= 1) {
        startFinishAnim();
        return;
    }

    const int newSize = arraySize / 2;
    visualState.rangeL = newSize;
    visualState.rangeR = arraySize - 1;
    visualState.write1 = newSize;
    visualState.write2 = arraySize - 1;
    visualState.pivotIdx = newSize;
    visualState.writes += arraySize - newSize;

    data.erase(data.begin() + newSize, data.end());
    arraySize = static_cast<int>(data.size());
    startFinishAnim();
}

void SortEngine::startFinishAnim() {
    visualState.isSorting = false;
    visualState.isFinishing = true;
    visualState.finishIdx = 0;
    clearVisuals();
}

void SortEngine::setAlgorithm(Algorithm algorithm) {
    if (selectedAlgo != algorithm) {
        selectedAlgo = algorithm;
        reset();
    }
}

Algorithm SortEngine::getAlgorithm() const {
    return selectedAlgo;
}

void SortEngine::setRenderMode(RenderMode mode) {
    renderMode = mode;
}

RenderMode SortEngine::getRenderMode() const {
    return renderMode;
}

void SortEngine::setDistribution(ArrayDistribution dist) {
    if (distribution != dist) {
        distribution = dist;
        reset();
    }
}

ArrayDistribution SortEngine::getDistribution() const {
    return distribution;
}

void SortEngine::setArraySize(int size) {
    if (size <= 0) throw std::invalid_argument("arraySize must be positive");
    if (configuredArraySize != size) {
        configuredArraySize = size;
        reset();
    }
}

int SortEngine::getArraySize() const {
    return arraySize;
}

void SortEngine::setData(const std::vector<int>& values) {
    if (values.empty()) throw std::invalid_argument("data must not be empty");
    data = values;
    arraySize = static_cast<int>(data.size());
    configuredArraySize = arraySize;
    resetAlgorithmState();
}

const std::vector<int>& SortEngine::getData() const {
    return data;
}

void SortEngine::toggleSorting() {
    if (visualState.isFinished) reset();
    visualState.isSorting = !visualState.isSorting;
}

bool SortEngine::isSortingActive() const {
    return visualState.isSorting;
}

bool SortEngine::isFinishingAnimation() const {
    return visualState.isFinishing;
}

bool SortEngine::hasFinished() const {
    return visualState.isFinished;
}

long long SortEngine::getComparisons() const {
    return visualState.comparisons;
}

long long SortEngine::getWrites() const {
    return visualState.writes;
}

const SortVisualState& SortEngine::getVisualState() const {
    return visualState;
}
