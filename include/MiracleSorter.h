#ifndef MIRACLE_SORTER_H
#define MIRACLE_SORTER_H

#include "ISortingStrategy.h"

class MiracleSorter : public ISortingStrategy {
public:
    using ISortingStrategy::ISortingStrategy;

    void reset() override;
    void step() override;
};

#endif
