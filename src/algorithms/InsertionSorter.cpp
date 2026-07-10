#include "InsertionSorter.h"

void InsertionSorter::reset() {
    i = 1;
    j = 0;
    key = 0;
    inserting = false;
}

void InsertionSorter::step() {
    if (i >= arraySize) {
        startFinishAnim();
        return;
    }

    if (!inserting) {
        key = data[i];
        j = i - 1;
        visualState.pivotIdx = i;
        inserting = true;
    }

    visualState.pivotIdx = i;
    if (j >= 0) {
        visualState.comp1 = j;
        visualState.comp2 = j + 1;
        visualState.comparisons++;
        if (data[j] > key) {
            data[j + 1] = data[j];
            visualState.write1 = j;
            visualState.write2 = j + 1;
            visualState.writes++;
            j--;
            return;
        }
    }

    data[j + 1] = key;
    visualState.write1 = j + 1;
    visualState.writes++;
    i++;
    inserting = false;
}
