#ifndef MERGE_SORTER_H
#define MERGE_SORTER_H

#include "ISortingStrategy.h"

class MergeSorter : public ISortingStrategy {
public:
    using ISortingStrategy::ISortingStrategy;

    void reset() override;
    void step() override;

private:
    enum class Phase {
        SELECT_RUN,
        MERGE_VALUES,
        COPY_BACK
    };

    int size = 1;
    int left = 0;
    int i = 0;
    int j = 0;
    int mid = 0;
    int right = 0;
    std::vector<int> temp;
    Phase phase = Phase::SELECT_RUN;
    int copyIndex = 0;
};

#endif
