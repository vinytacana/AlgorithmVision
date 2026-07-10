#ifndef SHELL_SORTER_H
#define SHELL_SORTER_H

#include "ISortingStrategy.h"

class ShellSorter : public ISortingStrategy {
public:
    using ISortingStrategy::ISortingStrategy;

    void reset() override;
    void step() override;

private:
    int gap = 0;
    int i = 0;
    int j = 0;
    int temp = 0;
    bool inserting = false;
};

#endif
