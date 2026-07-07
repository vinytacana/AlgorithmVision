#!/usr/bin/env bash
# =============================================================================
# Compilacao manual do AlgorithmVision com g++ (sem CMake), para Git Bash.
#
# Uso:
#   ./compilar.sh            # compila tudo (testes + biblioteca JNI)
#   ./compilar.sh tests      # so os testes headless (nao precisa de OpenGL)
#   ./compilar.sh jni        # classes Java + motor.dll
#   ./compilar.sh clean      # remove os artefatos gerados por este script
#
# Pre-requisitos:
#   - MinGW g++ no PATH (WinLibs)
#   - GLFW/GLM instalados em deps/install (ver README)
#   - JDK 17+ no PATH (apenas para o alvo jni)
# =============================================================================
set -euo pipefail

# Diretorio do proprio script (raiz do projeto)
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"

# --- Caminhos das dependencias (ajuste se necessario) ------------------------
DEPS="${DEPS:-$ROOT/../deps/install}"
if [[ -z "${JDK:-}" ]]; then
    if [[ -n "${JAVA_HOME:-}" ]]; then
        # Converte C:\... para /c/... se vier no formato Windows
        JDK="$(cygpath -u "$JAVA_HOME" 2>/dev/null || echo "$JAVA_HOME")"
    else
        JDK="/c/Program Files/Eclipse Adoptium/jdk-17.0.19.10-hotspot"
    fi
fi

# --- Fontes da biblioteca nativa ---------------------------------------------
CORE_SOURCES=(
    src/algorithms/DataGenerator.cpp
    src/algorithms/SortEngine.cpp
    src/algorithms/SortSimulator.cpp
    src/renderer/Shader.cpp
    src/renderer/SortRenderer.cpp
)

JAVA_SOURCES=(
    java/main/*.java
    java/main/controle/*.java
    java/main/ui/*.java
)

CXXFLAGS=(-std=c++17 -O2 -Wall -Wextra)
INCLUDES=(-Iinclude -Iglad/include -I"$DEPS/include")
# Ordem importa no link: bibliotecas DEPOIS dos objetos/fontes.
LIBS=(-L"$DEPS/lib" -lglfw3 -lopengl32 -lgdi32)

build_glad() {
    if [[ ! -f glad.o || glad/src/glad.c -nt glad.o ]]; then
        echo ">> Compilando GLAD..."
        gcc -c glad/src/glad.c -Iglad/include -O2 -o glad.o
    fi
}

build_tests() {
    echo ">> Compilando EngineTests (headless, sem OpenGL)..."
    g++ "${CXXFLAGS[@]}" -Iinclude \
        tests/EngineTests.cpp \
        src/algorithms/SortEngine.cpp \
        src/algorithms/DataGenerator.cpp \
        -o EngineTests.exe
    echo ">> Executando testes..."
    ./EngineTests.exe && echo ">> TESTES PASSARAM"
}

build_jni() {
    build_glad
    echo ">> Compilando classes Java e gerando header JNI..."
    javac -encoding UTF-8 -h headers -d java/build "${JAVA_SOURCES[@]}"

    echo ">> Compilando motor.dll (biblioteca nativa JNI)..."
    # -shared: gera DLL; -static: embute o runtime do MinGW (libstdc++/libgcc/
    # winpthread), para a JVM carregar a DLL sem depender do PATH.
    g++ -shared -static "${CXXFLAGS[@]}" "${INCLUDES[@]}" -Iheaders \
        -I"$JDK/include" -I"$JDK/include/win32" \
        src/jni/motor.cpp \
        "${CORE_SOURCES[@]}" \
        glad.o \
        "${LIBS[@]}" \
        -o motor.dll
    echo ">> OK. Para executar (CWD precisa ter shaders/ e motor.dll):"
    echo "   java -Djava.library.path=. -cp java/build main.Main"
    echo "   java -Djava.library.path=. -cp java/build main.TesteIntegracao"
}

clean() {
    echo ">> Limpando artefatos do build manual..."
    rm -f glad.o EngineTests.exe motor.dll
    rm -rf java/build
    echo ">> OK"
}

case "${1:-all}" in
    tests) build_tests ;;
    jni)   build_jni ;;
    clean) clean ;;
    all)   build_tests; build_jni ;;
    *)     echo "Uso: $0 [tests|jni|clean|all]"; exit 1 ;;
esac
