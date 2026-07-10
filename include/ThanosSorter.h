#ifndef THANOS_SORTER_H
#define THANOS_SORTER_H

#include "ISortingStrategy.h"

class ThanosSorter : public ISortingStrategy {
public:
    using ISortingStrategy::ISortingStrategy;

    void reset() override;
    void step() override;
};

#endif
