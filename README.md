# AlgorithmVision

An interactive, real-time sorting algorithm visualizer built with C++ and OpenGL. Explore sorting strategies with dynamic, shader-powered visualizations and integrated UI controls.

## Key Features

- Sorting visualizer with stepwise animations for traditional, didactic, and meme algorithms.
- Visual modes for bar charts, dot plots, and circular layouts.
- Data generation presets for random, reversed, nearly sorted, and few unique arrays.
- Dear ImGui control panel for algorithm choice, input tuning, speed, and reset.
- Dynamic viewport and projection updates for framebuffer resize and maximized windows.

## Algorithms

- Traditional/didactic: Bubble Sort, Quick Sort, Merge Sort, Insertion Sort, Selection Sort, Shell Sort, Heap Sort, K-Way Merge Sort.
- Meme/experimental: Miracle Sort, Sleepsort, Bogosort, Stalin Sort, Thanos Sort.

## Architecture

- `SortEngine`: owns array state, algorithm phases, counters, and step-by-step transitions.
- `SortRenderer`: owns OpenGL mesh setup and draws the current engine state using the active framebuffer size.
- `SortSimulator`: thin facade used by the app layer to compose engine and renderer.
- `AlgorithmRegistry`: single source of truth for algorithm enum values, UI names, and pedagogical descriptions.

## Tech Stack

- Core: C++17
- Graphics: OpenGL 3.3 Core Profile
- UI: Dear ImGui
- Windowing/Input: GLFW
- Mathematics: GLM
- Extensions: vendored GLAD

## Build

To build the project, you need to have a C++ compiler, CMake, and the following dependencies installed:

- GLFW
- GLM
- Vendored GLAD files in `glad/src/glad.c` and `glad/include/glad/glad.h`

```bash
cmake -S . -B build
cmake --build build
./build/AlgorithmVision
```

The build copies `shaders/` next to the executable automatically, so runtime shader lookup works from `build/`.

## Integracao Java (JNI)

Alem do executavel standalone (ImGui), o projeto oferece um modo integrado em que a interface grafica e uma aplicacao **Java Swing** e o motor C++/OpenGL e carregado pela JVM como biblioteca nativa (**JNI**), seguindo a arquitetura em tres camadas do Projeto Interdisciplinar (POO + CG):

```text
Java Swing (java/main/ControlUI.java)
        | metodos nativos (contrato: java/main/MotorGrafico.java)
JNI (headers/main_MotorGrafico.h, gerado por javac -h)
        |
Motor C++ (src/jni/motor.cpp) -> reusa SortSimulator/SortEngine/SortRenderer
```

- A thread Java que chama `MotorGrafico.init()` torna-se a thread de renderizacao (dona do contexto OpenGL).
- Comandos vindos da EDT do Swing sao **enfileirados com mutex** e aplicados pelo loop de render a cada frame (sem corridas de dados).
- Metricas (comparacoes/acessos/status) sao expostas por snapshots atomicos e lidas por polling no Swing.

### Build e execucao (requer JDK 17+)

```bash
# 1. Compilar as classes Java e gerar o header JNI
javac -encoding UTF-8 -h headers -d java/build java/main/*.java

# 2. Compilar a biblioteca nativa (alvo habilitado automaticamente se ha JDK)
cmake -S . -B build
cmake --build build --target motor

# 3. Executar (o CWD deve ser build/ para o motor achar shaders/)
cd build && java -Djava.library.path=. -cp ../java/build main.Main
```

No Windows ha um atalho: `run-integrado.bat`.

## Tests

The project ships a headless logic target, `EngineTests`, that validates sorting behavior without depending on OpenGL rendering objects.

```bash
cmake -S . -B build
cmake --build build
ctest --output-on-failure
```

## Notes

- The repository currently does not include a `LICENSE` file.
- In headless or remote environments without a display server, the graphical application may fail during GLFW initialization even when `EngineTests` passes.
