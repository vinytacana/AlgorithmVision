#ifndef DATA_GENERATOR_H
#define DATA_GENERATOR_H

#include "SortTypes.h"

#include <vector>

class DataGenerator {
public:
    static std::vector<int> generate(int arraySize, ArrayDistribution distribution);
};

#endif
