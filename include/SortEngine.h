#ifndef SORT_ENGINE_H
#define SORT_ENGINE_H

#include "AlgorithmRegistry.h"
#include "ISortingStrategy.h"
#include "SortTypes.h"

#include <memory>
#include <vector>

class SortEngine {
public:
    SortEngine();

    void reset();
    void step();

    void setAlgorithm(Algorithm algorithm);
    Algorithm getAlgorithm() const;

    void setRenderMode(RenderMode mode);
    RenderMode getRenderMode() const;

    void setDistribution(ArrayDistribution dist);
    ArrayDistribution getDistribution() const;

    void setArraySize(int size);
    int getArraySize() const;
    void setData(const std::vector<int>& values);
    const std::vector<int>& getData() const;

    void toggleSorting();
    bool isSortingActive() const;
    bool isFinishingAnimation() const;
    bool hasFinished() const;

    long long getComparisons() const;
    long long getWrites() const;
    const SortVisualState& getVisualState() const;

private:
    std::vector<int> data;
    int arraySize = 100;           // tamanho "vivo" (pode encolher em Stalin/Thanos)
    int configuredArraySize = 100; // tamanho escolhido pelo usuario; usado no reset()
    SortVisualState visualState;

    Algorithm selectedAlgo = BUBBLE_SORT;
    RenderMode renderMode = BARS;
    ArrayDistribution distribution = RANDOM;
    std::unique_ptr<ISortingStrategy> currentStrategy;

    std::unique_ptr<ISortingStrategy> createStrategy(Algorithm algorithm);
    void clearVisuals();
    void resetAlgorithmState();
};

#endif
