#ifndef K_WAY_MERGE_SORTER_H
#define K_WAY_MERGE_SORTER_H

#include "ISortingStrategy.h"

class KWayMergeSorter : public ISortingStrategy {
public:
    using ISortingStrategy::ISortingStrategy;

    void reset() override;
    void step() override;

private:
    static constexpr int RUNS = 4;

    bool initialized = false;
    bool sortingRuns = false;
    bool mergeSourceReady = false;
    int sortRunIndex = 0;
    int sortI = 0;
    int sortJ = 0;
    int sortKey = 0;
    bool sortInserting = false;
    int writeIndex = 0;
    std::vector<int> source;
    std::vector<int> output;
    std::vector<int> runStart;
    std::vector<int> runEnd;
    std::vector<int> runCursor;

    void initialize();
    void stepRunSort();
};

#endif
