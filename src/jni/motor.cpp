// Camada de integracao JNI do AlgorithmVision.
//
// Implementa o contrato declarado em java/main/MotorGrafico.java. A thread
// Java que chama init() torna-se a thread de renderizacao (dona do contexto
// OpenGL). Chamadas vindas de outras threads (ex.: EDT do Swing) NUNCA tocam
// o SortSimulator diretamente: elas enfileiram comandos protegidos por mutex,
// que o loop de render drena a cada frame. Metricas sao expostas por
// snapshots atomicos atualizados pelo proprio loop.

#include "main_MotorGrafico.h"

#include "GLLoader.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "AlgorithmRegistry.h"
#include "Shader.h"
#include "SortSimulator.h"

#include <algorithm>
#include <atomic>
#include <functional>
#include <iostream>
#include <mutex>
#include <utility>
#include <vector>

namespace {

using Command = std::function<void(SortSimulator&)>;

std::mutex g_commandMutex;
std::vector<Command> g_commands;

std::atomic<bool> g_running{false};
std::atomic<bool> g_stopRequested{false};
std::atomic<float> g_tickRate{0.03f};

// Snapshots atualizados pela thread de render, lidos pelo Swing.
std::atomic<long long> g_comparisons{0};
std::atomic<long long> g_writes{0};
std::atomic<bool> g_isSorting{false};
std::atomic<bool> g_hasFinished{false};

void enqueue(Command command) {
    std::lock_guard<std::mutex> lock(g_commandMutex);
    g_commands.push_back(std::move(command));
}

void drainCommands(SortSimulator& simulator) {
    std::vector<Command> pending;
    {
        std::lock_guard<std::mutex> lock(g_commandMutex);
        pending.swap(g_commands);
    }
    for (Command& command : pending) command(simulator);
}

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

bool validAlgorithmIndex(int index) {
    return index >= 0 && index < AlgorithmRegistry::count();
}

} // namespace

JNIEXPORT void JNICALL Java_main_MotorGrafico_init(JNIEnv*, jobject) {
    if (g_running.exchange(true)) return; // ja existe um loop ativo
    g_stopRequested = false;

    if (!glfwInit()) {
        std::cerr << "ERRO::GLFW::INICIALIZACAO_FALHOU" << std::endl;
        g_running = false;
        return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 800, "AlgorithmVision - Motor Grafico (JNI)", NULL, NULL);
    if (!window) {
        std::cerr << "ERRO::GLFW::CRIACAO_DE_JANELA_FALHOU" << std::endl;
        glfwTerminate();
        g_running = false;
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
        std::cerr << "ERRO::GLAD::INICIALIZACAO_FALHOU" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        g_running = false;
        return;
    }

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    try {
        Shader barShader("shaders/bar.vs", "shaders/bar.fs");
        SortSimulator simulator;
        double lastSortTime = glfwGetTime();

        while (!glfwWindowShouldClose(window) && !g_stopRequested) {
            glfwPollEvents();
            drainCommands(simulator);

            const double currentTime = glfwGetTime();
            if (currentTime - lastSortTime >= g_tickRate.load()) {
                if (simulator.isSortingActive() || simulator.isFinishingAnimation()) simulator.step();
                lastSortTime = currentTime;
            }

            g_comparisons = simulator.getComparisons();
            g_writes = simulator.getWrites();
            g_isSorting = simulator.isSortingActive();
            g_hasFinished = simulator.hasFinished();

            glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            simulator.render(barShader, framebufferWidth, framebufferHeight);
            glfwSwapBuffers(window);
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    // Shader/SortRenderer ja foram destruidos ao sair do escopo do try,
    // com o contexto OpenGL ainda corrente.
    glfwDestroyWindow(window);
    glfwTerminate();
    g_running = false;
}

JNIEXPORT void JNICALL Java_main_MotorGrafico_cleanup(JNIEnv*, jobject) {
    g_stopRequested = true;
}

JNIEXPORT jboolean JNICALL Java_main_MotorGrafico_isRunning(JNIEnv*, jobject) {
    return g_running.load() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL Java_main_MotorGrafico_setAlgorithm(JNIEnv*, jobject, jint index) {
    if (!validAlgorithmIndex(index)) return;
    enqueue([index](SortSimulator& simulator) {
        simulator.setAlgorithm(static_cast<Algorithm>(index));
    });
}

JNIEXPORT void JNICALL Java_main_MotorGrafico_setDistribution(JNIEnv*, jobject, jint index) {
    if (index < 0 || index >= DISTRIBUTION_COUNT) return;
    enqueue([index](SortSimulator& simulator) {
        simulator.setDistribution(static_cast<ArrayDistribution>(index));
    });
}

JNIEXPORT void JNICALL Java_main_MotorGrafico_setRenderMode(JNIEnv*, jobject, jint index) {
    if (index < 0 || index >= RENDER_MODE_COUNT) return;
    enqueue([index](SortSimulator& simulator) {
        simulator.setRenderMode(static_cast<RenderMode>(index));
    });
}

JNIEXPORT void JNICALL Java_main_MotorGrafico_setArraySize(JNIEnv*, jobject, jint n) {
    if (n <= 0) return;
    enqueue([n](SortSimulator& simulator) {
        simulator.setArraySize(n);
    });
}

JNIEXPORT void JNICALL Java_main_MotorGrafico_setTickRate(JNIEnv*, jobject, jfloat secondsPerStep) {
    g_tickRate = std::min(1.0f, std::max(0.001f, static_cast<float>(secondsPerStep)));
}

JNIEXPORT void JNICALL Java_main_MotorGrafico_toggleSorting(JNIEnv*, jobject) {
    enqueue([](SortSimulator& simulator) {
        simulator.toggleSorting();
    });
}

JNIEXPORT void JNICALL Java_main_MotorGrafico_reset(JNIEnv*, jobject) {
    enqueue([](SortSimulator& simulator) {
        simulator.reset();
    });
}

JNIEXPORT jlong JNICALL Java_main_MotorGrafico_getComparisons(JNIEnv*, jobject) {
    return static_cast<jlong>(g_comparisons.load());
}

JNIEXPORT jlong JNICALL Java_main_MotorGrafico_getWrites(JNIEnv*, jobject) {
    return static_cast<jlong>(g_writes.load());
}

JNIEXPORT jboolean JNICALL Java_main_MotorGrafico_isSorting(JNIEnv*, jobject) {
    return g_isSorting.load() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_main_MotorGrafico_hasFinished(JNIEnv*, jobject) {
    return g_hasFinished.load() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jint JNICALL Java_main_MotorGrafico_getAlgorithmCount(JNIEnv*, jobject) {
    return AlgorithmRegistry::count();
}

JNIEXPORT jstring JNICALL Java_main_MotorGrafico_getAlgorithmName(JNIEnv* env, jobject, jint index) {
    if (!validAlgorithmIndex(index)) return env->NewStringUTF("");
    return env->NewStringUTF(AlgorithmRegistry::getMetadataByIndex(index).name);
}

JNIEXPORT jstring JNICALL Java_main_MotorGrafico_getAlgorithmDescription(JNIEnv* env, jobject, jint index) {
    if (!validAlgorithmIndex(index)) return env->NewStringUTF("");
    return env->NewStringUTF(AlgorithmRegistry::getMetadataByIndex(index).description);
}
