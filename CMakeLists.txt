cmake_minimum_required(VERSION 3.10)
project(AlgorithmVision)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encontrar pacotes
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Diretórios de inclusão
include_directories(
    ${GLEW_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    include
    include/imgui
)

# Arquivos fonte do ImGui
set(IMGUI_SOURCES
    src/ui/imgui/imgui.cpp
    src/ui/imgui/imgui_draw.cpp
    src/ui/imgui/imgui_widgets.cpp
    src/ui/imgui/imgui_tables.cpp
    src/ui/imgui/imgui_demo.cpp
    src/ui/imgui/imgui_impl_glfw.cpp
    src/ui/imgui/imgui_impl_opengl3.cpp
)

# Arquivos fonte do projeto (MODULARIZADO)
set(SOURCES
    src/main.cpp
    src/renderer/Shader.cpp
    src/algorithms/SortSimulator.cpp
    src/algorithms/GraphSimulator.cpp
    src/ui/UIComponents.cpp
    ${IMGUI_SOURCES}
)

# Executável
add_executable(${PROJECT_NAME} ${SOURCES})

# Linkar bibliotecas
target_link_libraries(${PROJECT_NAME}
    ${GLEW_LIBRARIES}
    glfw
    ${OPENGL_LIBRARIES}
    ${CMAKE_DL_LIBS}
)
