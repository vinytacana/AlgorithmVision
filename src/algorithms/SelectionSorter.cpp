#include "SelectionSorter.h"

#include <algorithm>

void SelectionSorter::reset() {
    i = 0;
    j = 1;
    minIndex = 0;
}

void SelectionSorter::step() {
    if (i >= arraySize - 1) {
        startFinishAnim();
        return;
    }

    visualState.rangeL = i;
    visualState.rangeR = arraySize - 1;
    visualState.pivotIdx = minIndex;

    if (j < arraySize) {
        visualState.comp1 = minIndex;
        visualState.comp2 = j;
        visualState.comparisons++;
        if (data[j] < data[minIndex]) {
            minIndex = j;
            visualState.pivotIdx = minIndex;
        }
        j++;
        return;
    }

    if (minIndex != i) {
        std::swap(data[i], data[minIndex]);
        visualState.write1 = i;
        visualState.write2 = minIndex;
        visualState.writes++;
    }
    i++;
    minIndex = i;
    j = i + 1;
}
