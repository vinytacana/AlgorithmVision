@echo off
REM Executa o AlgorithmVision (aplicacao Java Swing consumindo o motor JNI).
REM
REM Pre-requisitos (ver README.md):
REM   cmake -S . -B build
REM   cmake --build build
REM
REM O diretorio de trabalho precisa ser lib\ para que o motor encontre
REM a pasta shaders\ copiada pelo CMake.

cd /d "%~dp0lib"
java -Djava.library.path=. -cp ..\build main.Main
