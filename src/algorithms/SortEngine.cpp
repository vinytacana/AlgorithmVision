#include "SortEngine.h"

#include "DataGenerator.h"

#include <algorithm>
#include <stdexcept>

SortEngine::SortEngine() : bogo_rng(std::random_device{}()) {
    reset();
}

void SortEngine::reset() {
    data = DataGenerator::generate(arraySize, distribution);
    resetAlgorithmState();
}

void SortEngine::resetAlgorithmState() {
    visualState = {};
    b_i = 0;
    b_j = 0;
    while (!q_stack.empty()) q_stack.pop();
    q_stack.push({0, arraySize - 1});
    q_partitioning = false;
    m_size = 1;
    m_l = 0;
    m_merging = false;
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
    kw_writeIndex = 0;
    kw_source.clear();
    kw_output.clear();
    kw_runStart.clear();
    kw_runEnd.clear();
    kw_runCursor.clear();
    sl_initialized = false;
    sl_writeIndex = 0;
    sl_events.clear();
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
    if (!q_partitioning) {
        if (q_stack.empty()) {
            startFinishAnim();
            return;
        }
        auto range = q_stack.top();
        q_stack.pop();
        q_low = range.first;
        q_high = range.second;
        if (q_low < q_high) {
            q_pivot_val = data[q_high];
            visualState.pivotIdx = q_high;
            q_i = q_low - 1;
            q_j = q_low;
            q_partitioning = true;
            visualState.rangeL = q_low;
            visualState.rangeR = q_high;
        } else {
            stepQuick();
        }
    } else {
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
        } else {
            std::swap(data[q_i + 1], data[q_high]);
            visualState.write1 = q_i + 1;
            visualState.write2 = q_high;
            visualState.writes++;
            const int p = q_i + 1;
            q_stack.push({p + 1, q_high});
            q_stack.push({q_low, p - 1});
            q_partitioning = false;
        }
    }
}

void SortEngine::stepMerge() {
    if (!m_merging) {
        if (m_size < arraySize) {
            if (m_l < arraySize - 1) {
                m_mid = std::min(m_l + m_size - 1, arraySize - 1);
                m_right = std::min(m_l + 2 * m_size - 1, arraySize - 1);
                m_temp.clear();
                m_i = m_l;
                m_j = m_mid + 1;
                m_merging = true;
                visualState.rangeL = m_l;
                visualState.rangeR = m_right;
            } else {
                m_l = 0;
                m_size *= 2;
                stepMerge();
            }
        } else {
            startFinishAnim();
        }
    } else {
        visualState.rangeL = m_l;
        visualState.rangeR = m_right;
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
            for (int idx = 0; idx < static_cast<int>(m_temp.size()); idx++) data[m_l + idx] = m_temp[idx];
            visualState.writes += static_cast<long long>(m_temp.size());
            m_l += 2 * m_size;
            m_merging = false;
            visualState.write1 = m_l - 2 * m_size;
            visualState.write2 = m_right;
            return;
        }
        visualState.write1 = m_l + static_cast<int>(m_temp.size()) - 1;
    }
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
        std::sort(data.begin() + start, data.begin() + end);
        kw_runStart.push_back(start);
        kw_runEnd.push_back(end);
        kw_runCursor.push_back(start);
        visualState.writes += end - start;
    }
    kw_source = data;
    visualState.rangeL = 0;
    visualState.rangeR = arraySize - 1;
}

void SortEngine::stepKWayMerge() {
    if (!kw_initialized) {
        initializeKWayMerge();
        return;
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

void SortEngine::stepBogo() {
    if (std::is_sorted(data.begin(), data.end())) {
        startFinishAnim();
        return;
    }

    std::shuffle(data.begin(), data.end(), bogo_rng);
    visualState.comparisons += std::max(0, arraySize - 1);
    visualState.writes += arraySize;
    visualState.rangeL = 0;
    visualState.rangeR = arraySize - 1;
    visualState.write1 = 0;
    visualState.write2 = arraySize - 1;

    if (std::is_sorted(data.begin(), data.end())) startFinishAnim();
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
    if (arraySize != size) {
        arraySize = size;
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
