#include "SortSimulator.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <stdexcept>
#include <string>
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

const std::array<Algorithm, 9> kDeterministicAlgorithms = {
    BUBBLE_SORT,
    QUICK_SORT,
    MERGE_SORT,
    INSERTION_SORT,
    SELECTION_SORT,
    SHELL_SORT,
    HEAP_SORT,
    K_WAY_MERGE_SORT,
    SLEEP_SORT
};

std::string algorithmName(Algorithm algorithm) {
    switch (algorithm) {
        case BUBBLE_SORT: return "Bubble Sort";
        case QUICK_SORT: return "Quick Sort";
        case MERGE_SORT: return "Merge Sort";
        case INSERTION_SORT: return "Insertion Sort";
        case SELECTION_SORT: return "Selection Sort";
        case SHELL_SORT: return "Shell Sort";
        case HEAP_SORT: return "Heap Sort";
        case K_WAY_MERGE_SORT: return "K-Way Merge Sort";
        case MIRACLE_SORT: return "Miracle Sort";
        case SLEEP_SORT: return "Sleepsort";
        case BOGO_SORT: return "Bogosort";
        case ALGORITHM_COUNT: break;
    }
    return "Unknown";
}

bool expectEqual(const std::vector<int>& actual, const std::vector<int>& expected, const std::string& context) {
    if (actual == expected) return true;

    std::cerr << "Unexpected data for " << context << '\n';
    std::cerr << "Expected:";
    for (int value : expected) std::cerr << ' ' << value;
    std::cerr << "\nActual:";
    for (int value : actual) std::cerr << ' ' << value;
    std::cerr << '\n';
    return false;
}

bool testAlgorithmWithInput(Algorithm algorithm, const std::vector<int>& input) {
    SortSimulator simulator(false);
    simulator.setAlgorithm(algorithm);
    simulator.setData(input);

    if (!runUntilFinished(simulator)) {
        std::cerr << "Sort did not finish for " << algorithmName(algorithm) << '\n';
        return false;
    }

    std::vector<int> expected = input;
    std::sort(expected.begin(), expected.end());
    return expectEqual(simulator.getData(), expected, algorithmName(algorithm));
}

bool testAllAlgorithmsHandleInput(const std::vector<int>& input, const std::string& label) {
    bool ok = true;
    for (Algorithm algorithm : kDeterministicAlgorithms) {
        ok = testAlgorithmWithInput(algorithm, input) && ok;
    }
    if (!ok) std::cerr << "Failed input case: " << label << '\n';
    return ok;
}

bool testAlgorithmSwitchResetsState() {
    SortSimulator simulator(false);
    simulator.setData({3, 2, 1});
    simulator.toggleSorting();
    simulator.step();
    simulator.setAlgorithm(MERGE_SORT);

    if (simulator.isSortingActive() || simulator.hasFinished() || simulator.isFinishingAnimation()) {
        std::cerr << "Algorithm switch did not reset simulator state\n";
        return false;
    }
    if (simulator.getAlgorithm() != MERGE_SORT) {
        std::cerr << "Algorithm switch did not update selected algorithm\n";
        return false;
    }
    if (simulator.getArraySize() != 3 || simulator.getData().size() != 3) {
        std::cerr << "Algorithm switch changed the configured array size unexpectedly\n";
        return false;
    }

    return true;
}

bool testResetPreservesConfiguration() {
    SortSimulator simulator(false);
    simulator.setArraySize(12);
    simulator.setDistribution(REVERSED);
    simulator.reset();

    if (simulator.getArraySize() != 12) {
        std::cerr << "Reset did not preserve array size\n";
        return false;
    }
    if (simulator.isSortingActive() || simulator.hasFinished() || simulator.isFinishingAnimation()) {
        std::cerr << "Reset left simulator in an active or finished state\n";
        return false;
    }

    const std::vector<int> expected = {12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    return expectEqual(simulator.getData(), expected, "reversed reset data");
}

bool testDistributionSwitchResetsData() {
    SortSimulator simulator(false);
    simulator.setArraySize(6);
    simulator.setData({3, 1, 2, 6, 5, 4});
    simulator.setDistribution(REVERSED);

    if (simulator.getDistribution() != REVERSED) {
        std::cerr << "Distribution switch did not update selected distribution\n";
        return false;
    }
    if (simulator.isSortingActive() || simulator.hasFinished() || simulator.isFinishingAnimation()) {
        std::cerr << "Distribution switch left simulator in an active or finished state\n";
        return false;
    }

    const std::vector<int> expected = {6, 5, 4, 3, 2, 1};
    return expectEqual(simulator.getData(), expected, "distribution switch data");
}

bool testSetArraySizeRejectsInvalidValues() {
    SortSimulator simulator(false);
    try {
        simulator.setArraySize(0);
    } catch (const std::invalid_argument&) {
        return true;
    }
    std::cerr << "Invalid array size was accepted\n";
    return false;
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

bool testMiracleSortWaitsForUnsortedData() {
    SortSimulator simulator(false);
    simulator.setAlgorithm(MIRACLE_SORT);
    simulator.setData({2, 1});
    simulator.toggleSorting();
    for (int i = 0; i < 10; ++i) simulator.step();

    if (simulator.hasFinished()) {
        std::cerr << "Miracle Sort finished unsorted data without a miracle\n";
        return false;
    }
    return expectEqual(simulator.getData(), {2, 1}, "Miracle Sort waiting data");
}

bool testMiracleSortFinishesSortedData() {
    SortSimulator simulator(false);
    simulator.setAlgorithm(MIRACLE_SORT);
    simulator.setData({1, 2, 3});
    if (!runUntilFinished(simulator, 10)) {
        std::cerr << "Miracle Sort did not finish already sorted data\n";
        return false;
    }
    return true;
}

bool testBogoSortFinishesSortedData() {
    SortSimulator simulator(false);
    simulator.setAlgorithm(BOGO_SORT);
    simulator.setData({1, 2, 3});
    if (!runUntilFinished(simulator, 10)) {
        std::cerr << "Bogosort did not finish already sorted data\n";
        return false;
    }
    return true;
}

} // namespace

int main() {
    bool ok = true;
    ok = testAllAlgorithmsHandleInput({5, 1, 4, 2, 8, 3, 7, 6, 2}, "mixed duplicates") && ok;
    ok = testAllAlgorithmsHandleInput({9, 8, 7, 6, 5, 4, 3, 2, 1}, "reversed") && ok;
    ok = testAllAlgorithmsHandleInput({1}, "single element") && ok;
    ok = testAllAlgorithmsHandleInput({2, 1}, "two elements") && ok;
    ok = testAllAlgorithmsHandleInput({4, 4, 4, 2, 2, 1}, "many duplicates") && ok;
    ok = testAlgorithmSwitchResetsState() && ok;
    ok = testResetPreservesConfiguration() && ok;
    ok = testDistributionSwitchResetsData() && ok;
    ok = testSetArraySizeRejectsInvalidValues() && ok;
    ok = testRejectsEmptyData() && ok;
    ok = testMiracleSortWaitsForUnsortedData() && ok;
    ok = testMiracleSortFinishesSortedData() && ok;
    ok = testBogoSortFinishesSortedData() && ok;

    return ok ? 0 : 1;
}
