#include "GraphSimulator.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <cmath>

GraphSimulator::GraphSimulator() { createDefaultGraph(); setupMesh(); }

void GraphSimulator::createDefaultGraph() {
    nodes.clear(); edges.clear();
    for (int i = 0; i < 6; ++i) {
        float angle = i * (2.0f * M_PI / 6.0f);
        nodes.push_back({{400 + 200 * cos(angle), 350 + 200 * sin(angle)}, NORMAL, i});
    }
    addEdge(0, 1, 1.0f); addEdge(1, 2, 2.0f); addEdge(2, 3, 3.0f);
    addEdge(3, 4, 1.0f); addEdge(4, 5, 2.0f); addEdge(5, 0, 1.0f);
    addEdge(0, 3, 5.0f);
}

void GraphSimulator::addNode(glm::vec2 pos) {
    int newId = 0; for(auto& n : nodes) if(n.id >= newId) newId = n.id + 1;
    nodes.push_back({pos, NORMAL, newId});
}

void GraphSimulator::removeNode(int id) {
    nodes.erase(std::remove_if(nodes.begin(), nodes.end(), [id](const Node& n){ return n.id == id; }), nodes.end());
    edges.erase(std::remove_if(edges.begin(), edges.end(), [id](const Edge& e){ return e.from == id || e.to == id; }), edges.end());
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
    glm::vec2 mousePos((float)x, (float)(h - y));
    int nodeUnderMouse = findNodeAt(mousePos);
    if (rightPressed && nodeUnderMouse != -1) { removeNode(nodeUnderMouse); return; }
    if (leftPressed) {
        if (shiftDown && nodeUnderMouse == -1) addNode(mousePos);
        else if (ctrlDown && nodeUnderMouse != -1) {
            if (connectingFrom == -1) connectingFrom = nodeUnderMouse;
            else if (connectingFrom != nodeUnderMouse) { addEdge(connectingFrom, nodeUnderMouse, 1.0f); connectingFrom = -1; }
        } else if (nodeUnderMouse != -1) { draggedNode = nodeUnderMouse; nodes[draggedNode].pos = mousePos; }
    } else { draggedNode = -1; if (!ctrlDown) connectingFrom = -1; }
}

void GraphSimulator::reset() {
    for (auto& n : nodes) { n.state = NORMAL; n.dist = 1e9; n.parent = -1; }
    visited.clear(); while(!q_bfs.empty()) q_bfs.pop(); while(!s_dfs.empty()) s_dfs.pop(); pq_dijkstra.clear();
    isRunning = false; isFinished = false;
}

void GraphSimulator::start() {
    if (nodes.empty()) return;
    reset(); isRunning = true;
    bool exists = false; for(auto& n : nodes) if(n.id == startNode) exists = true;
    if (!exists) startNode = nodes[0].id;
    if (selectedAlgo == BFS) q_bfs.push(startNode);
    else if (selectedAlgo == DFS) s_dfs.push(startNode);
    else if (selectedAlgo == DIJKSTRA) { for(auto& n : nodes) if(n.id == startNode) n.dist = 0; pq_dijkstra.insert({0, startNode}); }
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
    for (auto& n : nodes) if(n.id == curr) n.state = VISITED;
    for (auto& e : edges) if (e.from == curr && !visited.count(e.to)) {
        q_bfs.push(e.to);
        for(auto& n : nodes) if(n.id == e.to) { n.state = QUEUED; if (n.parent == -1) n.parent = curr; }
    }
}

void GraphSimulator::stepDFS() {
    if (s_dfs.empty()) { finish(); return; }
    int curr = s_dfs.top(); s_dfs.pop();
    if (visited.count(curr)) { stepDFS(); return; }
    visited.insert(curr);
    for (auto& n : nodes) if(n.id == curr) n.state = VISITED;
    for (auto& e : edges) if (e.from == curr && !visited.count(e.to)) {
        s_dfs.push(e.to);
        for(auto& n : nodes) if(n.id == e.to) { n.state = QUEUED; n.parent = curr; }
    }
}

void GraphSimulator::stepDijkstra() {
    if (pq_dijkstra.empty()) { finish(); return; }
    int curr = pq_dijkstra.begin()->second; pq_dijkstra.erase(pq_dijkstra.begin());
    if (visited.count(curr)) { stepDijkstra(); return; }
    visited.insert(curr);
    for (auto& n : nodes) if(n.id == curr) n.state = VISITED;
    for (auto& e : edges) if (e.from == curr) {
        float d_u = 1e9; for(auto& n : nodes) if(n.id == curr) d_u = n.dist;
        for(auto& n_to : nodes) if(n_to.id == e.to) {
            float newDist = d_u + e.weight;
            if (newDist < n_to.dist) { n_to.dist = newDist; n_to.parent = curr; pq_dijkstra.insert({newDist, n_to.id}); n_to.state = QUEUED; }
        }
    }
}

void GraphSimulator::finish() {
    isRunning = false; isFinished = true;
    if (endNode != -1) { int curr = endNode; while (curr != -1) { bool found = false; for(auto& n : nodes) if(n.id == curr) { n.state = PATH; curr = n.parent; found = true; break; } if(!found) break; } }
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
    for (auto& e : edges) {
        Node *n1 = nullptr, *n2 = nullptr; 
        for(auto& n : nodes) { 
            if(n.id == e.from) n1 = &n; 
            if(n.id == e.to) n2 = &n; 
        }
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
