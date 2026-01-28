#ifndef PATHFINDING_STRATEGY_H
#define PATHFINDING_STRATEGY_H

/**
 * @file PathfindingStrategy.h
 * @brief Pattern Strategy pour les algorithmes de pathfinding
 * 
 * Ce fichier implémente le design pattern Strategy pour permettre
 * l'utilisation de différents algorithmes de recherche de chemin.
 */

#include "Graph.h"
#include <vector>

/**
 * @class PathfindingStrategy
 * @brief Interface pour les stratégies de pathfinding
 * 
 * Pattern Strategy : définit l'interface commune pour tous les
 * algorithmes de recherche de chemin.
 */
class PathfindingStrategy {
public:
    virtual ~PathfindingStrategy() = default;
    
    /**
     * @brief Calcule un chemin entre deux nœuds
     * @param graph Graphe du réseau routier
     * @param start Nœud de départ
     * @param end Nœud de destination
     * @return Vecteur d'IDs de nœuds représentant le chemin
     */
    virtual std::vector<int> findPath(const Graph& graph, int start, int end) const = 0;
};

/**
 * @class AStarStrategy
 * @brief Implémentation de l'algorithme A* pour le pathfinding
 * 
 * Stratégie utilisant l'algorithme A* avec prise en compte
 * du trafic et des conditions de route.
 */
class AStarStrategy : public PathfindingStrategy {
private:
    /**
     * @brief Heuristique (distance euclidienne)
     */
    float heuristic(const Graph& graph, int node1, int node2) const;
    
    /**
     * @brief Reconstruction du chemin depuis les parents
     */
    std::vector<int> reconstructPath(const std::unordered_map<int, int>& cameFrom, int current) const;
    
public:
    /**
     * @brief Calcule un chemin optimal avec A*
     */
    std::vector<int> findPath(const Graph& graph, int start, int end) const override;
};

/**
 * @class DijkstraStrategy
 * @brief Implémentation de l'algorithme Dijkstra pour le pathfinding
 * 
 * Stratégie alternative utilisant Dijkstra (sans heuristique).
 * Utile pour comparer les performances avec A*.
 */
class DijkstraStrategy : public PathfindingStrategy {
public:
    /**
     * @brief Calcule un chemin avec Dijkstra
     */
    std::vector<int> findPath(const Graph& graph, int start, int end) const override;
};

#endif // PATHFINDING_STRATEGY_H

