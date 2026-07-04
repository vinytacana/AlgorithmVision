#include "DataGenerator.h"

#include <algorithm>
#include <random>

std::vector<int> DataGenerator::generate(int arraySize, ArrayDistribution distribution) {
    std::vector<int> data(arraySize);
    std::random_device rd;
    std::mt19937 rng(rd());

    if (distribution == RANDOM) {
        for (int i = 0; i < arraySize; ++i) data[i] = i + 1;
        std::shuffle(data.begin(), data.end(), rng);
    } else if (distribution == REVERSED) {
        for (int i = 0; i < arraySize; ++i) data[i] = arraySize - i;
    } else if (distribution == NEARLY_SORTED) {
        for (int i = 0; i < arraySize; ++i) data[i] = i + 1;
        std::uniform_int_distribution<int> dist(0, arraySize - 1);
        for (int i = 0; i < arraySize / 10; ++i) std::swap(data[dist(rng)], data[dist(rng)]);
    } else if (distribution == FEW_UNIQUE) {
        const int uniqueCount = 5;
        for (int i = 0; i < arraySize; ++i) data[i] = ((i % uniqueCount) + 1) * (arraySize / uniqueCount);
        std::shuffle(data.begin(), data.end(), rng);
    }

    return data;
}
