#ifndef QUICK_SORTER_H
#define QUICK_SORTER_H

#include "ISortingStrategy.h"

#include <stack>
#include <utility>

class QuickSorter : public ISortingStrategy {
public:
    using ISortingStrategy::ISortingStrategy;

    void reset() override;
    void step() override;

private:
    enum class Phase {
        SELECT_RANGE,
        PARTITION
    };

    std::stack<std::pair<int, int>> stack;
    int low = 0;
    int high = 0;
    int pivotValue = 0;
    int i = 0;
    int j = 0;
    Phase phase = Phase::SELECT_RANGE;
};

#endif
