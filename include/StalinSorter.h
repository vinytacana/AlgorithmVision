#ifndef STALIN_SORTER_H
#define STALIN_SORTER_H

#include "ISortingStrategy.h"

class StalinSorter : public ISortingStrategy {
public:
    using ISortingStrategy::ISortingStrategy;

    void reset() override;
    void step() override;

private:
    int scanIndex = 1;
};

#endif
