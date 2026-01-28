/**
 * @file Graph.h
 * @brief Représentation du graphe du réseau routier
 * 
 * Cette classe modélise le réseau routier comme un graphe orienté
 * avec des nœuds (intersections) et des routes (arêtes).
 */

#ifndef GRAPH_H
#define GRAPH_H

#include "Route.h"
#include <vector>
#include <unordered_map>
#include <memory>

/**
 * @struct Node
 * @brief Représente un nœud (intersection) dans le graphe
 */
struct Node {
    int id;        ///< Identifiant unique du nœud
    float x, y;    ///< Position pour la visualisation
    
    /**
     * @brief Constructeur du nœud
     * @param id Identifiant unique
     * @param x Coordonnée X
     * @param y Coordonnée Y
     */
    Node(int id, float x, float y) : id(id), x(x), y(y) {}
};

/**
 * @class Graph
 * @brief Classe représentant le graphe du réseau routier
 * 
 * Gère les nœuds et routes du réseau, permet la recherche de chemins
 * et le chargement depuis un fichier de configuration JSON.
 */
class Graph {
private:
    std::vector<std::unique_ptr<Node>> nodes;
    std::vector<std::unique_ptr<Route>> routes;
    std::unordered_map<int, std::vector<int>> adjacencyList; // nodeId -> vector of route indices
    
public:
    Graph();
    ~Graph();
    
    // Ajout de nœuds et routes
    void addNode(int id, float x, float y);
    void addRoute(int id, int fromNode, int toNode, float length, float speed, int capacity);
    
    // Accès aux données
    Node* getNode(int id) const;
    Route* getRoute(int id) const;
    std::vector<int> getNeighbors(int nodeId) const;
    std::vector<int> getRoutesFromNode(int nodeId) const;
    
    // Recherche de chemin
    std::vector<int> findPath(int start, int end) const;
    
    // Mise à jour du trafic
    void updateTraffic();
    
    // Getters
    const std::vector<std::unique_ptr<Node>>& getNodes() const { return nodes; }
    const std::vector<std::unique_ptr<Route>>& getRoutes() const { return routes; }
    
    // Chargement depuis configuration
    void loadFromConfig(const std::string& configPath);
};

#endif // GRAPH_H

