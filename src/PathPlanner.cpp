#include "PathPlanner.h"
#include "PathfindingStrategy.h"
#include "Graph.h"

PathPlanner::PathPlanner(const Graph* graph) 
    : graph(graph), strategy(std::make_unique<AStarStrategy>()) {
}

PathPlanner::PathPlanner(const Graph* graph, std::unique_ptr<PathfindingStrategy> strategy)
    : graph(graph), strategy(std::move(strategy)) {
}

void PathPlanner::setStrategy(std::unique_ptr<PathfindingStrategy> newStrategy) {
    strategy = std::move(newStrategy);
}

std::vector<int> PathPlanner::planPath(int start, int end) const {
    if (!strategy) {
        return std::vector<int>();
    }
    return strategy->findPath(*graph, start, end);
}

std::vector<int> PathPlanner::replanPath(int start, int end, 
                                         const std::vector<int>& currentPath, 
                                         int currentPosition) const {
    // Replanification depuis la position actuelle
    // Note: start et currentPath sont conservés pour compatibilité avec l'interface
    (void)start;        // Supprimer l'avertissement unused parameter
    (void)currentPath;  // Supprimer l'avertissement unused parameter
    return planPath(currentPosition, end);
}

