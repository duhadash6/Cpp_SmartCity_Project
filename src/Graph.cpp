#include "Graph.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <queue>
#include <algorithm>
#include <unordered_map>

Graph::Graph() {
}

Graph::~Graph() {
}

void Graph::addNode(int id, float x, float y) {
    nodes.push_back(std::make_unique<Node>(id, x, y));
}

void Graph::addRoute(int id, int fromNode, int toNode, float length, float speed, int capacity) {
    auto route = std::make_unique<Route>(id, fromNode, toNode, length, speed, capacity);
    routes.push_back(std::move(route));
    
    // Mise à jour de la liste d'adjacence (bidirectionnelle)
    adjacencyList[fromNode].push_back(routes.size() - 1);
    adjacencyList[toNode].push_back(routes.size() - 1);
}

Node* Graph::getNode(int id) const {
    for (const auto& node : nodes) {
        if (node->id == id) {
            return node.get();
        }
    }
    return nullptr;
}

Route* Graph::getRoute(int id) const {
    for (const auto& route : routes) {
        if (route->getId() == id) {
            return route.get();
        }
    }
    return nullptr;
}

std::vector<int> Graph::getNeighbors(int nodeId) const {
    std::vector<int> neighbors;
    if (adjacencyList.find(nodeId) != adjacencyList.end()) {
        for (int routeIdx : adjacencyList.at(nodeId)) {
            Route* route = routes[routeIdx].get();
            if (route->getToNode() != nodeId) {
                neighbors.push_back(route->getToNode());
            } else {
                neighbors.push_back(route->getFromNode());
            }
        }
    }
    return neighbors;
}

std::vector<int> Graph::getRoutesFromNode(int nodeId) const {
    if (adjacencyList.find(nodeId) != adjacencyList.end()) {
        return adjacencyList.at(nodeId);
    }
    return std::vector<int>();
}

std::vector<int> Graph::findPath(int start, int end) const {
    // Algorithme simple BFS pour trouver un chemin
    // (sera remplacé par A* dans PathPlanner)
    std::queue<int> queue;
    std::unordered_map<int, int> parent;
    std::unordered_map<int, bool> visited;
    
    queue.push(start);
    visited[start] = true;
    parent[start] = -1;
    
    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();
        
        if (current == end) {
            // Reconstruction du chemin
            std::vector<int> path;
            int node = end;
            while (node != -1) {
                path.push_back(node);
                node = parent[node];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        for (int routeIdx : getRoutesFromNode(current)) {
            Route* route = routes[routeIdx].get();
            if (!route->isUsable()) continue;
            
            int nextNode = (route->getFromNode() == current) ? route->getToNode() : route->getFromNode();
            if (!visited[nextNode]) {
                visited[nextNode] = true;
                parent[nextNode] = current;
                queue.push(nextNode);
            }
        }
    }
    
    return std::vector<int>(); // Pas de chemin trouvé
}

void Graph::updateTraffic() {
    for (auto& route : routes) {
        route->updateSpeed();
    }
}

void Graph::loadFromConfig(const std::string& configPath) {
    // Chargement simplifié - sera implémenté avec JSON
    // Pour l'instant, création d'un graphe de test
    std::cout << "Chargement de la configuration depuis: " << configPath << std::endl;
}

