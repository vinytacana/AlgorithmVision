#ifndef INSERTION_SORTER_H
#define INSERTION_SORTER_H

#include "ISortingStrategy.h"

class InsertionSorter : public ISortingStrategy {
public:
    using ISortingStrategy::ISortingStrategy;

    void reset() override;
    void step() override;

private:
    int i = 1;
    int j = 0;
    int key = 0;
    bool inserting = false;
};

#endif
