#include "ThanosSorter.h"

void ThanosSorter::reset() {}

void ThanosSorter::step() {
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
