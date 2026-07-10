#include "StalinSorter.h"

void StalinSorter::reset() {
    scanIndex = 1;
}

void StalinSorter::step() {
    if (arraySize <= 1 || scanIndex >= arraySize) {
        startFinishAnim();
        return;
    }

    visualState.rangeL = 0;
    visualState.rangeR = arraySize - 1;
    visualState.comp1 = scanIndex - 1;
    visualState.comp2 = scanIndex;
    visualState.pivotIdx = scanIndex;
    visualState.comparisons++;

    if (data[scanIndex] < data[scanIndex - 1]) {
        data.erase(data.begin() + scanIndex);
        arraySize = static_cast<int>(data.size());
        visualState.write1 = scanIndex;
        visualState.write2 = -1;
        visualState.writes++;
        if (scanIndex >= arraySize) {
            startFinishAnim();
        }
        return;
    }

    scanIndex++;
    if (scanIndex >= arraySize) {
        startFinishAnim();
    }
}
