#ifndef SORT_TYPES_H
#define SORT_TYPES_H

// ATENCAO: a ORDEM destes enums e contrato com a UI Java — os JComboBox de
// visualizacao e distribuicao em java/main/ui/PainelControles.java enviam o
// indice selecionado pelo JNI. Ao alterar a ordem, atualize os dois lados.
enum RenderMode { BARS, DOTS, CIRCULAR, RENDER_MODE_COUNT };
enum ArrayDistribution { RANDOM, REVERSED, NEARLY_SORTED, FEW_UNIQUE, DISTRIBUTION_COUNT };

struct SortVisualState {
    int comp1 = -1;
    int comp2 = -1;
    int write1 = -1;
    int write2 = -1;
    int pivotIdx = -1;
    int rangeL = -1;
    int rangeR = -1;
    int finishIdx = -1;
    long long comparisons = 0;
    long long writes = 0;
    bool isSorting = false;
    bool isFinished = false;
    bool isFinishing = false;
};

#endif
