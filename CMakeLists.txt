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

# ---------------------------------------------------------------------------
# Alvo opcional: biblioteca nativa JNI (motor.dll / libmotor.so)
# Integra o motor grafico com a aplicacao Java Swing (java/main).
# Requer um JDK instalado (JAVA_HOME). O contrato JNI e gerado por:
#   javac -encoding UTF-8 -h headers -d java/build java/main/MotorGrafico.java
# ---------------------------------------------------------------------------
find_package(JNI QUIET)
if (JNI_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/headers/main_MotorGrafico.h")
    add_library(motor SHARED
        src/jni/motor.cpp
        src/algorithms/DataGenerator.cpp
        src/algorithms/SortEngine.cpp
        src/algorithms/SortSimulator.cpp
        src/renderer/Shader.cpp
        src/renderer/SortRenderer.cpp
    )

    target_include_directories(motor
        PRIVATE
            include
            headers
            ${JNI_INCLUDE_DIRS}
    )

    target_link_libraries(motor
        PRIVATE
            glfw
            glm::glm
            OpenGL::GL
            algorithm_vision_glad
            ${CMAKE_DL_LIBS}
    )

    # System.loadLibrary("motor") procura por "motor.dll" no Windows,
    # sem o prefixo "lib" que o MinGW adiciona por padrao.
    if (WIN32)
        set_target_properties(motor PROPERTIES PREFIX "")
    endif()

    # Evita depender das DLLs de runtime do MinGW (libstdc++, libgcc,
    # winpthread) quando a JVM carregar a biblioteca.
    if (MINGW)
        target_link_options(motor PRIVATE -static)
    endif()

    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(motor PRIVATE -Wall -Wextra -Wpedantic)
    elseif (MSVC)
        target_compile_options(motor PRIVATE /W4)
    endif()

    add_custom_command(TARGET motor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:motor>/shaders"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${SHADER_SOURCE_DIR}"
                "$<TARGET_FILE_DIR:motor>/shaders"
        COMMENT "Copying shader assets next to the JNI library"
        VERBATIM
    )

    message(STATUS "Alvo JNI 'motor' habilitado (JDK: ${JAVA_INCLUDE_PATH})")
else()
    message(STATUS "Alvo JNI 'motor' desabilitado (JDK ou headers/main_MotorGrafico.h ausentes)")
endif()
