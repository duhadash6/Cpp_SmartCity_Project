#include "../include/Route.h"
#include <cassert>
#include <iostream>

void testRouteCreation() {
    Route route(1, 0, 1, 100.0f, 60.0f, 20);
    assert(route.getId() == 1);
    assert(route.getFromNode() == 0);
    assert(route.getToNode() == 1);
    assert(route.getLength() == 100.0f);
    assert(route.getBaseSpeed() == 60.0f);
    assert(route.getCapacity() == 20);
    std::cout << "Test creation route: OK" << std::endl;
}

void testRouteTraffic() {
    Route route(1, 0, 1, 100.0f, 60.0f, 20);
    
    // Ajout de véhicules
    for (int i = 0; i < 10; i++) {
        route.addVehicle();
    }
    
    assert(route.getVehicleCount() == 10);
    assert(route.getCurrentSpeed() < route.getBaseSpeed());
    
    // Retrait de véhicules
    for (int i = 0; i < 5; i++) {
        route.removeVehicle();
    }
    
    assert(route.getVehicleCount() == 5);
    std::cout << "Test trafic route: OK" << std::endl;
}

void testRouteStates() {
    Route route(1, 0, 1, 100.0f, 60.0f, 20);
    
    route.setState(RouteState::BLOCKED);
    assert(route.getState() == RouteState::BLOCKED);
    assert(!route.isUsable());
    
    route.setState(RouteState::NORMAL);
    assert(route.getState() == RouteState::NORMAL);
    assert(route.isUsable());
    
    std::cout << "Test etats route: OK" << std::endl;
}

void testTravelTime() {
    Route route(1, 0, 1, 100.0f, 60.0f, 20);
    float time = route.getTravelTime();
    assert(time > 0);
    
    route.setState(RouteState::BLOCKED);
    float blockedTime = route.getTravelTime();
    assert(blockedTime > time);
    
    std::cout << "Test temps de parcours: OK" << std::endl;
}

int main() {
    std::cout << "=== Tests Route ===" << std::endl;
    testRouteCreation();
    testRouteTraffic();
    testRouteStates();
    testTravelTime();
    std::cout << "Tous les tests Route sont passes!" << std::endl;
    return 0;
}

