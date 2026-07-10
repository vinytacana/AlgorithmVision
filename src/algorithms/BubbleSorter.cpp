#include "BubbleSorter.h"

#include <algorithm>

void BubbleSorter::reset() {
    i = 0;
    j = 0;
}

void BubbleSorter::step() {
    if (i < arraySize - 1) {
        if (j < arraySize - i - 1) {
            visualState.comp1 = j;
            visualState.comp2 = j + 1;
            visualState.comparisons++;
            if (data[j] > data[j + 1]) {
                std::swap(data[j], data[j + 1]);
                visualState.write1 = j;
                visualState.write2 = j + 1;
                visualState.writes++;
            }
            j++;
        } else {
            j = 0;
            i++;
        }
    } else {
        startFinishAnim();
    }
}
