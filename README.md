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

### Fedora / Linux

```bash
# 0. Install the required packages
sudo dnf install gcc-c++ cmake glfw-devel glm-devel java-17-openjdk-devel mesa-libGL-devel

# 1. Compile the Java classes and generate the JNI header
javac -encoding UTF-8 -h headers -d java/build \
      java/main/*.java java/main/controle/*.java java/main/ui/*.java

# 2. Build the native library (produces build/libmotor.so)
cmake -S . -B build
cmake --build build --target motor

# 3. Run
./run-integrado.sh
```

The launcher forces `GLFW_PLATFORM=x11` by default so the offscreen context is created through XWayland (the most battle-tested path on Wayland sessions such as Fedora's default); export `GLFW_PLATFORM=wayland` to override. To validate the machine before a presentation, run the headless integration test first:

```bash
cd build && java -Djava.library.path=. -cp ../java/build main.TesteIntegracao
```

### Windows

```bash
# 1. Compile the Java classes and generate the JNI header
javac -encoding UTF-8 -h headers -d java/build \
      java/main/*.java java/main/controle/*.java java/main/ui/*.java

# 2. Build the native library (produces build/motor.dll)
cmake -S . -B build
cmake --build build --target motor

# 3. Run (CWD must be build/ so the engine finds shaders/)
cd build && java -Djava.library.path=. -cp ../java/build main.Main
```

Shortcut: `run-integrado.bat`. A CMake-free manual build script for Git Bash (MinGW/Windows only) is also available: `./compilar.sh`.

## Mapeamento para a estrutura do tutorial (PDF)

O tutorial do projeto interdisciplinar usa uma estrutura de diretorios de referencia; este repositorio a adapta (a especificacao permite adaptacao). Correspondencia:

| Tutorial (PDF)                         | Este repositorio                                             |
| -------------------------------------- | ------------------------------------------------------------ |
| `src/main/*.java`                      | `java/main/**/*.java`                                        |
| `native/motor.cpp`                     | `src/jni/motor.cpp` + `src/algorithms` + `src/renderer`      |
| `headers/main_MotorGrafico.h`          | `headers/main_MotorGrafico.h` (identico)                     |
| `build/` (classes Java)                | `java/build/`                                                |
| `lib/` (biblioteca nativa)             | `build/` (gerado pelo CMake)                                 |
| `TransformUI.java`                     | `JanelaPrincipal` + `PainelControles` + `PainelRenderizacao` |
| Execucao com `-Djava.library.path=lib` | `./run-integrado.sh` ou `run-integrado.bat`                  |

O essencial do contrato JNI e preservado: pacote `main`, classe `MotorGrafico` com `System.loadLibrary("motor")`, header gerado por `javac -h headers`, funcoes nativas `Java_main_MotorGrafico_*`, `motor.dll` no Windows e `libmotor.so` no Linux.

## Documentacao

- [docs/uml/diagrama-classes.puml](docs/uml/diagrama-classes.puml) — diagrama de classes UML da camada Java (POO)
- [docs/uml/diagrama-sequencia.puml](docs/uml/diagrama-sequencia.puml) — diagrama de sequencia do laco de animacao
- [docs/motor-grafico.md](docs/motor-grafico.md) — documentacao tecnica do motor nativo (CG)

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
