#include "MergeSorter.h"

#include <algorithm>

void MergeSorter::reset() {
    size = 1;
    left = 0;
    i = 0;
    j = 0;
    mid = 0;
    right = 0;
    temp.clear();
    phase = Phase::SELECT_RUN;
    copyIndex = 0;
}

void MergeSorter::step() {
    if (phase == Phase::SELECT_RUN) {
        if (size >= arraySize) {
            startFinishAnim();
            return;
        }

        if (left >= arraySize - 1) {
            left = 0;
            size *= 2;
            return;
        }

        mid = std::min(left + size - 1, arraySize - 1);
        right = std::min(left + 2 * size - 1, arraySize - 1);
        temp.clear();
        i = left;
        j = mid + 1;
        copyIndex = 0;
        phase = Phase::MERGE_VALUES;
        visualState.rangeL = left;
        visualState.rangeR = right;
        return;
    }

    visualState.rangeL = left;
    visualState.rangeR = right;
    if (phase == Phase::COPY_BACK) {
        data[left + copyIndex] = temp[copyIndex];
        visualState.write1 = left + copyIndex;
        visualState.write2 = -1;
        visualState.pivotIdx = left + copyIndex;
        visualState.writes++;
        copyIndex++;
        if (copyIndex >= static_cast<int>(temp.size())) {
            left += 2 * size;
            phase = Phase::SELECT_RUN;
            copyIndex = 0;
        }
        return;
    }

    if (i <= mid && j <= right) {
        visualState.comp1 = i;
        visualState.comp2 = j;
        visualState.comparisons++;
        if (data[i] <= data[j]) temp.push_back(data[i++]);
        else temp.push_back(data[j++]);
    } else if (i <= mid) {
        temp.push_back(data[i++]);
    } else if (j <= right) {
        temp.push_back(data[j++]);
    } else {
        phase = Phase::COPY_BACK;
        copyIndex = 0;
        return;
    }
    visualState.write1 = left + static_cast<int>(temp.size()) - 1;
    visualState.write2 = -1;
}
