#include "GLLoader.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "SortSimulator.h"
#include "GraphSimulator.h"
#include "UIComponents.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

enum ActiveTab { SORTING, GRAPH };

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

#if ALGORITHM_VISION_HAS_GLAD
    const int gladVersion = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
    if (gladVersion == 0) {
        std::cerr << "ERRO::GLAD::INICIALIZACAO_FALHOU" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
#else
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        if (glCreateShader == nullptr) {
            std::cerr << "ERRO::GLEW::INICIALIZACAO_REAL_FALHOU: " << glewGetErrorString(err) << std::endl;
            glfwDestroyWindow(window);
            glfwTerminate();
            return -1;
        }
    }
    glGetError();
#endif

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ActiveTab activeTab = SORTING;
    bool darkMode = false;
    setupTheme(darkMode);

    try {
        Shader barShader("shaders/bar.vs", "shaders/bar.fs");
        Shader circleShader("shaders/circle.vs", "shaders/circle.fs");
        Shader lineShader("shaders/line.vs", "shaders/line.fs");

        SortSimulator sortSim;
        GraphSimulator graphSim;

        float tickRate = 0.05f;
        double lastTime = glfwGetTime();

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            int framebufferWidth = 0;
            int framebufferHeight = 0;
            int windowWidth = 0;
            int windowHeight = 0;
            glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
            glfwGetWindowSize(window, &windowWidth, &windowHeight);

            if (!ImGui::GetIO().WantCaptureMouse) {
                double mx = 0.0;
                double my = 0.0;
                glfwGetCursorPos(window, &mx, &my);
                const double scaleX = windowWidth > 0 ? static_cast<double>(framebufferWidth) / windowWidth : 1.0;
                const double scaleY = windowHeight > 0 ? static_cast<double>(framebufferHeight) / windowHeight : 1.0;
                bool lClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
                bool rClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
                bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
                bool ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
                if (activeTab == GRAPH) {
                    graphSim.handleMouseInteraction(mx * scaleX, my * scaleY, lClick, rClick, shift, ctrl, framebufferWidth, framebufferHeight);
                }
            }

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Painel de Controle", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            if (ImGui::Checkbox("Modo Escuro", &darkMode)) setupTheme(darkMode);
            ImGui::Separator();

            if (ImGui::BeginTabBar("MainTabs")) {
                if (ImGui::BeginTabItem("Ordenação")) {
                    activeTab = SORTING;
                    const char* algos[] = { "Bubble Sort", "Quick Sort", "Merge Sort" };
                    int curAlgo = static_cast<int>(sortSim.getAlgorithm());
                    if (ImGui::Combo("Algoritmo", &curAlgo, algos, 3)) sortSim.setAlgorithm(static_cast<Algorithm>(curAlgo));

                    const char* dists[] = { "Aleatório", "Reverso", "Quase Ordenado", "Poucos Únicos" };
                    int curDist = static_cast<int>(sortSim.getDistribution());
                    if (ImGui::Combo("Distribuição", &curDist, dists, 4)) sortSim.setDistribution(static_cast<ArrayDistribution>(curDist));

                    const char* modes[] = { "Barras", "Dot Plot", "Circular" };
                    int curMode = static_cast<int>(sortSim.getRenderMode());
                    if (ImGui::Combo("Visualização", &curMode, modes, 3)) sortSim.setRenderMode(static_cast<RenderMode>(curMode));

                    if (ImGui::Button(sortSim.isSortingActive() ? "Pausar" : "Iniciar")) sortSim.toggleSorting();
                    ImGui::SameLine();
                    if (ImGui::Button("Resetar")) sortSim.reset();

                    ImGui::SliderFloat("Velocidade", &tickRate, 0.0001f, 0.5f);
                    int arraySize = sortSim.getArraySize();
                    if (ImGui::SliderInt("Tamanho (N)", &arraySize, 10, 1000)) sortSim.setArraySize(arraySize);
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Grafos")) {
                    activeTab = GRAPH;
                    const char* algos[] = { "BFS", "DFS", "Dijkstra" };
                    int curAlgo = static_cast<int>(graphSim.getAlgorithm());
                    if (ImGui::Combo("Algoritmo", &curAlgo, algos, 3)) graphSim.setAlgorithm(static_cast<GraphAlgorithm>(curAlgo));

                    if (ImGui::Button(graphSim.isSimulationRunning() ? "Pausar" : "Iniciar")) {
                        if (!graphSim.isSimulationRunning() && !graphSim.hasFinished()) graphSim.start();
                        else graphSim.togglePause();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Resetar")) graphSim.reset();

                    const std::vector<int> nodeIds = graphSim.getNodeIds();
                    if (nodeIds.empty()) {
                        ImGui::TextDisabled("Adicione nós com Shift + clique.");
                    } else {
                        int startIndex = 0;
                        for (int i = 0; i < static_cast<int>(nodeIds.size()); ++i) {
                            if (nodeIds[i] == graphSim.getStartNode()) {
                                startIndex = i;
                                break;
                            }
                        }
                        std::string startPreview = "Nó " + std::to_string(nodeIds[startIndex]);
                        if (ImGui::BeginCombo("Nó Inicial", startPreview.c_str())) {
                            for (int id : nodeIds) {
                                const bool selected = id == graphSim.getStartNode();
                                std::string label = "Nó " + std::to_string(id);
                                if (ImGui::Selectable(label.c_str(), selected)) graphSim.setStartNode(id);
                                if (selected) ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }

                        std::string endPreview = graphSim.getEndNode() == -1 ? "Nenhum" : "Nó " + std::to_string(graphSim.getEndNode());
                        if (ImGui::BeginCombo("Nó Destino", endPreview.c_str())) {
                            if (ImGui::Selectable("Nenhum", graphSim.getEndNode() == -1)) graphSim.setEndNode(-1);
                            for (int id : nodeIds) {
                                const bool selected = id == graphSim.getEndNode();
                                std::string label = "Nó " + std::to_string(id);
                                if (ImGui::Selectable(label.c_str(), selected)) graphSim.setEndNode(id);
                                if (selected) ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }
                    }

                    if (ImGui::CollapsingHeader("Configurar Pesos")) {
                        const auto& edges = graphSim.getEdges();
                        for (std::size_t i = 0; i < edges.size(); i += 2) {
                            float weight = edges[i].weight;
                            char buf[32];
                            std::snprintf(buf, sizeof(buf), "Peso %d-%d", edges[i].from, edges[i].to);
                            if (ImGui::SliderFloat(buf, &weight, 0.5f, 10.0f)) {
                                graphSim.updateUndirectedEdgeWeight(edges[i].from, edges[i].to, weight);
                            }
                        }
                    }
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::End();

            ImGui::Begin("Informação Pedagógica", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
            int curAlgo = activeTab == SORTING ? static_cast<int>(sortSim.getAlgorithm()) : static_cast<int>(graphSim.getAlgorithm());
            ImGui::TextWrapped("%s", getAlgorithmDescription(curAlgo, activeTab == SORTING));
            if (activeTab == SORTING) {
                ImGui::Text("Comparações: %lld", sortSim.getComparisons());
                ImGui::Text("Acessos: %lld", sortSim.getWrites());
            }
            ImGui::End();

            double currentTime = glfwGetTime();
            if (currentTime - lastTime >= tickRate) {
                if (activeTab == SORTING && (sortSim.isSortingActive() || sortSim.isFinishingAnimation())) sortSim.step();
                if (activeTab == GRAPH && graphSim.isSimulationRunning()) graphSim.step();
                lastTime = currentTime;
            }

            if (darkMode) glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
            else glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            if (activeTab == SORTING) sortSim.render(barShader, framebufferWidth, framebufferHeight);
            else if (activeTab == GRAPH) graphSim.render(circleShader, lineShader, framebufferWidth, framebufferHeight);

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
