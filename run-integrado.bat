@echo off
REM Executa o AlgorithmVision (aplicacao Java Swing consumindo o motor JNI).
REM
REM Pre-requisitos (ver README.md):
REM   1. javac -encoding UTF-8 -h headers -d java\build java\main\*.java java\main\controle\*.java java\main\ui\*.java
REM   2. cmake --build build --target motor
REM
REM O diretorio de trabalho precisa ser build\ para que o motor encontre
REM a pasta shaders\ copiada pelo CMake.

cd /d "%~dp0build"
java -Djava.library.path=. -cp ..\java\build main.Main
