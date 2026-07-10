#include "KWayMergeSorter.h"

#include <algorithm>

void KWayMergeSorter::reset() {
    initialized = false;
    sortingRuns = false;
    mergeSourceReady = false;
    sortRunIndex = 0;
    sortI = 0;
    sortJ = 0;
    sortKey = 0;
    sortInserting = false;
    writeIndex = 0;
    source.clear();
    output.clear();
    runStart.clear();
    runEnd.clear();
    runCursor.clear();
}

void KWayMergeSorter::initialize() {
    initialized = true;
    sortingRuns = true;
    mergeSourceReady = false;
    sortRunIndex = 0;
    sortInserting = false;
    writeIndex = 0;
    output.clear();
    output.reserve(data.size());
    runStart.clear();
    runEnd.clear();
    runCursor.clear();

    const int runs = std::min(RUNS, arraySize);
    for (int run = 0; run < runs; ++run) {
        const int start = run * arraySize / runs;
        const int end = (run + 1) * arraySize / runs;
        runStart.push_back(start);
        runEnd.push_back(end);
        runCursor.push_back(start);
    }
}

void KWayMergeSorter::stepRunSort() {
    if (sortRunIndex >= static_cast<int>(runStart.size())) {
        sortingRuns = false;
        mergeSourceReady = false;
        return;
    }

    const int currentRunStart = runStart[sortRunIndex];
    const int currentRunEnd = runEnd[sortRunIndex];
    visualState.rangeL = currentRunStart;
    visualState.rangeR = currentRunEnd - 1;

    if (currentRunEnd - currentRunStart <= 1) {
        sortRunIndex++;
        sortInserting = false;
        return;
    }

    if (!sortInserting) {
        if (sortI < currentRunStart + 1) sortI = currentRunStart + 1;
        if (sortI >= currentRunEnd) {
            sortRunIndex++;
            sortI = 0;
            sortJ = 0;
            sortInserting = false;
            return;
        }

        sortKey = data[sortI];
        sortJ = sortI - 1;
        sortInserting = true;
        visualState.pivotIdx = sortI;
    }

    visualState.pivotIdx = sortI;
    if (sortJ >= currentRunStart) {
        visualState.comp1 = sortJ;
        visualState.comp2 = sortJ + 1;
        visualState.comparisons++;
        if (data[sortJ] > sortKey) {
            data[sortJ + 1] = data[sortJ];
            visualState.write1 = sortJ;
            visualState.write2 = sortJ + 1;
            visualState.writes++;
            sortJ--;
            return;
        }
    }

    data[sortJ + 1] = sortKey;
    visualState.write1 = sortJ + 1;
    visualState.write2 = -1;
    visualState.writes++;
    sortI++;
    sortInserting = false;
}

void KWayMergeSorter::step() {
    if (!initialized) {
        initialize();
        return;
    }
    if (sortingRuns) {
        stepRunSort();
        return;
    }
    if (!mergeSourceReady) {
        source = data;
        for (int run = 0; run < static_cast<int>(runCursor.size()); ++run) {
            runCursor[run] = runStart[run];
        }
        mergeSourceReady = true;
    }
    if (writeIndex >= arraySize) {
        startFinishAnim();
        return;
    }

    int bestRun = -1;
    for (int run = 0; run < static_cast<int>(runCursor.size()); ++run) {
        if (runCursor[run] >= runEnd[run]) continue;
        if (bestRun == -1) {
            bestRun = run;
            continue;
        }
        visualState.comp1 = runCursor[bestRun];
        visualState.comp2 = runCursor[run];
        visualState.comparisons++;
        if (source[runCursor[run]] < source[runCursor[bestRun]]) {
            bestRun = run;
        }
    }

    if (bestRun == -1) {
        startFinishAnim();
        return;
    }

    output.push_back(source[runCursor[bestRun]]);
    runCursor[bestRun]++;
    data[writeIndex] = output.back();
    visualState.write1 = writeIndex;
    visualState.pivotIdx = writeIndex;
    visualState.rangeL = runStart[bestRun];
    visualState.rangeR = runEnd[bestRun] - 1;
    visualState.writes++;
    writeIndex++;
}
