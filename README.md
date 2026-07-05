# AlgorithmVision

An interactive, real-time sorting algorithm visualizer. The user interface is a **Java Swing** application (OOP layer) that consumes a **native C++/OpenGL rendering library** through **JNI** (Computer Graphics layer) — the three-layer architecture of the interdisciplinary project (POO + CG).

The native engine is a **passive library**: it owns no window and no loop. Java owns the whole application — a single Swing window drives the simulation clock and asks the engine for offscreen-rendered frames, which are displayed in a panel.

## Key Features

- Single-window Swing application: controls on the left, OpenGL-rendered visualization in the center.
- Sorting visualizer with stepwise animations for traditional, didactic, and meme algorithms.
- Visual modes for bar charts, dot plots, and circular layouts.
- Data generation presets for random, reversed, nearly sorted, and few unique arrays.
- Offscreen OpenGL rendering (FBO) delivered to Java as ARGB pixel frames via JNI.

## Algorithms

- Traditional/didactic: Bubble Sort, Quick Sort, Merge Sort, Insertion Sort, Selection Sort, Shell Sort, Heap Sort, K-Way Merge Sort.
- Meme/experimental: Miracle Sort, Sleepsort, Bogosort, Stalin Sort, Thanos Sort.

## Architecture

```text
+--------------------------------------------------------------+
| Aplicacao Java (java/main) - POO                             |
|   main.ui.JanelaPrincipal   (heranca: extends JFrame)        |
|   main.ui.PainelRenderizacao(polimorfismo: paintComponent)   |
|   main.ui.PainelControles   (depende so da abstracao)        |
|   main.controle.ControladorSimulacao      <<interface>>      |
|   main.controle.ControladorMotorNativo    (implementacao)    |
|   main.MotorGrafico         (encapsula o JNI)                |
+-------------------------------+------------------------------+
                                | JNI (headers/main_MotorGrafico.h)
+-------------------------------v------------------------------+
| Biblioteca nativa (src/jni/motor.cpp) - CG                   |
|   Contexto OpenGL oculto + FBO offscreen -> pixels ARGB      |
|   SortSimulator -> SortEngine (logica) + SortRenderer (GL)   |
|   AlgorithmRegistry: fonte unica de nomes/descricoes         |
+--------------------------------------------------------------+
```

OOP principles in the Java layer:

- **Abstraction/Polymorphism**: the UI depends only on the `ControladorSimulacao` interface; swapping the implementation (native engine, mock, pure-Java) requires no UI changes. `PainelRenderizacao` overrides `paintComponent`.
- **Inheritance**: `JanelaPrincipal extends JFrame`, panels extend `JPanel`.
- **Encapsulation**: `MotorGrafico` is the only class that knows JNI exists.

Native layer:

- `SortEngine`: owns array state, algorithm phases, counters, and step-by-step transitions.
- `SortRenderer`: owns OpenGL mesh setup and draws the current engine state.
- `SortSimulator`: thin facade composing engine and renderer.
- `AlgorithmRegistry`: single source of truth for algorithm enum values, UI names, and pedagogical descriptions.
- `src/jni/motor.cpp`: implements the JNI contract; renders offscreen into an FBO and returns ARGB frames.

Threading contract: every native call must happen on the thread that called `init()`. The Swing app satisfies this by doing everything (init, animation `Timer`, shutdown) on the Event Dispatch Thread.

## Tech Stack

- Application/UI: Java 17+ (Swing)
- Bridge: JNI
- Engine: C++17, OpenGL 3.3 Core Profile (offscreen/FBO)
- Context/loader: GLFW (hidden window) + vendored GLAD
- Mathematics: GLM

## Build

Requirements: C++ compiler, CMake, JDK 17+, GLFW, GLM, and the vendored GLAD files in `glad/`.

```bash
# 1. Compile the Java classes and generate the JNI header
javac -encoding UTF-8 -h headers -d java/build \
      java/main/*.java java/main/controle/*.java java/main/ui/*.java

# 2. Build the native library (target enabled automatically when a JDK is found)
cmake -S . -B build
cmake --build build --target motor

# 3. Run (CWD must be build/ so the engine finds shaders/)
cd build && java -Djava.library.path=. -cp ../java/build main.Main
```

On Windows there is a shortcut: `run-integrado.bat`. A CMake-free manual build script for Git Bash is also available: `./compilar.sh`.

## Tests

- `EngineTests` (C++, headless): validates sorting behavior without any OpenGL dependency.

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

- `main.TesteIntegracao` (Java, no visible window): exercises the full Java -> JNI -> OpenGL offscreen path and checks that rendered frames contain content and metrics evolve.

```bash
cd build && java -Djava.library.path=. -cp ../java/build main.TesteIntegracao
```

## Notes

- The repository currently does not include a `LICENSE` file.
- In headless or remote environments without a display server, GLFW context creation may fail even though `EngineTests` passes.
