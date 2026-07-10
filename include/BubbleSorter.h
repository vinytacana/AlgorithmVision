#ifndef BUBBLE_SORTER_H
#define BUBBLE_SORTER_H

#include "ISortingStrategy.h"

class BubbleSorter : public ISortingStrategy {
public:
    using ISortingStrategy::ISortingStrategy;

    void reset() override;
    void step() override;

private:
    int i = 0;
    int j = 0;
};

#endif
