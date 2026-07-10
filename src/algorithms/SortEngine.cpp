#include "SortEngine.h"

#include "BogoSorter.h"
#include "BubbleSorter.h"
#include "DataGenerator.h"
#include "HeapSorter.h"
#include "InsertionSorter.h"
#include "KWayMergeSorter.h"
#include "MergeSorter.h"
#include "MiracleSorter.h"
#include "QuickSorter.h"
#include "SelectionSorter.h"
#include "ShellSorter.h"
#include "SleepSorter.h"
#include "StalinSorter.h"
#include "ThanosSorter.h"

#include <cstddef>
#include <memory>
#include <stdexcept>

SortEngine::SortEngine() {
    reset();
}

void SortEngine::reset() {
    arraySize = configuredArraySize;
    data = DataGenerator::generate(arraySize, distribution);
    resetAlgorithmState();
}

std::unique_ptr<ISortingStrategy> SortEngine::createStrategy(Algorithm algorithm) {
    switch (algorithm) {
        case BUBBLE_SORT:
            return std::make_unique<BubbleSorter>(data, arraySize, visualState);
        case QUICK_SORT:
            return std::make_unique<QuickSorter>(data, arraySize, visualState);
        case MERGE_SORT:
            return std::make_unique<MergeSorter>(data, arraySize, visualState);
        case INSERTION_SORT:
            return std::make_unique<InsertionSorter>(data, arraySize, visualState);
        case SELECTION_SORT:
            return std::make_unique<SelectionSorter>(data, arraySize, visualState);
        case SHELL_SORT:
            return std::make_unique<ShellSorter>(data, arraySize, visualState);
        case HEAP_SORT:
            return std::make_unique<HeapSorter>(data, arraySize, visualState);
        case K_WAY_MERGE_SORT:
            return std::make_unique<KWayMergeSorter>(data, arraySize, visualState);
        case MIRACLE_SORT:
            return std::make_unique<MiracleSorter>(data, arraySize, visualState);
        case SLEEP_SORT:
            return std::make_unique<SleepSorter>(data, arraySize, visualState);
        case BOGO_SORT:
            return std::make_unique<BogoSorter>(data, arraySize, visualState);
        case STALIN_SORT:
            return std::make_unique<StalinSorter>(data, arraySize, visualState);
        case THANOS_SORT:
            return std::make_unique<ThanosSorter>(data, arraySize, visualState);
        case ALGORITHM_COUNT:
            break;
    }
    return std::make_unique<BubbleSorter>(data, arraySize, visualState);
}

void SortEngine::resetAlgorithmState() {
    visualState = {};
    currentStrategy = createStrategy(selectedAlgo);
    currentStrategy->reset();
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

    if (!visualState.isSorting || visualState.isFinished || !currentStrategy) return;

    clearVisuals();
    currentStrategy->step();
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
    if (size > MAX_ARRAY_SIZE) throw std::invalid_argument("arraySize exceeds maximum allowed size");
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
    if (values.size() > static_cast<std::size_t>(MAX_ARRAY_SIZE)) {
        throw std::invalid_argument("data exceeds maximum allowed size");
    }
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
