#ifndef ALGORITHM_REGISTRY_H
#define ALGORITHM_REGISTRY_H

#include <array>
#include <cstddef>
#include <stdexcept>

enum Algorithm {
    BUBBLE_SORT,
    QUICK_SORT,
    MERGE_SORT,
    INSERTION_SORT,
    SELECTION_SORT,
    SHELL_SORT,
    HEAP_SORT,
    K_WAY_MERGE_SORT,
    MIRACLE_SORT,
    SLEEP_SORT,
    BOGO_SORT,
    STALIN_SORT,
    THANOS_SORT,
    ALGORITHM_COUNT
};

struct AlgorithmMetadata {
    Algorithm algorithm;
    const char* name;
    const char* description;
};

namespace AlgorithmRegistry {

inline constexpr std::array<AlgorithmMetadata, static_cast<std::size_t>(ALGORITHM_COUNT)> kAlgorithmMetadata = {{
    {BUBBLE_SORT, "Bubble Sort", "Bubble Sort: Compara elementos adjacentes e os troca se estiverem na ordem errada. Complexidade: O(n^2)."},
    {QUICK_SORT, "Quick Sort", "Quick Sort: Algoritmo de divisao e conquista. Usa um pivo para particionar o array. Complexidade: O(n log n)."},
    {MERGE_SORT, "Merge Sort", "Merge Sort: Divide o array ao meio recursivamente e mescla as partes ordenadas. Complexidade: O(n log n)."},
    {INSERTION_SORT, "Insertion Sort", "Insertion Sort: Insere cada elemento na posicao correta dentro da parte ja ordenada. Muito bom para listas pequenas ou quase ordenadas. Complexidade: O(n^2)."},
    {SELECTION_SORT, "Selection Sort", "Selection Sort: Procura o menor elemento da parte nao ordenada e o coloca na frente. Faz poucas trocas, mas muitas comparacoes. Complexidade: O(n^2)."},
    {SHELL_SORT, "Shell Sort", "Shell Sort: Generaliza o Insertion Sort usando gaps decrescentes para acelerar a ordenacao de elementos distantes. Complexidade depende da sequencia de gaps."},
    {HEAP_SORT, "Heap Sort", "Heap Sort: Constroi um heap maximo e remove repetidamente o maior elemento para formar a parte ordenada. Complexidade: O(n log n)."},
    {K_WAY_MERGE_SORT, "K-Way Merge Sort", "K-Way Merge Sort: Divide os dados em varios blocos ordenados e mescla todos escolhendo o menor candidato entre os blocos. Complexidade: O(n log k) durante a mesclagem."},
    {MIRACLE_SORT, "Miracle Sort", "Miracle Sort: Nao altera os dados. Ele apenas espera que os elementos ja estejam ordenados por um milagre espontaneo."},
    {SLEEP_SORT, "Sleepsort", "Sleepsort: Simula timers proporcionais aos valores; os menores acordam primeiro e sao escritos antes."},
    {BOGO_SORT, "Bogosort", "Bogosort: Embaralha tudo aleatoriamente e verifica se ficou ordenado. Repete ate a sorte resolver."},
    {STALIN_SORT, "Stalin Sort", "Stalin Sort: Remove qualquer elemento que nao esteja na ordem correta durante uma unica varredura. O que sobra e tecnicamente ordenado."},
    {THANOS_SORT, "Thanos Sort", "Thanos Sort: Exclui metade dos elementos aleatoriamente em um unico passo. O resultado pode ou nao estar ordenado."}
}};

inline const AlgorithmMetadata& getMetadata(Algorithm algorithm) {
    return kAlgorithmMetadata.at(static_cast<std::size_t>(algorithm));
}

inline const AlgorithmMetadata& getMetadataByIndex(int index) {
    if (index < 0 || index >= static_cast<int>(kAlgorithmMetadata.size())) {
        throw std::out_of_range("invalid algorithm index");
    }
    return kAlgorithmMetadata[static_cast<std::size_t>(index)];
}

inline const AlgorithmMetadata* data() {
    return kAlgorithmMetadata.data();
}

inline int count() {
    return static_cast<int>(kAlgorithmMetadata.size());
}

} // namespace AlgorithmRegistry

#endif
