#include "SortSimulator.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {

bool runUntilFinished(SortSimulator& simulator, int maxSteps = 20000) {
    simulator.toggleSorting();
    for (int step = 0; step < maxSteps; ++step) {
        simulator.step();
        if (simulator.hasFinished()) return true;
    }
    return false;
}

bool testAlgorithm(Algorithm algorithm, const std::vector<int>& input) {
    SortSimulator simulator(false);
    simulator.setAlgorithm(algorithm);
    simulator.setData(input);

    if (!runUntilFinished(simulator)) {
        std::cerr << "Sort did not finish for algorithm " << static_cast<int>(algorithm) << '\n';
        return false;
    }

    std::vector<int> expected = input;
    std::sort(expected.begin(), expected.end());
    if (simulator.getData() != expected) {
        std::cerr << "Unexpected sorted output for algorithm " << static_cast<int>(algorithm) << '\n';
        return false;
    }

    return true;
}

bool testRejectsEmptyData() {
    SortSimulator simulator(false);
    try {
        simulator.setData({});
    } catch (const std::invalid_argument&) {
        return true;
    }
    std::cerr << "Empty input was accepted\n";
    return false;
}

} // namespace

int main() {
    const std::vector<int> input = {5, 1, 4, 2, 8, 3, 7, 6, 2};
    const Algorithm algorithms[] = {
        BUBBLE_SORT,
        QUICK_SORT,
        MERGE_SORT,
        INSERTION_SORT,
        SELECTION_SORT,
        SHELL_SORT
    };

    bool ok = true;
    for (Algorithm algorithm : algorithms) {
        ok = testAlgorithm(algorithm, input) && ok;
    }
    ok = testRejectsEmptyData() && ok;

    return ok ? 0 : 1;
}
