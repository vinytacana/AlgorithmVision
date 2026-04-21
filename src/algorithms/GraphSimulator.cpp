#include "GraphSimulator.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <cmath>

GraphSimulator::GraphSimulator() { createDefaultGraph(); setupMesh(); }

GraphSimulator::~GraphSimulator() {
    if (lineVBO != 0) glDeleteBuffers(1, &lineVBO);
    if (lineVAO != 0) glDeleteVertexArrays(1, &lineVAO);
    if (quadVBO != 0) glDeleteBuffers(1, &quadVBO);
    if (quadVAO != 0) glDeleteVertexArrays(1, &quadVAO);
}

void GraphSimulator::createDefaultGraph() {
    nodes.clear(); edges.clear();
    for (int i = 0; i < 6; ++i) {
        float angle = i * (2.0f * M_PI / 6.0f);
        nodes.push_back({{400 + 200 * cos(angle), 350 + 200 * sin(angle)}, NORMAL, i});
    }
    addEdge(0, 1, 1.0f); addEdge(1, 2, 2.0f); addEdge(2, 3, 3.0f);
    addEdge(3, 4, 1.0f); addEdge(4, 5, 2.0f); addEdge(5, 0, 1.0f);
    addEdge(0, 3, 5.0f);
    rebuildNodeIndex();
    sanitizeSelection();
}

void GraphSimulator::addNode(glm::vec2 pos) {
    int newId = 0; for(auto& n : nodes) if(n.id >= newId) newId = n.id + 1;
    nodes.push_back({pos, NORMAL, newId});
    rebuildNodeIndex();
    sanitizeSelection();
}

void GraphSimulator::removeNode(int id) {
    nodes.erase(std::remove_if(nodes.begin(), nodes.end(), [id](const Node& n){ return n.id == id; }), nodes.end());
    edges.erase(std::remove_if(edges.begin(), edges.end(), [id](const Edge& e){ return e.from == id || e.to == id; }), edges.end());
    rebuildNodeIndex();
    sanitizeSelection();
    reset();
}

void GraphSimulator::addEdge(int u, int v, float w) {
    if (u == v) return;
    for(auto& e : edges) if((e.from == u && e.to == v) || (e.from == v && e.to == u)) return;
    edges.push_back({u, v, w}); edges.push_back({v, u, w});
}

void GraphSimulator::setupMesh() {
    float vertices[] = { -0.5f,-0.5f,0,0,0, 0.5f,-0.5f,0,1,0, 0.5f,0.5f,0,1,1, -0.5f,0.5f,0,0,1 };
    glGenVertexArrays(1, &quadVAO); glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO); glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
    glGenVertexArrays(1, &lineVAO); glGenBuffers(1, &lineVBO);
}

int GraphSimulator::findNodeAt(glm::vec2 pos) {
    for (auto& n : nodes) if (glm::distance(pos, n.pos) < 30.0f) return n.id;
    return -1;
}

void GraphSimulator::handleMouseInteraction(double x, double y, bool leftPressed, bool rightPressed, bool shiftDown, bool ctrlDown, int w, int h) {
    (void)w;
    glm::vec2 mousePos((float)x, (float)(h - y));
    int nodeUnderMouse = findNodeAt(mousePos);
    if (rightPressed && !wasRightPressed && nodeUnderMouse != -1) {
        removeNode(nodeUnderMouse);
        wasRightPressed = rightPressed;
        wasLeftPressed = leftPressed;
        return;
    }
    if (leftPressed) {
        if (shiftDown && nodeUnderMouse == -1 && !wasLeftPressed) addNode(mousePos);
        else if (ctrlDown && nodeUnderMouse != -1) {
            if (!wasLeftPressed) {
                if (connectingFrom == -1) connectingFrom = nodeUnderMouse;
                else if (connectingFrom != nodeUnderMouse) { addEdge(connectingFrom, nodeUnderMouse, 1.0f); connectingFrom = -1; }
            }
        } else if (nodeUnderMouse != -1 || draggedNode != -1) {
            if (draggedNode == -1) draggedNode = nodeUnderMouse;
            if (Node* dragged = getNodeById(draggedNode)) dragged->pos = mousePos;
        }
    } else {
        draggedNode = -1;
        if (!ctrlDown) connectingFrom = -1;
    }
    wasLeftPressed = leftPressed;
    wasRightPressed = rightPressed;
}

void GraphSimulator::reset() {
    for (auto& n : nodes) { n.state = NORMAL; n.dist = 1e9; n.parent = -1; }
    visited.clear(); while(!q_bfs.empty()) q_bfs.pop(); while(!s_dfs.empty()) s_dfs.pop(); pq_dijkstra.clear();
    isRunning = false; isFinished = false; draggedNode = -1; connectingFrom = -1;
    sanitizeSelection();
}

void GraphSimulator::start() {
    if (nodes.empty()) return;
    reset(); isRunning = true;
    sanitizeSelection();
    if (selectedAlgo == BFS) q_bfs.push(startNode);
    else if (selectedAlgo == DFS) s_dfs.push(startNode);
    else if (selectedAlgo == DIJKSTRA) {
        if (Node* start = getNodeById(startNode)) start->dist = 0.0f;
        pq_dijkstra.insert({0, startNode});
    }
}

void GraphSimulator::step() {
    if (!isRunning || isFinished) return;
    if (selectedAlgo == BFS) stepBFS();
    else if (selectedAlgo == DFS) stepDFS();
    else if (selectedAlgo == DIJKSTRA) stepDijkstra();
}

void GraphSimulator::stepBFS() {
    if (q_bfs.empty()) { finish(); return; }
    int curr = q_bfs.front(); q_bfs.pop();
    if (visited.count(curr)) { stepBFS(); return; }
    visited.insert(curr);
    if (Node* current = getNodeById(curr)) current->state = VISITED;
    for (auto& e : edges) if (e.from == curr && !visited.count(e.to)) {
        q_bfs.push(e.to);
        if (Node* next = getNodeById(e.to)) {
            next->state = QUEUED;
            if (next->parent == -1) next->parent = curr;
        }
    }
}

void GraphSimulator::stepDFS() {
    if (s_dfs.empty()) { finish(); return; }
    int curr = s_dfs.top(); s_dfs.pop();
    if (visited.count(curr)) { stepDFS(); return; }
    visited.insert(curr);
    if (Node* current = getNodeById(curr)) current->state = VISITED;
    for (auto& e : edges) if (e.from == curr && !visited.count(e.to)) {
        s_dfs.push(e.to);
        if (Node* next = getNodeById(e.to)) {
            next->state = QUEUED;
            next->parent = curr;
        }
    }
}

void GraphSimulator::stepDijkstra() {
    if (pq_dijkstra.empty()) { finish(); return; }
    int curr = pq_dijkstra.begin()->second; pq_dijkstra.erase(pq_dijkstra.begin());
    if (visited.count(curr)) { stepDijkstra(); return; }
    visited.insert(curr);
    Node* current = getNodeById(curr);
    if (current == nullptr) return;
    current->state = VISITED;
    const float d_u = current->dist;
    for (auto& e : edges) if (e.from == curr) {
        if (Node* next = getNodeById(e.to)) {
            float newDist = d_u + e.weight;
            if (newDist < next->dist) {
                next->dist = newDist;
                next->parent = curr;
                pq_dijkstra.insert({newDist, next->id});
                next->state = QUEUED;
            }
        }
    }
}

void GraphSimulator::finish() {
    isRunning = false; isFinished = true;
    if (endNode != -1 && hasNode(endNode)) {
        int curr = endNode;
        while (curr != -1) {
            Node* node = getNodeById(curr);
            if (node == nullptr) break;
            node->state = PATH;
            curr = node->parent;
        }
    }
}

void GraphSimulator::render(Shader& circleShader, Shader& lineShader, int w, int h) {
    glm::mat4 projection = glm::ortho(0.0f, (float)w, 0.0f, (float)h, -1.0f, 1.0f);
    
    // Draw edges
    lineShader.use(); 
    lineShader.setMat4("projection", projection);
    lineShader.setMat4("model", glm::mat4(1.0f));
    lineShader.setVec3("color", 0.3f, 0.3f, 0.3f);
    
    glLineWidth(2.5f);
    glBindVertexArray(lineVAO);
    for (std::size_t edgeIndex = 0; edgeIndex < edges.size(); edgeIndex += 2) {
        const Edge& e = edges[edgeIndex];
        const Node* n1 = getNodeById(e.from);
        const Node* n2 = getNodeById(e.to);
        if(!n1 || !n2) continue;
        
        float pts[] = { n1->pos.x, n1->pos.y, 0, n2->pos.x, n2->pos.y, 0 };
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pts), pts, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); 
        glEnableVertexAttribArray(0);
        
        glDrawArrays(GL_LINES, 0, 2);
    }
    
    // Draw nodes
    circleShader.use(); 
    circleShader.setMat4("projection", projection);
    glBindVertexArray(quadVAO);
    for (auto& n : nodes) {
        glm::mat4 model = glm::mat4(1.0f); 
        model = glm::translate(model, glm::vec3(n.pos, 0.0f)); 
        model = glm::scale(model, glm::vec3(55.0f, 55.0f, 1.0f));
        circleShader.setMat4("model", model);
        
        int vs = 0; 
        if (n.state == QUEUED) vs = 1; 
        else if (n.state == VISITED) vs = 2; 
        else if (n.state == PATH) vs = 3; 
        else if (n.id == startNode || n.id == endNode) vs = 4;
        
        if (connectingFrom == n.id) vs = 1; 
        circleShader.setInt("state", vs);
        
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
}

void GraphSimulator::setAlgorithm(GraphAlgorithm algorithm) {
    if (selectedAlgo != algorithm) {
        selectedAlgo = algorithm;
        reset();
    }
}

GraphAlgorithm GraphSimulator::getAlgorithm() const { return selectedAlgo; }

bool GraphSimulator::isSimulationRunning() const { return isRunning; }

bool GraphSimulator::hasFinished() const { return isFinished; }

void GraphSimulator::togglePause() {
    if (!isFinished) {
        isRunning = !isRunning;
    }
}

int GraphSimulator::getStartNode() const { return startNode; }

int GraphSimulator::getEndNode() const { return endNode; }

void GraphSimulator::setStartNode(int id) {
    if (hasNode(id)) startNode = id;
}

void GraphSimulator::setEndNode(int id) {
    if (id == -1 || hasNode(id)) endNode = id;
}

bool GraphSimulator::hasNode(int id) const {
    return nodeIndexById.find(id) != nodeIndexById.end();
}

std::vector<int> GraphSimulator::getNodeIds() const {
    std::vector<int> ids;
    ids.reserve(nodes.size());
    for (const auto& node : nodes) ids.push_back(node.id);
    return ids;
}

const std::vector<Node>& GraphSimulator::getNodes() const { return nodes; }

const std::vector<Edge>& GraphSimulator::getEdges() const { return edges; }

void GraphSimulator::updateUndirectedEdgeWeight(int u, int v, float weight) {
    for (auto& edge : edges) {
        if ((edge.from == u && edge.to == v) || (edge.from == v && edge.to == u)) {
            edge.weight = weight;
        }
    }
}

int GraphSimulator::findNodeIndexById(int id) const {
    auto it = nodeIndexById.find(id);
    if (it == nodeIndexById.end()) return -1;
    return static_cast<int>(it->second);
}

Node* GraphSimulator::getNodeById(int id) {
    const int index = findNodeIndexById(id);
    if (index < 0) return nullptr;
    return &nodes[static_cast<std::size_t>(index)];
}

const Node* GraphSimulator::getNodeById(int id) const {
    const int index = findNodeIndexById(id);
    if (index < 0) return nullptr;
    return &nodes[static_cast<std::size_t>(index)];
}

void GraphSimulator::rebuildNodeIndex() {
    nodeIndexById.clear();
    for (std::size_t index = 0; index < nodes.size(); ++index) {
        nodeIndexById[nodes[index].id] = index;
    }
}

void GraphSimulator::sanitizeSelection() {
    if (nodes.empty()) {
        startNode = 0;
        endNode = -1;
        return;
    }
    if (!hasNode(startNode)) startNode = nodes.front().id;
    if (endNode != -1 && !hasNode(endNode)) endNode = -1;
    if (draggedNode != -1 && !hasNode(draggedNode)) draggedNode = -1;
    if (connectingFrom != -1 && !hasNode(connectingFrom)) connectingFrom = -1;
}
