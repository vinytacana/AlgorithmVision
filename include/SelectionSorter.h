#ifndef SELECTION_SORTER_H
#define SELECTION_SORTER_H

#include "ISortingStrategy.h"

class SelectionSorter : public ISortingStrategy {
public:
    using ISortingStrategy::ISortingStrategy;

    void reset() override;
    void step() override;

private:
    int i = 0;
    int j = 1;
    int minIndex = 0;
};

#endif
