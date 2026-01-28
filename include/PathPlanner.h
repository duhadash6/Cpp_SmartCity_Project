#ifndef PATHPLANNER_H
#define PATHPLANNER_H

/**
 * @file PathPlanner.h
 * @brief Planificateur de trajets utilisant le pattern Strategy
 * 
 * Cette classe utilise le pattern Strategy pour permettre l'utilisation
 * de différents algorithmes de pathfinding (A*, Dijkstra, etc.)
 */

#include "Graph.h"
#include "PathfindingStrategy.h"
#include <vector>
#include <memory>

/**
 * @class PathPlanner
 * @brief Planificateur de trajets avec support de différentes stratégies
 * 
 * Utilise le pattern Strategy pour permettre de changer d'algorithme
 * de pathfinding à l'exécution. Par défaut, utilise A*.
 */
class PathPlanner {
private:
    const Graph* graph;
    std::unique_ptr<PathfindingStrategy> strategy;  // Strategy Pattern
    
public:
    /**
     * @brief Constructeur avec stratégie par défaut (A*)
     * @param graph Pointeur vers le graphe du réseau routier
     */
    PathPlanner(const Graph* graph);
    
    /**
     * @brief Constructeur avec stratégie personnalisée
     * @param graph Pointeur vers le graphe du réseau routier
     * @param strategy Stratégie de pathfinding à utiliser
     */
    PathPlanner(const Graph* graph, std::unique_ptr<PathfindingStrategy> strategy);
    
    /**
     * @brief Change la stratégie de pathfinding
     * @param strategy Nouvelle stratégie à utiliser
     */
    void setStrategy(std::unique_ptr<PathfindingStrategy> strategy);
    
    /**
     * @brief Planification du chemin optimal tenant compte du trafic
     * @param start Nœud de départ
     * @param end Nœud de destination
     * @return Vecteur d'IDs de nœuds représentant le chemin
     */
    std::vector<int> planPath(int start, int end) const;
    
    /**
     * @brief Replanification en cas de changement de conditions
     * @param start Nœud de départ (position actuelle)
     * @param end Nœud de destination
     * @param currentPath Chemin actuel (non utilisé, conservé pour compatibilité)
     * @param currentPosition Position actuelle dans le chemin
     * @return Nouveau chemin recalculé
     */
    std::vector<int> replanPath(int start, int end, const std::vector<int>& currentPath, int currentPosition) const;
};

#endif // PATHPLANNER_H

