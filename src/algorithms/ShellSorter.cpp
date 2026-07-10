#include "ShellSorter.h"

#include <algorithm>

void ShellSorter::reset() {
    gap = std::max(1, arraySize / 2);
    i = gap;
    j = gap;
    temp = 0;
    inserting = false;
}

void ShellSorter::step() {
    if (gap <= 0) {
        startFinishAnim();
        return;
    }

    if (i >= arraySize) {
        gap /= 2;
        if (gap <= 0) {
            startFinishAnim();
            return;
        }
        i = gap;
        inserting = false;
    }

    if (!inserting) {
        temp = data[i];
        j = i;
        inserting = true;
    }

    visualState.rangeL = 0;
    visualState.rangeR = arraySize - 1;
    visualState.pivotIdx = i;

    if (j >= gap) {
        visualState.comp1 = j - gap;
        visualState.comp2 = j;
        visualState.comparisons++;
        if (data[j - gap] > temp) {
            data[j] = data[j - gap];
            visualState.write1 = j - gap;
            visualState.write2 = j;
            visualState.writes++;
            j -= gap;
            return;
        }
    }

    data[j] = temp;
    visualState.write1 = j;
    visualState.writes++;
    i++;
    inserting = false;
}
