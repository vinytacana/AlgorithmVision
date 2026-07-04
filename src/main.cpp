#include "GLLoader.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "AlgorithmRegistry.h"
#include "Shader.h"
#include "SortSimulator.h"
#include "UIComponents.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>

namespace {

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

} // namespace

int main() {
    if (!glfwInit()) {
        std::cerr << "ERRO::GLFW::INICIALIZACAO_FALHOU" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __linux__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(1280, 800, "AlgorithmVision Pro - Modular Edition", NULL, NULL);
    if (!window) { 
        std::cerr << "ERRO::GLFW::CRIACAO_DE_JANELA_FALHOU" << std::endl;
        glfwTerminate(); 
        return -1; 
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    const int gladVersion = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (gladVersion == 0) {
        std::cerr << "ERRO::GLAD::INICIALIZACAO_FALHOU" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    bool darkMode = false;
    setupTheme(darkMode);

    try {
        Shader barShader("shaders/bar.vs", "shaders/bar.fs");

        SortSimulator sortSim;

        float sortTickRate = 0.03f;
        double lastSortTime = glfwGetTime();

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Painel de Controle", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            if (ImGui::Checkbox("Modo Escuro", &darkMode)) setupTheme(darkMode);
            ImGui::Separator();
            ImGui::TextDisabled("Fluxo: escolha o algoritmo, ajuste os dados e controle a animacao.");
            ImGui::SeparatorText("Algoritmo e Dados");

            int curAlgo = static_cast<int>(sortSim.getAlgorithm());
            if (ImGui::Combo(
                    "Algoritmo",
                    &curAlgo,
                    [](void*, int idx, const char** outText) {
                        *outText = AlgorithmRegistry::getMetadataByIndex(idx).name;
                        return true;
                    },
                    nullptr,
                    AlgorithmRegistry::count())) {
                sortSim.setAlgorithm(static_cast<Algorithm>(curAlgo));
            }

            const char* dists[] = { "Aleatório", "Reverso", "Quase Ordenado", "Poucos Únicos" };
            int curDist = static_cast<int>(sortSim.getDistribution());
            if (ImGui::Combo("Distribuição", &curDist, dists, static_cast<int>(sizeof(dists) / sizeof(dists[0])))) sortSim.setDistribution(static_cast<ArrayDistribution>(curDist));

            const char* modes[] = { "Barras", "Dot Plot", "Circular" };
            int curMode = static_cast<int>(sortSim.getRenderMode());
            if (ImGui::Combo("Visualização", &curMode, modes, static_cast<int>(sizeof(modes) / sizeof(modes[0])))) sortSim.setRenderMode(static_cast<RenderMode>(curMode));

            int arraySize = sortSim.getArraySize();
            if (ImGui::SliderInt("Tamanho (N)", &arraySize, 10, 1000)) sortSim.setArraySize(arraySize);
            ImGui::SliderFloat("Velocidade da ordenação", &sortTickRate, 0.001f, 0.5f, "%.3f s/passo", ImGuiSliderFlags_Logarithmic);

            ImGui::SeparatorText("Controles");
            if (ImGui::Button(sortSim.isSortingActive() ? "Pausar" : "Iniciar")) sortSim.toggleSorting();
            ImGui::SameLine();
            if (ImGui::Button("Resetar")) sortSim.reset();
            ImGui::Text("Status: %s", sortSim.hasFinished() ? "Finalizado" : (sortSim.isSortingActive() ? "Executando" : "Pronto"));
            ImGui::Text("Comparações: %lld", sortSim.getComparisons());
            ImGui::Text("Acessos: %lld", sortSim.getWrites());
            ImGui::End();

            ImGui::Begin("Informação Pedagógica", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::TextWrapped("%s", getAlgorithmDescription(sortSim.getAlgorithm()));
            ImGui::Text("Comparações: %lld", sortSim.getComparisons());
            ImGui::Text("Acessos: %lld", sortSim.getWrites());
            ImGui::End();

            double currentTime = glfwGetTime();
            if (currentTime - lastSortTime >= sortTickRate) {
                if (sortSim.isSortingActive() || sortSim.isFinishingAnimation()) sortSim.step();
                lastSortTime = currentTime;
            }

            if (darkMode) glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
            else glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            sortSim.render(barShader, framebufferWidth, framebufferHeight);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
