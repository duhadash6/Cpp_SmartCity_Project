#include "PathfindingStrategy.h"
#include "Route.h"
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <limits>
#include <cmath>

// Structure pour A* algorithm
struct AStarNode {
    int nodeId;
    float gCost;  // Coût depuis le départ
    float hCost;  // Heuristique (distance estimée à la destination)
    float fCost() const { return gCost + hCost; }
    int parent;
    
    bool operator>(const AStarNode& other) const {
        return fCost() > other.fCost();
    }
};

// Structure pour Dijkstra
struct DijkstraNode {
    int nodeId;
    float distance;
    int parent;
    
    bool operator>(const DijkstraNode& other) const {
        return distance > other.distance;
    }
};

float AStarStrategy::heuristic(const Graph& graph, int node1, int node2) const {
    Node* n1 = graph.getNode(node1);
    Node* n2 = graph.getNode(node2);
    
    if (!n1 || !n2) {
        return std::numeric_limits<float>::max();
    }
    
    // Distance euclidienne
    float dx = n2->x - n1->x;
    float dy = n2->y - n1->y;
    return std::sqrt(dx * dx + dy * dy);
}

std::vector<int> AStarStrategy::reconstructPath(const std::unordered_map<int, int>& cameFrom, int current) const {
    std::vector<int> path;
    path.push_back(current);
    
    while (cameFrom.find(current) != cameFrom.end()) {
        current = cameFrom.at(current);
        path.push_back(current);
    }
    
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<int> AStarStrategy::findPath(const Graph& graph, int start, int end) const {
    if (start == end) {
        return std::vector<int>{start};
    }
    
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> openSet;
    std::unordered_map<int, float> gScore;
    std::unordered_map<int, int> cameFrom;
    std::unordered_map<int, bool> closedSet;
    
    // Initialisation
    gScore[start] = 0.0f;
    AStarNode startNode;
    startNode.nodeId = start;
    startNode.gCost = 0.0f;
    startNode.hCost = heuristic(graph, start, end);
    startNode.parent = -1;
    openSet.push(startNode);
    
    while (!openSet.empty()) {
        AStarNode current = openSet.top();
        openSet.pop();
        
        if (closedSet[current.nodeId]) {
            continue;
        }
        
        closedSet[current.nodeId] = true;
        
        if (current.nodeId == end) {
            return reconstructPath(cameFrom, end);
        }
        
        // Explorer les voisins
        std::vector<int> routesFromNode = graph.getRoutesFromNode(current.nodeId);
        
        for (int routeIdx : routesFromNode) {
            if (routeIdx < 0 || routeIdx >= static_cast<int>(graph.getRoutes().size())) {
                continue;
            }
            
            const Route* route = graph.getRoutes()[routeIdx].get();
            if (!route || !route->isUsable()) {
                continue;
            }
            
            int neighborId = (route->getFromNode() == current.nodeId) ? 
                            route->getToNode() : route->getFromNode();
            
            if (!graph.getNode(neighborId)) {
                continue;
            }
            
            if (closedSet[neighborId]) {
                continue;
            }
            
            // Coût = temps de parcours
            float travelTime = route->getTravelTime();
            if (!std::isfinite(travelTime) || travelTime < 0) {
                travelTime = 1.0f;
            }
            
            float tentativeGScore = gScore[current.nodeId] + travelTime;
            
            if (!std::isfinite(tentativeGScore)) {
                continue;
            }
            
            if (gScore.find(neighborId) == gScore.end() || 
                tentativeGScore < gScore[neighborId]) {
                cameFrom[neighborId] = current.nodeId;
                gScore[neighborId] = tentativeGScore;
                
                AStarNode neighborNode;
                neighborNode.nodeId = neighborId;
                neighborNode.gCost = tentativeGScore;
                neighborNode.hCost = heuristic(graph, neighborId, end);
                neighborNode.parent = current.nodeId;
                openSet.push(neighborNode);
            }
        }
    }
    
    // Pas de chemin trouvé
    return std::vector<int>();
}

std::vector<int> DijkstraStrategy::findPath(const Graph& graph, int start, int end) const {
    if (start == end) {
        return std::vector<int>{start};
    }
    
    std::priority_queue<DijkstraNode, std::vector<DijkstraNode>, std::greater<DijkstraNode>> pq;
    std::unordered_map<int, float> distances;
    std::unordered_map<int, int> cameFrom;
    std::unordered_map<int, bool> visited;
    
    // Initialisation
    distances[start] = 0.0f;
    DijkstraNode startNode;
    startNode.nodeId = start;
    startNode.distance = 0.0f;
    startNode.parent = -1;
    pq.push(startNode);
    
    while (!pq.empty()) {
        DijkstraNode current = pq.top();
        pq.pop();
        
        if (visited[current.nodeId]) {
            continue;
        }
        
        visited[current.nodeId] = true;
        
        if (current.nodeId == end) {
            // Reconstruction du chemin
            std::vector<int> path;
            int node = end;
            path.push_back(node);
            
            while (cameFrom.find(node) != cameFrom.end()) {
                node = cameFrom.at(node);
                path.push_back(node);
            }
            
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        // Explorer les voisins
        std::vector<int> routesFromNode = graph.getRoutesFromNode(current.nodeId);
        
        for (int routeIdx : routesFromNode) {
            if (routeIdx < 0 || routeIdx >= static_cast<int>(graph.getRoutes().size())) {
                continue;
            }
            
            const Route* route = graph.getRoutes()[routeIdx].get();
            if (!route || !route->isUsable()) {
                continue;
            }
            
            int neighborId = (route->getFromNode() == current.nodeId) ? 
                            route->getToNode() : route->getFromNode();
            
            if (!graph.getNode(neighborId)) {
                continue;
            }
            
            if (visited[neighborId]) {
                continue;
            }
            
            // Coût = temps de parcours (comme A*, mais sans heuristique)
            float travelTime = route->getTravelTime();
            if (!std::isfinite(travelTime) || travelTime < 0) {
                travelTime = 1.0f;
            }
            
            float newDistance = distances[current.nodeId] + travelTime;
            
            if (distances.find(neighborId) == distances.end() || 
                newDistance < distances[neighborId]) {
                distances[neighborId] = newDistance;
                cameFrom[neighborId] = current.nodeId;
                
                DijkstraNode neighborNode;
                neighborNode.nodeId = neighborId;
                neighborNode.distance = newDistance;
                neighborNode.parent = current.nodeId;
                pq.push(neighborNode);
            }
        }
    }
    
    // Pas de chemin trouvé
    return std::vector<int>();
}

