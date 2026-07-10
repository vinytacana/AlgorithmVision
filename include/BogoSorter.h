#ifndef BOGO_SORTER_H
#define BOGO_SORTER_H

#include "ISortingStrategy.h"

#include <random>

class BogoSorter : public ISortingStrategy {
public:
    BogoSorter(std::vector<int>& dataRef, int& arraySizeRef, SortVisualState& visualStateRef);

    void reset() override;
    void step() override;

private:
    std::mt19937 rng;
    bool shuffling = false;
    int shuffleIndex = -1;
    std::vector<int> swapTargets;

    void prepareShuffle();
};

#endif
