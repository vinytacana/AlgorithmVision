@echo off
REM Executa a versao integrada Java Swing + OpenGL (JNI) do AlgorithmVision.
REM
REM Pre-requisitos (ver README.md, secao "Integracao Java (JNI)"):
REM   1. javac -encoding UTF-8 -h headers -d java\build java\main\*.java
REM   2. cmake --build build --target motor
REM
REM O diretorio de trabalho precisa ser build\ para que o motor encontre
REM a pasta shaders\ copiada pelo CMake.

cd /d "%~dp0build"
java -Djava.library.path=. -cp ..\java\build main.Main
