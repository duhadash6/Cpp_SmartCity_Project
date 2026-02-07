#include "../include/PathPlanner.h"
#include "../include/Graph.h"
#include <cassert>
#include <iostream>

void testPathPlanner() {
    Graph graph;
    graph.addNode(0, 0.0f, 0.0f);
    graph.addNode(1, 100.0f, 0.0f);
    graph.addNode(2, 200.0f, 0.0f);
    graph.addNode(3, 100.0f, 100.0f);
    
    graph.addRoute(0, 0, 1, 100.0f, 60.0f, 20);
    graph.addRoute(1, 1, 2, 100.0f, 60.0f, 20);
    graph.addRoute(2, 0, 3, 141.0f, 60.0f, 20);
    graph.addRoute(3, 3, 2, 141.0f, 60.0f, 20);
    
    PathPlanner planner(&graph);
    std::vector<int> path = planner.planPath(0, 2);
    
    assert(!path.empty());
    assert(path[0] == 0);
    assert(path.back() == 2);
    
    std::cout << "Test planificateur: OK" << std::endl;
}

void testPathPlannerWithTraffic() {
    Graph graph;
    graph.addNode(0, 0.0f, 0.0f);
    graph.addNode(1, 100.0f, 0.0f);
    graph.addNode(2, 200.0f, 0.0f);
    
    graph.addRoute(0, 0, 1, 100.0f, 60.0f, 20);
    graph.addRoute(1, 1, 2, 100.0f, 60.0f, 20);
    
    // Congestionner la première route
    Route* route = graph.getRoute(0);
    route->setCongestion(0.9f);
    
    PathPlanner planner(&graph);
    std::vector<int> path = planner.planPath(0, 2);
    
    // Le planificateur devrait toujours trouver un chemin
    assert(!path.empty());
    
    std::cout << "Test planificateur avec trafic: OK" << std::endl;
}

int main() {
    std::cout << "=== Tests PathPlanner ===" << std::endl;
    testPathPlanner();
    testPathPlannerWithTraffic();
    std::cout << "Tous les tests PathPlanner sont passes!" << std::endl;
    return 0;
}

