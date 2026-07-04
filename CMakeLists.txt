cmake_minimum_required(VERSION 3.10)
if (POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()
project(AlgorithmVision LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
    src/algorithms/DataGenerator.cpp
    src/algorithms/SortEngine.cpp
    src/renderer/Shader.cpp
    src/renderer/SortRenderer.cpp
    src/algorithms/SortSimulator.cpp
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

set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_OUTPUT_DIR "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADER_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${SHADER_SOURCE_DIR}"
            "${SHADER_OUTPUT_DIR}"
    COMMENT "Copying shader assets to runtime directory"
    VERBATIM
)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glad/include/glad/glad.h")
    add_library(algorithm_vision_glad STATIC glad/src/glad.c)
    target_include_directories(algorithm_vision_glad PUBLIC glad/include)
    target_link_libraries(algorithm_vision_glad PUBLIC ${CMAKE_DL_LIBS})
    target_link_libraries(${PROJECT_NAME} PRIVATE algorithm_vision_glad)
else()
    message(FATAL_ERROR "GLAD e obrigatorio. Adicione os arquivos em glad/src/glad.c e glad/include/glad/glad.h.")
endif()

enable_testing()

add_executable(EngineTests
    tests/EngineTests.cpp
    src/algorithms/DataGenerator.cpp
    src/algorithms/SortEngine.cpp
)

target_include_directories(EngineTests
    PRIVATE
        include
)

target_link_libraries(EngineTests
    PRIVATE
        glm::glm
)

add_test(NAME EngineTests COMMAND EngineTests)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(EngineTests PRIVATE -Wall -Wextra -Wpedantic)
elseif (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    target_compile_options(EngineTests PRIVATE /W4)
endif()
