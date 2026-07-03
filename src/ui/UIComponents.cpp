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
    if (algo == HEAP_SORT) return "Heap Sort: Constroi um heap maximo e remove repetidamente o maior elemento para formar a parte ordenada. Complexidade: O(n log n).";
    if (algo == K_WAY_MERGE_SORT) return "K-Way Merge Sort: Divide os dados em varios blocos ordenados e mescla todos escolhendo o menor candidato entre os blocos. Complexidade: O(n log k) durante a mesclagem.";
    if (algo == MIRACLE_SORT) return "Miracle Sort: Nao altera os dados. Ele apenas espera que os elementos ja estejam ordenados por um milagre espontaneo.";
    if (algo == SLEEP_SORT) return "Sleepsort: Simula timers proporcionais aos valores; os menores acordam primeiro e sao escritos antes.";
    if (algo == BOGO_SORT) return "Bogosort: Embaralha tudo aleatoriamente e verifica se ficou ordenado. Repete ate a sorte resolver.";
    return "";
}
