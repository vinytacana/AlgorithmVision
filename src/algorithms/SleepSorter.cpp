#include "SleepSorter.h"

#include <algorithm>

void SleepSorter::reset() {
    initialized = false;
    writeIndex = 0;
    events.clear();
}

void SleepSorter::initialize() {
    initialized = true;
    writeIndex = 0;
    events.clear();
    events.reserve(data.size());
    for (int i = 0; i < arraySize; ++i) events.push_back({data[i], i});
    std::stable_sort(events.begin(), events.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.first != rhs.first) return lhs.first < rhs.first;
        return lhs.second < rhs.second;
    });
}

void SleepSorter::step() {
    if (!initialized) {
        initialize();
        return;
    }
    if (writeIndex >= arraySize) {
        startFinishAnim();
        return;
    }

    visualState.comp1 = events[writeIndex].second;
    visualState.write1 = writeIndex;
    visualState.pivotIdx = writeIndex;
    data[writeIndex] = events[writeIndex].first;
    visualState.writes++;
    writeIndex++;
}
