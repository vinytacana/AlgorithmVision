#ifndef GRAPH_SIMULATOR_H
#define GRAPH_SIMULATOR_H

#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <unordered_map>
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
    GraphSimulator();
    ~GraphSimulator();
    GraphSimulator(const GraphSimulator&) = delete;
    GraphSimulator& operator=(const GraphSimulator&) = delete;

    void reset();
    void start();
    void step();
    void render(Shader& circleShader, Shader& lineShader, int w, int h);
    void handleMouseInteraction(double x, double y, bool leftPressed, bool rightPressed, bool shiftDown, bool ctrlDown, int w, int h);
    void addNode(glm::vec2 pos);
    void removeNode(int id);
    void addEdge(int u, int v, float w);

    void setAlgorithm(GraphAlgorithm algorithm);
    GraphAlgorithm getAlgorithm() const;

    bool isSimulationRunning() const;
    bool hasFinished() const;
    void togglePause();

    int getStartNode() const;
    int getEndNode() const;
    void setStartNode(int id);
    void setEndNode(int id);
    bool hasNode(int id) const;
    std::vector<int> getNodeIds() const;

    const std::vector<Node>& getNodes() const;
    const std::vector<Edge>& getEdges() const;
    void updateUndirectedEdgeWeight(int u, int v, float weight);

private:
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::unordered_map<int, std::size_t> nodeIndexById;
    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;
    unsigned int lineVAO = 0;
    unsigned int lineVBO = 0;

    GraphAlgorithm selectedAlgo = BFS;
    bool isRunning = false;
    bool isFinished = false;
    int startNode = 0;
    int endNode = -1;
    int draggedNode = -1;
    int connectingFrom = -1;
    bool wasLeftPressed = false;
    bool wasRightPressed = false;

    std::queue<int> q_bfs;
    std::stack<int> s_dfs;
    std::set<std::pair<float, int>> pq_dijkstra;
    std::set<int> visited;

    void createDefaultGraph();
    void setupMesh();
    int findNodeAt(glm::vec2 pos);
    int findNodeIndexById(int id) const;
    Node* getNodeById(int id);
    const Node* getNodeById(int id) const;
    void rebuildNodeIndex();
    void sanitizeSelection();
    void stepBFS();
    void stepDFS();
    void stepDijkstra();
    void finish();
};

#endif
