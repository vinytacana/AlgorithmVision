#include "UIComponents.h"
#include "SortSimulator.h"
#include "GraphSimulator.h"

void setupTheme(bool darkMode) {
    if (darkMode) ImGui::StyleColorsDark();
    else ImGui::StyleColorsLight();
}

const char* getAlgorithmDescription(int algo, bool sorting) {
    if (sorting) {
        if (algo == BUBBLE_SORT) return "Bubble Sort: Compara elementos adjacentes e os troca se estiverem na ordem errada. Complexidade: O(n^2).";
        if (algo == QUICK_SORT) return "Quick Sort: Algoritmo de divisao e conquista. Usa um pivo para particionar o array. Complexidade: O(n log n).";
        if (algo == MERGE_SORT) return "Merge Sort: Divide o array ao meio recursivamente e mescla as partes ordenadas. Complexidade: O(n log n).";
    } else {
        if (algo == (int)BFS) return "BFS (Busca em Largura): Explora todos os vizinhos de um no antes de passar para o proximo nivel. Usa uma Fila.";
        if (algo == (int)DFS) return "DFS (Busca em Profundidade): Explora o mais longe possível ao longo de cada ramo antes de retroceder. Usa uma Pilha.";
        if (algo == (int)DIJKSTRA) return "Dijkstra: Encontra o caminho mais curto entre nos em um grafo com pesos positivos. Usa uma Fila de Prioridade.";
    }
    return "";
}
