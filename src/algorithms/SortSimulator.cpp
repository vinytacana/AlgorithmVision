#include "SortSimulator.h"

#include "SortEngine.h"
#include "SortRenderer.h"

#include <utility>

SortSimulator::SortSimulator(bool initializeGraphics)
    : engine(std::make_unique<SortEngine>()),
      renderer(initializeGraphics ? std::make_unique<SortRenderer>() : nullptr) {}

SortSimulator::~SortSimulator() = default;

void SortSimulator::reset() {
    engine->reset();
}

void SortSimulator::step() {
    engine->step();
}

void SortSimulator::render(Shader& shader, int screenWidth, int screenHeight) {
    if (renderer) renderer->render(*engine, shader, screenWidth, screenHeight);
}

void SortSimulator::setAlgorithm(Algorithm algorithm) {
    engine->setAlgorithm(algorithm);
}

Algorithm SortSimulator::getAlgorithm() const {
    return engine->getAlgorithm();
}

void SortSimulator::setRenderMode(RenderMode mode) {
    engine->setRenderMode(mode);
}

RenderMode SortSimulator::getRenderMode() const {
    return engine->getRenderMode();
}

void SortSimulator::setDistribution(ArrayDistribution dist) {
    engine->setDistribution(dist);
}

ArrayDistribution SortSimulator::getDistribution() const {
    return engine->getDistribution();
}

void SortSimulator::setArraySize(int size) {
    engine->setArraySize(size);
}

int SortSimulator::getArraySize() const {
    return engine->getArraySize();
}

void SortSimulator::setData(const std::vector<int>& values) {
    engine->setData(values);
}

const std::vector<int>& SortSimulator::getData() const {
    return engine->getData();
}

void SortSimulator::toggleSorting() {
    engine->toggleSorting();
}

bool SortSimulator::isSortingActive() const {
    return engine->isSortingActive();
}

bool SortSimulator::isFinishingAnimation() const {
    return engine->isFinishingAnimation();
}

bool SortSimulator::hasFinished() const {
    return engine->hasFinished();
}

long long SortSimulator::getComparisons() const {
    return engine->getComparisons();
}

long long SortSimulator::getWrites() const {
    return engine->getWrites();
}
