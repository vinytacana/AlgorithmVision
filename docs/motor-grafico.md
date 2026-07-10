# Documentacao Tecnica do Motor Grafico (CG)

Deliverable da equipe de Computacao Grafica no Projeto Interdisciplinar (POO + CG):
motor grafico encapsulado em biblioteca nativa (`motor.dll` no Windows,
`libmotor.so` no Linux), consumido pela aplicacao Java exclusivamente por meio
do contrato JNI.

## Visao geral

O motor segue o modelo de **biblioteca passiva**: nao cria janela visivel nem
possui loop de renderizacao proprio. A aplicacao Java e a dona do ciclo de
vida — controla o relogio da simulacao e pede quadros sob demanda. Cada quadro
e desenhado pelo pipeline OpenGL em um framebuffer *offscreen* e devolvido ao
Java como um vetor de pixels ARGB, exibido em um `JPanel` via `BufferedImage`.

```text
Java (EDT)                          C++ (mesma thread, contexto GL)
----------                          --------------------------------
init() ────────────────────────▶ glfwInit + janela OCULTA (so contexto)
                                  gladLoadGLLoader, Shader, SortSimulator
step() ────────────────────────▶ SortEngine::step() (um passo do algoritmo)
renderFrame(w, h, pixels) ─────▶ FBO(w x h) → glViewport → render
                                  glReadPixels (RGBA) → ARGB + flip ─▶ pixels
cleanup() ─────────────────────▶ destroi recursos GL, janela e GLFW
```

## Pipeline de renderizacao (OpenGL 3.3 Core)

1. **Contexto**: criado por GLFW com uma janela `GLFW_VISIBLE=GLFW_FALSE`
   (64x64) — a janela existe apenas para portar o contexto; nada e apresentado
   nela (nao ha `glfwSwapBuffers`).
2. **Framebuffer offscreen (FBO)**: `ensureFramebuffer(w, h)` mantem um FBO com
   um renderbuffer de cor `GL_RGBA8` no tamanho pedido pelo painel Java,
   recriado quando o painel e redimensionado.
3. **Geometria**: um unico quad unitario (`SortRenderer::setupMesh`, VAO+VBO,
   `GL_TRIANGLE_FAN`), instanciado logicamente por elemento do array via
   matriz `model` (translacao+escala para barras; rotacao para o modo circular).
4. **Shaders** (`shaders/bar.vs`, `shaders/bar.fs`): projecao ortografica
   `glm::ortho(0, w, 0, h)`; o fragment shader colore por `state`
   (0 normal, 1 comparacao, 2 escrita, 3 pivo, 4 sub-array, 5 concluido),
   derivado do `SortVisualState` do engine.
5. **Leitura**: `glReadPixels(GL_RGBA)` para um buffer de staging; o C++
   converte para ARGB (`0xAARRGGBB`) e inverte verticalmente (origem do OpenGL
   e o canto inferior esquerdo; a `BufferedImage` desenha de cima para baixo).
   A escrita no `int[]` Java usa `GetPrimitiveArrayCritical` (sem copia extra).

## Contrato JNI

Classe Java: `main.MotorGrafico` (unica ponte; padrao de nome
`Java_main_MotorGrafico_<metodo>` em `native/motor.cpp`).

| Metodo Java                          | Funcao nativa                                   | Papel |
| ------------------------------------ | ------------------------------------------------ | ----- |
| `init(): boolean`                    | `Java_main_MotorGrafico_init`                    | contexto GL oculto + shader + simulador |
| `cleanup()`                          | `Java_main_MotorGrafico_cleanup`                 | libera recursos GL/GLFW |
| `step()`                             | `Java_main_MotorGrafico_step`                    | um passo (se ordenando/finalizando) |
| `reset()`                            | `Java_main_MotorGrafico_reset`                   | regenera os dados |
| `toggleSorting()`                    | `Java_main_MotorGrafico_toggleSorting`           | inicia/pausa |
| `isSorting()/hasFinished()`          | `..._isSorting` / `..._hasFinished`              | estado |
| `setAlgorithm/Distribution/RenderMode/ArraySize` | `..._set*`                          | configuracao |
| `getComparisons()/getWrites()`       | `..._getComparisons` / `..._getWrites`           | metricas |
| `getAlgorithmCount/Name/Description` | `..._getAlgorithm*`                              | metadados do `AlgorithmRegistry` |
| `renderFrame(w, h, int[]): boolean`  | `Java_main_MotorGrafico_renderFrame`             | quadro offscreen → pixels ARGB |

## Contrato de threading

**Todas** as chamadas nativas devem ocorrer na mesma thread, pois o contexto
OpenGL pertence a thread que executou `init()`. A aplicacao Swing garante isso
fazendo tudo na *Event Dispatch Thread*: o `init()` roda dentro de
`SwingUtilities.invokeLater`, o laco de animacao e um `javax.swing.Timer`
(dispara na EDT) e o `cleanup()` acontece no `windowClosing`. Por consequencia
o motor dispensa mutexes e atomics.

## Portabilidade

- **Windows**: `motor.dll` (CMake remove o prefixo `lib`); linkada com
  `-static` no MinGW para nao depender das DLLs de runtime.
- **Linux**: `libmotor.so`; o GLAD estatico e compilado com
  `POSITION_INDEPENDENT_CODE ON` (obrigatorio para entrar em uma `.so`).
- **Wayland (Fedora)**: `run-integrado.sh` exporta `GLFW_PLATFORM=x11` por
  padrao, criando o contexto via XWayland (caminho mais testado).
- Os shaders sao carregados por caminho relativo (`shaders/`), por isso o
  diretorio de trabalho deve ser `build/` (o CMake copia os shaders para la).

## Componentes internos

- `SortEngine` (`src/algorithms/SortEngine.cpp`): maquinas de estado
  incrementais dos 13 algoritmos; um `step()` = uma comparacao/escrita.
- `SortRenderer` (`src/renderer/SortRenderer.cpp`): malha + desenho do estado.
- `SortSimulator`: fachada engine+renderer usada pelo motor JNI.
- `AlgorithmRegistry` (`include/AlgorithmRegistry.h`): fonte unica de verdade
  de nomes/descricoes — o Java os consulta via JNI, sem duplicacao.
- `EngineTests` (`tests/EngineTests.cpp`): valida a logica sem OpenGL.
- `main.TesteIntegracao` (Java): valida o caminho completo ate os pixels.
