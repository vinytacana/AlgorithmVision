// Biblioteca nativa de renderizacao do AlgorithmVision.
//
// Implementa o contrato declarado em java/main/MotorGrafico.java seguindo o
// modelo de BIBLIOTECA PASSIVA: nenhuma janela visivel, nenhum loop proprio.
// A aplicacao Java e a dona do ciclo de vida - ela controla o tempo da
// simulacao (step) e pede quadros via renderFrame(), que desenha o estado
// atual em um framebuffer offscreen (FBO) e devolve os pixels em ARGB.
//
// Contrato de threading: TODAS as funcoes devem ser chamadas na MESMA thread,
// pois o contexto OpenGL pertence a thread que executou init(). Na aplicacao
// Swing isso e garantido rodando tudo na Event Dispatch Thread (EDT).

#include "main_MotorGrafico.h"

#include "GLLoader.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "AlgorithmRegistry.h"
#include "Shader.h"
#include "SortSimulator.h"
#include "SortTypes.h"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <memory>
#include <thread>
#include <vector>

namespace {

// Estado do motor. Ponteiros com destruicao manual em cleanup() porque a
// ordem importa: recursos OpenGL precisam morrer com o contexto ainda ativo.
GLFWwindow* g_window = nullptr; // janela OCULTA: existe so para ter contexto GL
std::unique_ptr<Shader> g_shader;
std::unique_ptr<SortSimulator> g_simulator;
std::thread::id g_ownerThread;

GLuint g_fbo = 0;
GLuint g_colorBuffer = 0;
int g_fboWidth = 0;
int g_fboHeight = 0;
std::vector<unsigned char> g_readBuffer; // pixels RGBA lidos da GPU

constexpr int kMaxFramebufferDimension = 7680;
constexpr std::int64_t kMaxFramebufferPixels = 7680LL * 4320LL;

void destroyFramebuffer() {
    if (g_colorBuffer != 0) glDeleteRenderbuffers(1, &g_colorBuffer);
    if (g_fbo != 0) glDeleteFramebuffers(1, &g_fbo);
    g_colorBuffer = 0;
    g_fbo = 0;
    g_fboWidth = 0;
    g_fboHeight = 0;
}

// Garante um FBO com anexo de cor RGBA8 no tamanho pedido (recria ao mudar).
bool ensureFramebuffer(int width, int height) {
    if (g_fbo != 0 && width == g_fboWidth && height == g_fboHeight) return true;
    destroyFramebuffer();

    glGenFramebuffers(1, &g_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, g_fbo);
    glGenRenderbuffers(1, &g_colorBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, g_colorBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, g_colorBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERRO::FBO::INCOMPLETO" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        destroyFramebuffer();
        return false;
    }

    g_fboWidth = width;
    g_fboHeight = height;
    return true;
}

bool engineReady() {
    return g_window != nullptr && g_shader && g_simulator;
}

bool validAlgorithmIndex(int index) {
    return index >= 0 && index < AlgorithmRegistry::count();
}

bool validateRenderRequest(jint width, jint height, jintArray pixelsOut, JNIEnv* env, jsize& needed) {
    if (width <= 0 || height <= 0 || pixelsOut == nullptr) return false;
    if (width > kMaxFramebufferDimension || height > kMaxFramebufferDimension) {
        std::cerr << "ERRO::JNI::FRAMEBUFFER_MUITO_GRANDE: " << width << "x" << height << std::endl;
        return false;
    }

    const std::int64_t pixelCount = static_cast<std::int64_t>(width) * static_cast<std::int64_t>(height);
    if (pixelCount > kMaxFramebufferPixels || pixelCount > std::numeric_limits<jsize>::max()) {
        std::cerr << "ERRO::JNI::FRAMEBUFFER_EXCEDE_LIMITE_SEGURO: " << width << "x" << height << std::endl;
        return false;
    }

    needed = static_cast<jsize>(pixelCount);
    return env->GetArrayLength(pixelsOut) >= needed;
}

bool calledFromOwnerThread() {
    return g_ownerThread == std::this_thread::get_id();
}

} // namespace

JNIEXPORT jboolean JNICALL Java_main_MotorGrafico_init(JNIEnv*, jobject) {
    const std::thread::id currentThread = std::this_thread::get_id();
    if (g_window != nullptr) {
        if (g_ownerThread != currentThread) {
            std::cerr << "ERRO::JNI::THREAD_INVALIDA_INIT" << std::endl;
            return JNI_FALSE;
        }
        return JNI_TRUE; // ja inicializado
    }

    if (!glfwInit()) {
        std::cerr << "ERRO::GLFW::INICIALIZACAO_FALHOU" << std::endl;
        return JNI_FALSE;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // sem janela visivel: so o contexto

    g_window = glfwCreateWindow(64, 64, "AlgorithmVision (contexto offscreen)", NULL, NULL);
    if (!g_window) {
        std::cerr << "ERRO::GLFW::CRIACAO_DE_CONTEXTO_FALHOU" << std::endl;
        glfwTerminate();
        return JNI_FALSE;
    }
    glfwMakeContextCurrent(g_window);

    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
        std::cerr << "ERRO::GLAD::INICIALIZACAO_FALHOU" << std::endl;
        glfwDestroyWindow(g_window);
        g_window = nullptr;
        glfwTerminate();
        return JNI_FALSE;
    }

    try {
        g_shader = std::make_unique<Shader>("shaders/bar.vs", "shaders/bar.fs");
        g_simulator = std::make_unique<SortSimulator>();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        g_simulator.reset();
        g_shader.reset();
        glfwDestroyWindow(g_window);
        g_window = nullptr;
        glfwTerminate();
        return JNI_FALSE;
    }

    g_ownerThread = currentThread;
    return JNI_TRUE;
}

JNIEXPORT void JNICALL Java_main_MotorGrafico_cleanup(JNIEnv*, jobject) {
    if (g_window == nullptr) return;

    // Destroi recursos OpenGL com o contexto ainda corrente.
    g_simulator.reset();
    g_shader.reset();
    destroyFramebuffer();
    g_readBuffer.clear();
    g_readBuffer.shrink_to_fit();

    glfwDestroyWindow(g_window);
    g_window = nullptr;
    g_ownerThread = std::thread::id{};
    glfwTerminate();
}

JNIEXPORT void JNICALL Java_main_MotorGrafico_step(JNIEnv*, jobject) {
    if (!engineReady()) return;
    if (g_simulator->isSortingActive() || g_simulator->isFinishingAnimation()) {
        g_simulator->step();
    }
}

JNIEXPORT void JNICALL Java_main_MotorGrafico_reset(JNIEnv*, jobject) {
    if (engineReady()) g_simulator->reset();
}

JNIEXPORT void JNICALL Java_main_MotorGrafico_toggleSorting(JNIEnv*, jobject) {
    if (engineReady()) g_simulator->toggleSorting();
}

JNIEXPORT jboolean JNICALL Java_main_MotorGrafico_isSorting(JNIEnv*, jobject) {
    return engineReady() && g_simulator->isSortingActive() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_main_MotorGrafico_hasFinished(JNIEnv*, jobject) {
    return engineReady() && g_simulator->hasFinished() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL Java_main_MotorGrafico_setAlgorithm(JNIEnv*, jobject, jint index) {
    if (engineReady() && validAlgorithmIndex(index)) {
        g_simulator->setAlgorithm(static_cast<Algorithm>(index));
    }
}

JNIEXPORT void JNICALL Java_main_MotorGrafico_setDistribution(JNIEnv*, jobject, jint index) {
    if (engineReady() && index >= 0 && index < DISTRIBUTION_COUNT) {
        g_simulator->setDistribution(static_cast<ArrayDistribution>(index));
    }
}

JNIEXPORT void JNICALL Java_main_MotorGrafico_setRenderMode(JNIEnv*, jobject, jint index) {
    if (engineReady() && index >= 0 && index < RENDER_MODE_COUNT) {
        g_simulator->setRenderMode(static_cast<RenderMode>(index));
    }
}

JNIEXPORT void JNICALL Java_main_MotorGrafico_setArraySize(JNIEnv*, jobject, jint n) {
    if (engineReady() && n > 0 && n <= MAX_ARRAY_SIZE) g_simulator->setArraySize(n);
}

JNIEXPORT jlong JNICALL Java_main_MotorGrafico_getComparisons(JNIEnv*, jobject) {
    return engineReady() ? static_cast<jlong>(g_simulator->getComparisons()) : 0;
}

JNIEXPORT jlong JNICALL Java_main_MotorGrafico_getWrites(JNIEnv*, jobject) {
    return engineReady() ? static_cast<jlong>(g_simulator->getWrites()) : 0;
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

JNIEXPORT jboolean JNICALL Java_main_MotorGrafico_renderFrame(
        JNIEnv* env, jobject, jint width, jint height, jintArray pixelsOut) {
    if (!engineReady()) return JNI_FALSE;
    if (!calledFromOwnerThread()) {
        std::cerr << "ERRO::JNI::THREAD_INVALIDA_RENDERFRAME" << std::endl;
        return JNI_FALSE;
    }

    jsize needed = 0;
    if (!validateRenderRequest(width, height, pixelsOut, env, needed)) return JNI_FALSE;
    if (!ensureFramebuffer(width, height)) return JNI_FALSE;

    // 1. Desenha o estado atual no framebuffer offscreen.
    glBindFramebuffer(GL_FRAMEBUFFER, g_fbo);
    glViewport(0, 0, width, height);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    g_simulator->render(*g_shader, width, height);

    // 2. Le os pixels de volta (RGBA, origem no canto inferior esquerdo).
    g_readBuffer.resize(static_cast<std::size_t>(needed) * 4);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, g_readBuffer.data());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 3. Converte RGBA -> ARGB e inverte verticalmente (Java desenha de cima
    //    para baixo; OpenGL le de baixo para cima).
    jint* dest = static_cast<jint*>(env->GetPrimitiveArrayCritical(pixelsOut, nullptr));
    if (dest == nullptr) return JNI_FALSE;
    for (int y = 0; y < height; ++y) {
        const unsigned char* src =
                g_readBuffer.data() + static_cast<std::size_t>(height - 1 - y) * width * 4;
        jint* row = dest + static_cast<std::size_t>(y) * width;
        for (int x = 0; x < width; ++x) {
            const unsigned int r = src[0];
            const unsigned int gCh = src[1];
            const unsigned int b = src[2];
            row[x] = static_cast<jint>(0xFF000000u | (r << 16) | (gCh << 8) | b);
            src += 4;
        }
    }
    env->ReleasePrimitiveArrayCritical(pixelsOut, dest, 0);
    return JNI_TRUE;
}
