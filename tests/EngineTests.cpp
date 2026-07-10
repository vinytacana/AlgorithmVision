#include "AlgorithmRegistry.h"
#include "DataGenerator.h"
#include "SortEngine.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

bool runUntilFinished(SortEngine& engine, int maxSteps = 20000) {
    engine.toggleSorting();
    for (int step = 0; step < maxSteps; ++step) {
        engine.step();
        if (engine.hasFinished()) return true;
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
    return AlgorithmRegistry::getMetadata(algorithm).name;
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
    SortEngine engine;
    engine.setAlgorithm(algorithm);
    engine.setData(input);

    if (!runUntilFinished(engine)) {
        std::cerr << "Sort did not finish for " << algorithmName(algorithm) << '\n';
        return false;
    }

    std::vector<int> expected = input;
    std::sort(expected.begin(), expected.end());
    return expectEqual(engine.getData(), expected, algorithmName(algorithm));
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
    SortEngine engine;
    engine.setData({3, 2, 1});
    engine.toggleSorting();
    engine.step();
    engine.setAlgorithm(MERGE_SORT);

    if (engine.isSortingActive() || engine.hasFinished() || engine.isFinishingAnimation()) {
        std::cerr << "Algorithm switch did not reset engine state\n";
        return false;
    }
    if (engine.getAlgorithm() != MERGE_SORT) {
        std::cerr << "Algorithm switch did not update selected algorithm\n";
        return false;
    }
    if (engine.getArraySize() != 3 || engine.getData().size() != 3) {
        std::cerr << "Algorithm switch changed the configured array size unexpectedly\n";
        return false;
    }

    return true;
}

bool testResetPreservesConfiguration() {
    SortEngine engine;
    engine.setArraySize(12);
    engine.setDistribution(REVERSED);
    engine.reset();

    if (engine.getArraySize() != 12) {
        std::cerr << "Reset did not preserve array size\n";
        return false;
    }
    if (engine.isSortingActive() || engine.hasFinished() || engine.isFinishingAnimation()) {
        std::cerr << "Reset left engine in an active or finished state\n";
        return false;
    }

    const std::vector<int> expected = {12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    return expectEqual(engine.getData(), expected, "reversed reset data");
}

bool testDistributionSwitchResetsData() {
    SortEngine engine;
    engine.setArraySize(6);
    engine.setData({3, 1, 2, 6, 5, 4});
    engine.setDistribution(REVERSED);

    if (engine.getDistribution() != REVERSED) {
        std::cerr << "Distribution switch did not update selected distribution\n";
        return false;
    }
    if (engine.isSortingActive() || engine.hasFinished() || engine.isFinishingAnimation()) {
        std::cerr << "Distribution switch left engine in an active or finished state\n";
        return false;
    }

    const std::vector<int> expected = {6, 5, 4, 3, 2, 1};
    return expectEqual(engine.getData(), expected, "distribution switch data");
}

bool testSetArraySizeRejectsInvalidValues() {
    SortEngine engine;
    try {
        engine.setArraySize(0);
    } catch (const std::invalid_argument&) {
        return true;
    }
    std::cerr << "Invalid array size was accepted\n";
    return false;
}

bool testSetArraySizeRejectsTooLargeValues() {
    SortEngine engine;
    try {
        engine.setArraySize(MAX_ARRAY_SIZE + 1);
    } catch (const std::invalid_argument&) {
        return true;
    }
    std::cerr << "Oversized array was accepted\n";
    return false;
}

bool testRejectsEmptyData() {
    SortEngine engine;
    try {
        engine.setData({});
    } catch (const std::invalid_argument&) {
        return true;
    }
    std::cerr << "Empty input was accepted\n";
    return false;
}

bool testRejectsTooLargeData() {
    SortEngine engine;
    try {
        engine.setData(std::vector<int>(MAX_ARRAY_SIZE + 1, 1));
    } catch (const std::invalid_argument&) {
        return true;
    }
    std::cerr << "Oversized data input was accepted\n";
    return false;
}

bool testDataGeneratorSeedIsDeterministic() {
    const auto first = DataGenerator::generate(32, RANDOM, 1234u);
    const auto second = DataGenerator::generate(32, RANDOM, 1234u);
    if (first == second) return true;

    std::cerr << "DataGenerator did not reproduce RANDOM distribution with a fixed seed\n";
    return false;
}

bool testMiracleSortWaitsForUnsortedData() {
    SortEngine engine;
    engine.setAlgorithm(MIRACLE_SORT);
    engine.setData({2, 1});
    engine.toggleSorting();
    for (int i = 0; i < 10; ++i) engine.step();

    if (engine.hasFinished()) {
        std::cerr << "Miracle Sort finished unsorted data without a miracle\n";
        return false;
    }
    return expectEqual(engine.getData(), {2, 1}, "Miracle Sort waiting data");
}

bool testMiracleSortFinishesSortedData() {
    SortEngine engine;
    engine.setAlgorithm(MIRACLE_SORT);
    engine.setData({1, 2, 3});
    if (!runUntilFinished(engine, 10)) {
        std::cerr << "Miracle Sort did not finish already sorted data\n";
        return false;
    }
    return true;
}

bool testMergeSortCopyBackIsIncremental() {
    SortEngine engine;
    engine.setAlgorithm(MERGE_SORT);
    engine.setData({2, 1});
    engine.toggleSorting();
    for (int i = 0; i < 4; ++i) engine.step();

    return expectEqual(engine.getData(), {2, 1}, "Merge Sort incremental copy-back");
}

bool testKWayMergeDoesNotPreSortRunsOnFirstStep() {
    SortEngine engine;
    engine.setAlgorithm(K_WAY_MERGE_SORT);
    engine.setData({2, 1, 4, 3, 6, 5, 8, 7});
    engine.toggleSorting();
    engine.step();

    return expectEqual(engine.getData(), {2, 1, 4, 3, 6, 5, 8, 7}, "K-Way Merge first step preserves unsorted runs");
}

bool testBogoSortFinishesSortedData() {
    SortEngine engine;
    engine.setAlgorithm(BOGO_SORT);
    engine.setData({1, 2, 3});
    if (!runUntilFinished(engine, 10)) {
        std::cerr << "Bogosort did not finish already sorted data\n";
        return false;
    }
    return true;
}

bool testStalinSortRemovesOutOfOrderElements() {
    SortEngine engine;
    engine.setAlgorithm(STALIN_SORT);
    engine.setData({1, 3, 2, 4, 0, 5});

    if (!runUntilFinished(engine, 20)) {
        std::cerr << "Stalin Sort did not finish\n";
        return false;
    }

    if (!expectEqual(engine.getData(), {1, 3, 4, 5}, "Stalin Sort filtered data")) {
        return false;
    }
    if (engine.getArraySize() != 4) {
        std::cerr << "Stalin Sort did not shrink array size correctly\n";
        return false;
    }
    if (engine.getWrites() != 2) {
        std::cerr << "Stalin Sort expected 2 removals but got " << engine.getWrites() << '\n';
        return false;
    }
    if (!engine.hasFinished() || engine.isSortingActive() || engine.isFinishingAnimation()) {
        std::cerr << "Stalin Sort did not settle into final finished state\n";
        return false;
    }
    return true;
}

bool testThanosSortRemovesSecondHalfImmediately() {
    SortEngine engine;
    engine.setAlgorithm(THANOS_SORT);
    engine.setData({8, 6, 7, 5, 3, 0});
    engine.toggleSorting();
    engine.step();

    if (!expectEqual(engine.getData(), {8, 6, 7}, "Thanos Sort snapped half")) {
        return false;
    }
    if (engine.getArraySize() != 3) {
        std::cerr << "Thanos Sort did not halve array size correctly\n";
        return false;
    }
    if (!engine.isFinishingAnimation()) {
        std::cerr << "Thanos Sort should enter finishing animation immediately after snap\n";
        return false;
    }
    return true;
}

bool testThanosSortFinishesAfterSnap() {
    SortEngine engine;
    engine.setAlgorithm(THANOS_SORT);
    engine.setData({10, 9, 8, 7});

    if (!runUntilFinished(engine, 10)) {
        std::cerr << "Thanos Sort did not finish after snap\n";
        return false;
    }

    if (!expectEqual(engine.getData(), {10, 9}, "Thanos Sort final retained half")) {
        return false;
    }
    if (!engine.hasFinished() || engine.isSortingActive() || engine.isFinishingAnimation()) {
        std::cerr << "Thanos Sort did not settle into final finished state\n";
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
    ok = testSetArraySizeRejectsTooLargeValues() && ok;
    ok = testRejectsEmptyData() && ok;
    ok = testRejectsTooLargeData() && ok;
    ok = testDataGeneratorSeedIsDeterministic() && ok;
    ok = testMiracleSortWaitsForUnsortedData() && ok;
    ok = testMiracleSortFinishesSortedData() && ok;
    ok = testMergeSortCopyBackIsIncremental() && ok;
    ok = testKWayMergeDoesNotPreSortRunsOnFirstStep() && ok;
    ok = testBogoSortFinishesSortedData() && ok;
    ok = testStalinSortRemovesOutOfOrderElements() && ok;
    ok = testThanosSortRemovesSecondHalfImmediately() && ok;
    ok = testThanosSortFinishesAfterSnap() && ok;

    return ok ? 0 : 1;
}
