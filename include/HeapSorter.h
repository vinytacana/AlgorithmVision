#ifndef HEAP_SORTER_H
#define HEAP_SORTER_H

#include "ISortingStrategy.h"

class HeapSorter : public ISortingStrategy {
public:
    using ISortingStrategy::ISortingStrategy;

    void reset() override;
    void step() override;

private:
    int phase = 0;
    int heapSize = 0;
    int buildIndex = 0;
    int extractIndex = 0;
    int siftRoot = 0;
    bool sifting = false;

    bool siftDownStep(int currentHeapSize, int& root);
};

#endif
