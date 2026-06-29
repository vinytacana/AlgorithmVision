#include "UIComponents.h"
#include "SortSimulator.h"

void setupTheme(bool darkMode) {
    if (darkMode) ImGui::StyleColorsDark();
    else ImGui::StyleColorsLight();
}

const char* getAlgorithmDescription(int algo) {
    if (algo == BUBBLE_SORT) return "Bubble Sort: Compara elementos adjacentes e os troca se estiverem na ordem errada. Complexidade: O(n^2).";
    if (algo == QUICK_SORT) return "Quick Sort: Algoritmo de divisao e conquista. Usa um pivo para particionar o array. Complexidade: O(n log n).";
    if (algo == MERGE_SORT) return "Merge Sort: Divide o array ao meio recursivamente e mescla as partes ordenadas. Complexidade: O(n log n).";
    if (algo == INSERTION_SORT) return "Insertion Sort: Insere cada elemento na posicao correta dentro da parte ja ordenada. Muito bom para listas pequenas ou quase ordenadas. Complexidade: O(n^2).";
    if (algo == SELECTION_SORT) return "Selection Sort: Procura o menor elemento da parte nao ordenada e o coloca na frente. Faz poucas trocas, mas muitas comparacoes. Complexidade: O(n^2).";
    if (algo == SHELL_SORT) return "Shell Sort: Generaliza o Insertion Sort usando gaps decrescentes para acelerar a ordenacao de elementos distantes. Complexidade depende da sequencia de gaps.";
    return "";
}
