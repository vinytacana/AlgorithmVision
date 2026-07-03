# AlgorithmVision

An interactive, real-time sorting algorithm visualizer built with C++ and OpenGL. Explore sorting strategies with dynamic, shader-powered visualizations and integrated UI controls.

## 🚀 Key Features

*   **Sorting Visualizer:**
    *   **Algorithms:** Bubble Sort, Quick Sort, Merge Sort, Insertion Sort, Selection Sort, Shell Sort, Heap Sort, K-Way Merge Sort, Miracle Sort, Sleepsort, and Bogosort.
    *   **Visual Modes:** Supports **Bar charts**, **Dot plots**, and **Circular** layouts.
    *   **Data Control:** Generate arrays with Random, Reversed, Nearly Sorted, or Few Unique distributions.
*   **Intuitive UI:** Integrated control panel using **Dear ImGui** to choose algorithms, tune input data, adjust animation speed, and reset the simulation.
*   **Custom Shader Pipeline:** Uses GLSL vertex and fragment shaders for visual highlights of algorithm states such as comparisons, writes, pivots, ranges, and completion.

## 🛠️ Tech Stack

*   **Core:** C++17
*   **Graphics:** OpenGL 3.3 (Core Profile)
*   **UI:** Dear ImGui
*   **Windowing & Input:** GLFW
*   **Mathematics:** GLM
*   **Extensions:** Vendored GLAD

## 🏗️ Build Instructions

To build the project, you need to have a C++ compiler, CMake, and the following dependencies installed:
- GLFW
- GLM
- Vendored GLAD files in `glad/src/glad.c` and `glad/include/glad/glad.h`

### Linux
```bash
mkdir build
cd build
cmake ..
make
./AlgorithmVision
```

The build copies the `shaders/` directory next to the executable, so running `./AlgorithmVision` from `build/` can find `shaders/bar.vs` and `shaders/bar.fs`.

To run the headless simulator tests:
```bash
ctest --output-on-failure
```

## Notes

The repository currently does not include a `LICENSE` file.
