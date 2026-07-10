#include "HeapSorter.h"

#include <algorithm>

void HeapSorter::reset() {
    phase = 0;
    heapSize = arraySize;
    buildIndex = arraySize / 2 - 1;
    extractIndex = arraySize - 1;
    siftRoot = 0;
    sifting = false;
}

bool HeapSorter::siftDownStep(int currentHeapSize, int& root) {
    const int left = 2 * root + 1;
    const int right = left + 1;
    if (left >= currentHeapSize) return true;

    int largest = root;
    visualState.comp1 = root;
    visualState.comp2 = left;
    visualState.comparisons++;
    if (data[left] > data[largest]) largest = left;

    if (right < currentHeapSize) {
        visualState.comp1 = largest;
        visualState.comp2 = right;
        visualState.comparisons++;
        if (data[right] > data[largest]) largest = right;
    }

    visualState.rangeL = 0;
    visualState.rangeR = currentHeapSize - 1;
    visualState.pivotIdx = root;

    if (largest == root) return true;

    std::swap(data[root], data[largest]);
    visualState.write1 = root;
    visualState.write2 = largest;
    visualState.writes++;
    root = largest;
    return false;
}

void HeapSorter::step() {
    if (arraySize <= 1) {
        startFinishAnim();
        return;
    }

    if (phase == 0) {
        if (buildIndex < 0) {
            phase = 1;
            sifting = false;
            return;
        }
        if (!sifting) {
            siftRoot = buildIndex;
            heapSize = arraySize;
            sifting = true;
        }
        if (siftDownStep(heapSize, siftRoot)) {
            sifting = false;
            buildIndex--;
        }
        return;
    }

    if (extractIndex <= 0) {
        startFinishAnim();
        return;
    }

    if (!sifting) {
        std::swap(data[0], data[extractIndex]);
        visualState.write1 = 0;
        visualState.write2 = extractIndex;
        visualState.pivotIdx = extractIndex;
        visualState.rangeL = 0;
        visualState.rangeR = extractIndex;
        visualState.writes++;
        heapSize = extractIndex;
        extractIndex--;
        siftRoot = 0;
        sifting = true;
        return;
    }

    if (siftDownStep(heapSize, siftRoot)) {
        sifting = false;
    }
}
