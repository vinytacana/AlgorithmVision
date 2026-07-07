#!/usr/bin/env bash
# Executa o AlgorithmVision (aplicacao Java Swing + motor nativo JNI) no Linux.
#
# Pre-requisitos (ver README.md, secao "Fedora / Linux"):
#   1. javac -encoding UTF-8 -h headers -d java/build \
#          java/main/*.java java/main/controle/*.java java/main/ui/*.java
#   2. cmake -S . -B build && cmake --build build --target motor
#
# O diretorio de trabalho precisa ser build/ para que o motor encontre a
# pasta shaders/ copiada pelo CMake (e a libmotor.so gerada la).
#
# Em sessoes Wayland (padrao do Fedora), forcamos o backend X11 do GLFW
# (via XWayland), que e o caminho mais testado para contexto offscreen.
# Exporte GLFW_PLATFORM=wayland antes de rodar se quiser o backend nativo.
cd "$(dirname "$0")/build" || exit 1
export GLFW_PLATFORM="${GLFW_PLATFORM:-x11}"
exec java -Djava.library.path=. -cp ../java/build main.Main
