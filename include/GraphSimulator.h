#ifndef GRAPH_SIMULATOR_H
#define GRAPH_SIMULATOR_H

#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <glm/glm.hpp>
#include "Shader.h"

enum GraphAlgorithm { BFS, DFS, DIJKSTRA };
enum NodeState { NORMAL, QUEUED, VISITED, PATH, START_NODE, END_NODE };

struct Node {
    glm::vec2 pos;
    NodeState state = NORMAL;
    int id;
    float dist = 1e9;
    int parent = -1;
};

struct Edge {
    int from, to;
    float weight;
};

class GraphSimulator {
public:
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    unsigned int quadVAO, quadVBO, lineVAO, lineVBO;

    GraphAlgorithm selectedAlgo = BFS;
    bool isRunning = false;
    bool isFinished = false;
    int startNode = 0, endNode = -1;
    int draggedNode = -1;
    int connectingFrom = -1;

    std::queue<int> q_bfs;
    std::stack<int> s_dfs;
    std::set<std::pair<float, int>> pq_dijkstra;
    std::set<int> visited;

    GraphSimulator();
    void reset();
    void start();
    void step();
    void render(Shader& circleShader, Shader& lineShader, int w, int h);
    void handleMouseInteraction(double x, double y, bool leftPressed, bool rightPressed, bool shiftDown, bool ctrlDown, int w, int h);
    void addNode(glm::vec2 pos);
    void removeNode(int id);
    void addEdge(int u, int v, float w);

private:
    void createDefaultGraph();
    void setupMesh();
    int findNodeAt(glm::vec2 pos);
    void stepBFS();
    void stepDFS();
    void stepDijkstra();
    void finish();
};

#endif
