#include "MiracleSorter.h"

#include <algorithm>

void MiracleSorter::reset() {}

void MiracleSorter::step() {
    if (std::is_sorted(data.begin(), data.end())) {
        startFinishAnim();
        return;
    }

    visualState.rangeL = 0;
    visualState.rangeR = arraySize - 1;
    visualState.pivotIdx = arraySize > 0 ? static_cast<int>(visualState.comparisons % arraySize) : -1;
    visualState.comparisons++;
}
