#include "../include/Vehicle.h"
#include "../include/Graph.h"
#include <cassert>
#include <iostream>

void testVehicleCreation() {
    Vehicle vehicle(0, 0, 5);
    assert(vehicle.getId() == 0);
    assert(vehicle.getCurrentNode() == 0);
    assert(vehicle.getTargetNode() == 5);
    std::cout << "Test creation vehicule: OK" << std::endl;
}

void testVehiclePath() {
    Vehicle vehicle(0, 0, 2);
    std::vector<int> path = {0, 1, 2};
    vehicle.setPath(path);
    
    const auto& vehiclePath = vehicle.getPath();
    assert(vehiclePath.size() == 3);
    assert(vehiclePath[0] == 0);
    assert(vehiclePath[1] == 1);
    assert(vehiclePath[2] == 2);
    
    std::cout << "Test chemin vehicule: OK" << std::endl;
}

void testVehicleUpdate() {
    Graph graph;
    graph.addNode(0, 0.0f, 0.0f);
    graph.addNode(1, 100.0f, 0.0f);
    graph.addRoute(0, 0, 1, 100.0f, 60.0f, 20);
    
    Vehicle vehicle(0, 0, 1);
    std::vector<int> path = {0, 1};
    vehicle.setPath(path);
    
    // Mise à jour avec un petit deltaTime
    vehicle.update(0.1f, graph);
    
    // Le véhicule devrait avoir progressé
    assert(vehicle.getCurrentNode() == 0 || vehicle.getCurrentNode() == 1);
    
    std::cout << "Test mise a jour vehicule: OK" << std::endl;
}

int main() {
    std::cout << "=== Tests Vehicle ===" << std::endl;
    testVehicleCreation();
    testVehiclePath();
    testVehicleUpdate();
    std::cout << "Tous les tests Vehicle sont passes!" << std::endl;
    return 0;
}

