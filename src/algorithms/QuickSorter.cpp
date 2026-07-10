#include "QuickSorter.h"

#include <algorithm>

void QuickSorter::reset() {
    while (!stack.empty()) stack.pop();
    stack.push({0, arraySize - 1});
    low = 0;
    high = 0;
    pivotValue = 0;
    i = 0;
    j = 0;
    phase = Phase::SELECT_RANGE;
}

void QuickSorter::step() {
    if (phase == Phase::SELECT_RANGE) {
        if (stack.empty()) {
            startFinishAnim();
            return;
        }
        const auto range = stack.top();
        stack.pop();
        low = range.first;
        high = range.second;
        if (low >= high) {
            return;
        }

        pivotValue = data[high];
        visualState.pivotIdx = high;
        i = low - 1;
        j = low;
        phase = Phase::PARTITION;
        visualState.rangeL = low;
        visualState.rangeR = high;
        return;
    }

    visualState.pivotIdx = high;
    visualState.rangeL = low;
    visualState.rangeR = high;
    if (j < high) {
        visualState.comp1 = j;
        visualState.comp2 = high;
        visualState.comparisons++;
        if (data[j] < pivotValue) {
            i++;
            std::swap(data[i], data[j]);
            visualState.write1 = i;
            visualState.write2 = j;
            visualState.writes++;
        }
        j++;
        return;
    }

    std::swap(data[i + 1], data[high]);
    visualState.write1 = i + 1;
    visualState.write2 = high;
    visualState.writes++;
    const int pivot = i + 1;
    stack.push({pivot + 1, high});
    stack.push({low, pivot - 1});
    phase = Phase::SELECT_RANGE;
}
