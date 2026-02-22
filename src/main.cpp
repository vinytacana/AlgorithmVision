#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "SortSimulator.h"
#include "GraphSimulator.h"
#include "UIComponents.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>

enum ActiveTab { SORTING, GRAPH };
ActiveTab activeTab = SORTING;
bool darkMode = false;

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
    
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        // No Linux, o GLEW costuma retornar "Unknown error" mesmo quando funciona.
        // Verificamos se funções essenciais foram realmente carregadas.
        if (glCreateShader == nullptr) {
            std::cerr << "ERRO::GLEW::INICIALIZACAO_REAL_FALHOU: " << glewGetErrorString(err) << std::endl;
            return -1;
        }
    }
    // Limpa erro dummy gerado pelo glewInit()
    glGetError(); 

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    setupTheme(darkMode);

    Shader barShader("shaders/bar.vs", "shaders/bar.fs");
    Shader circleShader("shaders/circle.vs", "shaders/circle.fs");
    Shader lineShader("shaders/line.vs", "shaders/line.fs");
    
    SortSimulator sortSim;
    GraphSimulator graphSim;

    float tickRate = 0.05f;
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (!ImGui::GetIO().WantCaptureMouse) {
            double mx, my; glfwGetCursorPos(window, &mx, &my);
            int w, h; glfwGetWindowSize(window, &w, &h);
            bool lClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
            bool rClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
            bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
            bool ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
            if (activeTab == GRAPH) graphSim.handleMouseInteraction(mx, my, lClick, rClick, shift, ctrl, w, h);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Painel de Controle", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            if (ImGui::Checkbox("Modo Escuro", &darkMode)) setupTheme(darkMode);
            ImGui::Separator();

            if (ImGui::BeginTabBar("MainTabs")) {
                if (ImGui::BeginTabItem("Ordenação")) {
                    activeTab = SORTING;
                    const char* algos[] = { "Bubble Sort", "Quick Sort", "Merge Sort" };
                    int curAlgo = (int)sortSim.selectedAlgo;
                    if (ImGui::Combo("Algoritmo", &curAlgo, algos, 3)) { sortSim.selectedAlgo = (Algorithm)curAlgo; sortSim.reset(); }
                    const char* dists[] = { "Aleatório", "Reverso", "Quase Ordenado", "Poucos Únicos" };
                    int curDist = (int)sortSim.distribution;
                    if (ImGui::Combo("Distribuição", &curDist, dists, 4)) { sortSim.distribution = (ArrayDistribution)curDist; sortSim.reset(); }
                    const char* modes[] = { "Barras", "Dot Plot", "Circular" };
                    int curMode = (int)sortSim.renderMode;
                    if (ImGui::Combo("Visualização", &curMode, modes, 3)) sortSim.renderMode = (RenderMode)curMode;
                    if (ImGui::Button(sortSim.isSorting ? "Pausar" : "Iniciar")) sortSim.isSorting = !sortSim.isSorting;
                    ImGui::SameLine(); if (ImGui::Button("Resetar")) sortSim.reset();
                    ImGui::SliderFloat("Velocidade", &tickRate, 0.0001f, 0.5f);
                    if (ImGui::SliderInt("Tamanho (N)", &sortSim.arraySize, 10, 1000)) sortSim.reset();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Grafos")) {
                    activeTab = GRAPH;
                    const char* algos[] = { "BFS", "DFS", "Dijkstra" };
                    int curAlgo = (int)graphSim.selectedAlgo;
                    if (ImGui::Combo("Algoritmo", &curAlgo, algos, 3)) { graphSim.selectedAlgo = (GraphAlgorithm)curAlgo; graphSim.reset(); }
                    if (ImGui::Button(graphSim.isRunning ? "Pausar" : "Iniciar")) { if (!graphSim.isRunning && !graphSim.isFinished) graphSim.start(); else graphSim.isRunning = !graphSim.isRunning; }
                    ImGui::SameLine(); if (ImGui::Button("Resetar")) graphSim.reset();
                    ImGui::SliderInt("Nó Inicial", &graphSim.startNode, 0, graphSim.nodes.empty() ? 0 : 50);
                    ImGui::SliderInt("Nó Destino", &graphSim.endNode, -1, graphSim.nodes.empty() ? 0 : 50);
                    if (ImGui::CollapsingHeader("Configurar Pesos")) {
                        for(int i=0; i < (int)graphSim.edges.size(); i += 2) {
                            char buf[32]; sprintf(buf, "Peso %d-%d", graphSim.edges[i].from, graphSim.edges[i].to);
                            if (ImGui::SliderFloat(buf, &graphSim.edges[i].weight, 0.5f, 10.0f)) graphSim.edges[i+1].weight = graphSim.edges[i].weight;
                        }
                    }
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::End();
        }

        {
            ImGui::Begin("Informação Pedagógica", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
            int curAlgo = (activeTab == SORTING) ? (int)sortSim.selectedAlgo : (int)graphSim.selectedAlgo;
            ImGui::TextWrapped("%s", getAlgorithmDescription(curAlgo, activeTab == SORTING));
            if (activeTab == SORTING) { ImGui::Text("Comparações: %lld", sortSim.comparisons); ImGui::Text("Acessos: %lld", sortSim.writes); }
            ImGui::End();
        }

        double currentTime = glfwGetTime();
        if (currentTime - lastTime >= tickRate) {
            if (activeTab == SORTING && (sortSim.isSorting || sortSim.isFinishing)) sortSim.step();
            if (activeTab == GRAPH && graphSim.isRunning) graphSim.step();
            lastTime = currentTime;
        }

        if (darkMode) glClearColor(0.1f, 0.1f, 0.12f, 1.0f); else glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        int w, h; glfwGetFramebufferSize(window, &w, &h);
        if (activeTab == SORTING) sortSim.render(barShader, w, h);
        else if (activeTab == GRAPH) graphSim.render(circleShader, lineShader, w, h);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown(); ImGui_ImplGlfw_Shutdown(); ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
