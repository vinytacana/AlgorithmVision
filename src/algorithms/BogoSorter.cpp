#include "BogoSorter.h"

#include <algorithm>
#include <numeric>

BogoSorter::BogoSorter(std::vector<int>& dataRef, int& arraySizeRef, SortVisualState& visualStateRef)
    : ISortingStrategy(dataRef, arraySizeRef, visualStateRef),
      rng(std::random_device{}()) {}

void BogoSorter::reset() {
    shuffling = false;
    shuffleIndex = -1;
    swapTargets.clear();
}

void BogoSorter::prepareShuffle() {
    swapTargets.resize(arraySize);
    std::iota(swapTargets.begin(), swapTargets.end(), 0);
    for (int i = arraySize - 1; i > 0; --i) {
        std::uniform_int_distribution<int> dist(0, i);
        swapTargets[i] = dist(rng);
    }
    shuffleIndex = arraySize - 1;
    shuffling = true;
}

void BogoSorter::step() {
    if (std::is_sorted(data.begin(), data.end())) {
        startFinishAnim();
        return;
    }

    visualState.rangeL = 0;
    visualState.rangeR = arraySize - 1;
    if (!shuffling) {
        prepareShuffle();
    }

    if (shuffleIndex > 0) {
        const int swapTarget = swapTargets[shuffleIndex];
        std::swap(data[shuffleIndex], data[swapTarget]);
        visualState.write1 = shuffleIndex;
        visualState.write2 = swapTarget;
        visualState.pivotIdx = shuffleIndex;
        visualState.writes++;
        shuffleIndex--;
        return;
    }

    shuffling = false;
    shuffleIndex = -1;
    visualState.comparisons += std::max(0, arraySize - 1);
    if (std::is_sorted(data.begin(), data.end())) startFinishAnim();
}
