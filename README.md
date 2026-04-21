# AlgorithmVision

An interactive, real-time algorithm visualizer built with C++ and OpenGL. Explore sorting and graph algorithms with dynamic, shader-powered visualizations and integrated UI controls.

## 🚀 Key Features

*   **Sorting Visualizer:** 
    *   **Algorithms:** Bubble Sort, Quick Sort, and Merge Sort.
    *   **Visual Modes:** Supports **Bar charts**, **Dot plots**, and **Circular** layouts.
    *   **Data Control:** Generate arrays with Random, Reversed, Nearly Sorted, or Few Unique distributions.
*   **Graph Visualizer:**
    *   **Algorithms:** Breadth-First Search (BFS), Depth-First Search (DFS), and Dijkstra’s Algorithm.
    *   **Interactive Builder:** Add, remove, and drag nodes in real-time. Connect nodes to create weighted edges dynamically.
*   **Intuitive UI:** Integrated control panel using **Dear ImGui** to adjust simulation speed, step through iterations, or reset the state.
*   **Custom Shader Pipeline:** Uses GLSL vertex and fragment shaders for high-quality rendering and visual highlights of algorithm states (comparisons, visits, paths).

## 🛠️ Tech Stack

*   **Core:** C++17
*   **Graphics:** OpenGL 3.3 (Core Profile)
*   **UI:** Dear ImGui
*   **Windowing & Input:** GLFW
*   **Mathematics:** GLM
*   **Extensions:** GLAD (optional) or GLEW

## 🏗️ Build Instructions

To build the project, you need to have a C++ compiler, CMake, and the following dependencies installed:
- GLFW
- GLM
- OpenGL loader:
  - GLEW, or
  - GLAD via `glad::glad` package or vendored files in `external/glad/`

### Linux
```bash
mkdir build
cd build
cmake ..
make
./AlgorithmVision
```

To prefer GLAD when available:
```bash
cmake -DALGORITHM_VISION_USE_GLAD=ON ..
make
```

## Notes

The repository currently does not include a `LICENSE` file.
