#ifndef SLEEP_SORTER_H
#define SLEEP_SORTER_H

#include "ISortingStrategy.h"

#include <utility>

class SleepSorter : public ISortingStrategy {
public:
    using ISortingStrategy::ISortingStrategy;

    void reset() override;
    void step() override;

private:
    bool initialized = false;
    int writeIndex = 0;
    std::vector<std::pair<int, int>> events;

    void initialize();
};

#endif
