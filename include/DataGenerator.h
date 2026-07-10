#ifndef DATA_GENERATOR_H
#define DATA_GENERATOR_H

#include "SortTypes.h"

#include <optional>
#include <vector>

class DataGenerator {
public:
    static std::vector<int> generate(
            int arraySize,
            ArrayDistribution distribution,
            std::optional<unsigned int> seed = std::nullopt);
};

#endif
