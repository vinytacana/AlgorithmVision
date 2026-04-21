cmake_minimum_required(VERSION 3.10)
if (POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()
project(AlgorithmVision LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ALGORITHM_VISION_USE_GLAD "Use GLAD instead of GLEW when available" OFF)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

set(IMGUI_SOURCES
    src/ui/imgui/imgui.cpp
    src/ui/imgui/imgui_draw.cpp
    src/ui/imgui/imgui_widgets.cpp
    src/ui/imgui/imgui_tables.cpp
    src/ui/imgui/imgui_impl_glfw.cpp
    src/ui/imgui/imgui_impl_opengl3.cpp
)

set(SOURCES
    src/main.cpp
    src/renderer/Shader.cpp
    src/algorithms/SortSimulator.cpp
    src/algorithms/GraphSimulator.cpp
    src/ui/UIComponents.cpp
    ${IMGUI_SOURCES}
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        include
        include/imgui
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glfw
        glm::glm
        OpenGL::GL
        ${CMAKE_DL_LIBS}
)

if (ALGORITHM_VISION_USE_GLAD)
    find_package(glad CONFIG QUIET)

    if (TARGET glad::glad)
        target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)
    elseif (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/src/gl.c" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include/glad/gl.h")
        add_library(algorithm_vision_glad STATIC external/glad/src/gl.c)
        target_include_directories(algorithm_vision_glad PUBLIC external/glad/include)
        target_link_libraries(algorithm_vision_glad PUBLIC ${CMAKE_DL_LIBS})
        target_link_libraries(${PROJECT_NAME} PRIVATE algorithm_vision_glad)
    else()
        message(FATAL_ERROR "ALGORITHM_VISION_USE_GLAD=ON, mas GLAD nao foi encontrado. Instale um pacote CMake glad::glad ou adicione os arquivos em external/glad/.")
    endif()
else()
    find_package(GLEW REQUIRED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${GLEW_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${GLEW_LIBRARIES})
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
elseif (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
endif()
