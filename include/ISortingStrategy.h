#ifndef ISORTING_STRATEGY_H
#define ISORTING_STRATEGY_H

#include "SortTypes.h"

#include <vector>

class ISortingStrategy {
public:
    ISortingStrategy(std::vector<int>& dataRef, int& arraySizeRef, SortVisualState& visualStateRef)
        : data(dataRef), arraySize(arraySizeRef), visualState(visualStateRef) {}

    virtual ~ISortingStrategy() = default;

    virtual void reset() = 0;
    virtual void step() = 0;

protected:
    std::vector<int>& data;
    int& arraySize;
    SortVisualState& visualState;

    void clearVisuals() {
        visualState.comp1 = -1;
        visualState.comp2 = -1;
        visualState.write1 = -1;
        visualState.write2 = -1;
        visualState.pivotIdx = -1;
        visualState.rangeL = -1;
        visualState.rangeR = -1;
    }

    void startFinishAnim() {
        visualState.isSorting = false;
        visualState.isFinishing = true;
        visualState.finishIdx = 0;
        clearVisuals();
    }
};

#endif
