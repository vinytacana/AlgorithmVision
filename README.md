# AlgorithmVision

Visualizador interativo de algoritmos de ordenacao feito para o projeto interdisciplinar de POO + Computacao Grafica.

A interface e uma aplicacao Java Swing. A renderizacao fica em uma biblioteca nativa C++/OpenGL acessada por JNI. O Java controla a janela, os botoes e o tempo da simulacao; o C++ desenha cada quadro em um framebuffer offscreen e devolve os pixels para a tela Swing.

## O Que O Projeto Faz

- Mostra algoritmos de ordenacao passo a passo.
- Permite trocar algoritmo, distribuicao dos dados, tamanho do vetor, velocidade e modo de visualizacao.
- Usa uma unica janela Swing com controles laterais e area de renderizacao.
- Renderiza com OpenGL em uma biblioteca nativa carregada pelo Java.
- Inclui testes da logica de ordenacao e um teste de integracao Java -> JNI -> OpenGL.

## Algoritmos

Tradicionais e didaticos:

- Bubble Sort
- Quick Sort
- Merge Sort
- Insertion Sort
- Selection Sort
- Shell Sort
- Heap Sort
- K-Way Merge Sort

Experimentais:

- Miracle Sort
- Sleepsort
- Bogosort
- Stalin Sort
- Thanos Sort

## Estrutura Do Projeto

```text
.
├── src/main/                 # Codigo Java da aplicacao Swing
│   ├── Main.java             # Entrada da aplicacao
│   ├── MotorGrafico.java     # Contrato JNI
│   ├── controle/             # Controladores da simulacao
│   └── ui/                   # Janela, painel de controles e painel de renderizacao
├── native/motor.cpp          # Implementacao JNI e contexto OpenGL offscreen
├── headers/                  # Header JNI
├── src/algorithms/           # Logica dos algoritmos de ordenacao
├── src/renderer/             # Renderizacao OpenGL dos dados
├── include/                  # Headers C++
├── shaders/                  # Shaders OpenGL
├── tests/                    # Testes C++ headless
├── build/                    # Gerado pelo CMake: classes Java e binarios de teste
└── lib/                      # Gerado pelo CMake: libmotor.so/motor.dll e shaders
```

## Arquitetura

```text
Java Swing (POO)
  Main
  JanelaPrincipal
  PainelControles
  PainelRenderizacao
  ControladorSimulacao
  ControladorMotorNativo
  MotorGrafico
        |
        | JNI
        v
C++ / OpenGL (CG)
  native/motor.cpp
  SortSimulator
  SortEngine
  SortRenderer
  Shader
```

Principios usados na camada Java:

- **Abstracao**: a interface grafica depende de `ControladorSimulacao`, nao diretamente do motor nativo.
- **Polimorfismo**: `PainelRenderizacao` sobrescreve `paintComponent`, e a UI conversa com o controlador pela interface.
- **Heranca**: a janela estende `JFrame` e os paineis estendem `JPanel`.
- **Encapsulamento**: `MotorGrafico` e o unico ponto que conhece JNI.

Contrato importante: todas as chamadas nativas devem acontecer na mesma thread que chamou `init()`. Na aplicacao, isso fica concentrado na EDT do Swing.

## Dependencias

- CMake
- Compilador C++ com suporte a C++17
- JDK 17 ou superior
- GLFW
- GLM
- OpenGL/Mesa
- GLAD ja esta incluido no repositorio em `glad/`

No Fedora:

```bash
sudo dnf install gcc-c++ cmake glfw-devel glm-devel java-17-openjdk-devel mesa-libGL-devel
```

## Compilar

```bash
cmake -S . -B build
cmake --build build
```

O build gera:

- classes Java em `build/main/`
- biblioteca nativa em `lib/libmotor.so` no Linux ou `lib/motor.dll` no Windows
- copia de `shaders/` para `lib/shaders/`

## Executar

Linux:

```bash
./run-integrado.sh
```

Windows:

```bat
run-integrado.bat
```

Execucao manual:

```bash
cd lib
java -Djava.library.path=. -cp ../build main.Main
```

Em sessoes Wayland, o script Linux usa `GLFW_PLATFORM=x11` por padrao para criar o contexto via XWayland. Para testar o backend Wayland nativo:

```bash
GLFW_PLATFORM=wayland ./run-integrado.sh
```

## Testes

Rodar todos os testes registrados no CMake:

```bash
ctest --test-dir build --output-on-failure
```

Testes existentes:

- `EngineTests`: valida a logica dos algoritmos sem depender de OpenGL.
- `JavaIntegration`: valida o caminho Java -> JNI -> OpenGL -> pixels.

Observacao: `JavaIntegration` precisa que a maquina consiga criar um contexto GLFW. Em terminal remoto ou ambiente sem servidor grafico, esse teste pode falhar mesmo com a logica C++ funcionando.

## Documentacao Complementar

- [docs/motor-grafico.md](docs/motor-grafico.md)
- [docs/uml/diagrama-classes.puml](docs/uml/diagrama-classes.puml)
- [docs/uml/diagrama-sequencia.puml](docs/uml/diagrama-sequencia.puml)

## Observacoes

- O projeto ainda nao possui arquivo `LICENSE`.
- O diretorio `build/` e o diretorio `lib/` sao gerados e nao devem ser versionados.
