# 🚀 AlgorithmVision Pro: Master Edition
### *Simulador de Algoritmos com Renderização de Alto Nível em OpenGL*

![C++](https://img.shields.io/badge/C%2B%2B-17-blue?style=for-the-badge&logo=c%2B%2B)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3-red?style=for-the-badge&logo=opengl)
![Status](https://img.shields.io/badge/Status-Finalizado-green?style=for-the-badge)

**AlgorithmVision Pro** é uma plataforma de visualização pedagógica que utiliza o poder da **Computação Gráfica moderna** para desmistificar a complexidade de algoritmos clássicos. Desenvolvido com uma arquitetura modular robusta, o simulador oferece uma experiência interativa onde o usuário manipula estruturas de dados em tempo real.

---

## 💎 Diferenciais Técnicos da Versão Atualizada

Esta versão foi otimizada para performance e compatibilidade multiplataforma (Fedora/Ubuntu):

*   **Arquitetura de Renderização Otimizada:** Implementação de um sistema de **Uniform Caching** na classe `Shader`, reduzindo drasticamente as chamadas redundantes ao driver gráfico (`glGetUniformLocation`).
*   **Compatibilidade Cross-Distro (Fedora/Ubuntu):** Inicialização robusta do **GLEW** que detecta e contorna o erro falso de inicialização comum em drivers Mesa/Wayland no Fedora, garantindo que o contexto OpenGL seja criado corretamente.
*   **Segurança de Memória (Memory Safety):** Lógica de renderização resiliente que utiliza `std::vector::size()` dinâmico, evitando acessos fora dos limites (`Assertion failed`) mesmo durante mudanças de parâmetros na UI em tempo real.
*   **Pipeline de Shaders Moderno:** Shaders GLSL 3.30 Core com suporte a estados visuais dinâmicos (Pivôs, Comparações, Escritas e Animação de Conclusão).

---

## 🛠️ O Simulador de Ordenação (Sorting)

A visualização de ordenação transforma o array em elementos geométricos que "dançam" conforme a lógica do algoritmo.

### Modos de Visualização:
1.  **Barras (Visual Clássico):** Ideal para ver a ordenação por trocas adjacentes.
2.  **Dot Plot (Nuvem de Dados):** Perfeito para visualizar a convergência estatística do array.
3.  **Circular (Radial):** Transforma o array em um relógio cromático, onde ângulos e raios representam os valores. É o modo mais estético e hipnotizante.

### Algoritmos Implementados:
-   **Bubble Sort:** Visualização da "flutuação" dos maiores valores.
-   **Quick Sort (Lomuto):** Demonstração clara da partição recursiva em torno de um pivô colorido.
-   **Merge Sort (Bottom-Up):** Visualização granular da fusão de sub-blocos no array principal.

---

## 🌐 O Simulador de Grafos (Interativo)

O simulador de grafos permite a criação de redes complexas para testes de busca e otimização com interação direta.

### Algoritmos de Caminho e Busca:
-   **Dijkstra:** Encontra e desenha o caminho mais curto em destaque visual.
-   **BFS / DFS:** Mostra a expansão da fronteira de busca e os nós já visitados com cores distintas.

### Guia de Atalhos (Interação de Mouse):
| Comando | Ação |
| :--- | :--- |
| **Clique Esquerdo (Arraste)** | Move um nó existente |
| **Shift + Clique Esquerdo** | Adiciona um novo Nó na posição |
| **Ctrl + Clique (em 2 nós)** | Cria uma Aresta entre eles |
| **Clique Direito** | Remove o Nó selecionado |

---

## 🏗️ Detalhes da Implementação

### Engenharia de Shaders (GLSL):
*   `bar.vs/fs`: Renderização de primitivas retangulares com interpolação de cores por estado.
*   `circle.vs/fs`: Geometria procedural para nodos circulares com detecção de bordas.
*   `line.vs/fs`: Renderização dinâmica de arestas com suporte a pesos visuais.

### Estrutura do Projeto:
*   **`Shader`**: Gerenciador de programas GLSL com cache de localização de uniforms para alta performance.
*   **`SortSimulator`**: Implementação "steppable" de algoritmos de ordenação, permitindo controle granular de velocidade (Tick Rate).
*   **`GraphSimulator`**: Motor de grafos baseado em adjacência com manipulação espacial de nodos.
*   **`UIComponents`**: Camada de interface construída sobre **Dear ImGui** para controle de parâmetros e descrições pedagógicas.

---

## 🚀 Como Compilar e Rodar

### Dependências (Fedora)
```bash
sudo dnf install gcc-c++ cmake glew-devel glfw-devel mesa-libGL-devel glm-devel
```

### Dependências (Ubuntu)
```bash
sudo apt update
sudo apt install build-essential cmake libglew-dev libglfw3-dev libglm-dev libgl1-mesa-dev
```

### Build e Execução
```bash
# Na raiz do projeto
mkdir -p build && cd build
cmake ..
make -j$(nproc)
cd ..

# Executar a partir da raiz para carregar os shaders corretamente
./build/AlgorithmVision
```

---

## 🎨 Customização e UI
O painel de controle permite alternar entre **Modo Claro** e **Modo Escuro**, além de ajustar a **Velocidade (Tick Rate)** e o **Tamanho do Array (N)**. A aplicação sincroniza automaticamente o estado dos simuladores ao alterar parâmetros na interface.

---

**AlgorithmVision Pro** - *Transformando código em arte visual através da Computação Gráfica.*
