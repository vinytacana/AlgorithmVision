#ifndef SORT_SIMULATOR_H
#define SORT_SIMULATOR_H

#include "AlgorithmRegistry.h"
#include "Shader.h"
#include "SortTypes.h"

#include <memory>
#include <vector>

class SortEngine;
class SortRenderer;

class SortSimulator {
public:
    explicit SortSimulator(bool initializeGraphics = true);
    ~SortSimulator();
    SortSimulator(const SortSimulator&) = delete;
    SortSimulator& operator=(const SortSimulator&) = delete;

    void reset();
    void step();
    void render(Shader& shader, int screenWidth, int screenHeight);

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

private:
    std::unique_ptr<SortEngine> engine;
    std::unique_ptr<SortRenderer> renderer;
};

#endif
