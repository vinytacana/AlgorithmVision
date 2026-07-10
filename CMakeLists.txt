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
find_package(Java COMPONENTS Runtime Development QUIET)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glad/include/glad/glad.h")
    add_library(algorithm_vision_glad STATIC glad/src/glad.c)
    target_include_directories(algorithm_vision_glad PUBLIC glad/include)
    target_link_libraries(algorithm_vision_glad PUBLIC ${CMAKE_DL_LIBS})
    # Obrigatorio no Linux: esta lib estatica e linkada dentro da biblioteca
    # compartilhada 'motor' (.so), o que exige codigo independente de posicao.
    set_target_properties(algorithm_vision_glad PROPERTIES POSITION_INDEPENDENT_CODE ON)
else()
    message(FATAL_ERROR "GLAD e obrigatorio. Adicione os arquivos em glad/src/glad.c e glad/include/glad/glad.h.")
endif()

set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")

# ---------------------------------------------------------------------------
# Artefato principal: biblioteca nativa JNI (motor.dll / libmotor.so)
#
# O motor e uma biblioteca PASSIVA de renderizacao offscreen consumida pela
# aplicacao Java Swing (src/main). Requer um JDK instalado (JAVA_HOME).
# O contrato JNI e gerado por:
#   javac -encoding UTF-8 -h headers -d build \
#         src/main/*.java src/main/controle/*.java src/main/ui/*.java
# ---------------------------------------------------------------------------
find_package(JNI QUIET)
if (JNI_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/headers/main_MotorGrafico.h")
    if (Java_JAVAC_EXECUTABLE)
        set(JAVA_CLASS_DIR "${CMAKE_CURRENT_BINARY_DIR}")
        set(GENERATED_JNI_HEADER_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/headers")
        set(GENERATED_JNI_HEADER "${GENERATED_JNI_HEADER_DIR}/main_MotorGrafico.h")
        set(JAVA_SOURCES
            src/main/Main.java
            src/main/MotorGrafico.java
            src/main/TesteIntegracao.java
            src/main/controle/ControladorMotorNativo.java
            src/main/controle/ControladorSimulacao.java
            src/main/ui/JanelaPrincipal.java
            src/main/ui/PainelControles.java
            src/main/ui/PainelRenderizacao.java
        )

        add_custom_command(
            OUTPUT "${GENERATED_JNI_HEADER}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${JAVA_CLASS_DIR}" "${GENERATED_JNI_HEADER_DIR}"
            COMMAND "${Java_JAVAC_EXECUTABLE}" -encoding UTF-8
                    -h "${GENERATED_JNI_HEADER_DIR}"
                    -d "${JAVA_CLASS_DIR}"
                    ${JAVA_SOURCES}
            DEPENDS ${JAVA_SOURCES}
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            COMMENT "Compiling Java classes and generating JNI header"
            VERBATIM
        )

        add_custom_target(java_classes ALL DEPENDS "${GENERATED_JNI_HEADER}")
    endif()

    add_library(motor SHARED
        native/motor.cpp
        src/algorithms/DataGenerator.cpp
        src/algorithms/SortEngine.cpp
        src/algorithms/SortSimulator.cpp
        src/renderer/Shader.cpp
        src/renderer/SortRenderer.cpp
    )

    target_include_directories(motor
        PRIVATE
            include
            ${GENERATED_JNI_HEADER_DIR}
            headers
            ${JNI_INCLUDE_DIRS}
    )

    if (TARGET java_classes)
        add_dependencies(motor java_classes)
    endif()

    set_target_properties(motor PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
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
    message(WARNING "Alvo JNI 'motor' desabilitado: instale um JDK e gere headers/main_MotorGrafico.h com javac -h.")
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

if (TARGET motor AND Java_JAVA_EXECUTABLE AND TARGET java_classes)
    add_test(
        NAME JavaIntegration
        COMMAND "${Java_JAVA_EXECUTABLE}"
                --enable-native-access=ALL-UNNAMED
                -Djava.library.path=$<TARGET_FILE_DIR:motor>
                -cp "${JAVA_CLASS_DIR}"
                main.TesteIntegracao
    )
    set_tests_properties(JavaIntegration PROPERTIES
        WORKING_DIRECTORY "$<TARGET_FILE_DIR:motor>"
        ENVIRONMENT "GLFW_PLATFORM=x11"
    )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(EngineTests PRIVATE -Wall -Wextra -Wpedantic)
elseif (MSVC)
    target_compile_options(EngineTests PRIVATE /W4)
endif()
