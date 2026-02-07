#include "../include/Event.h"
#include "../include/Route.h"
#include <cassert>
#include <iostream>

void testEventCreation() {
    Event event(0, EventType::ACCIDENT, 1, 0.8f, 30.0f);
    assert(event.getId() == 0);
    assert(event.getType() == EventType::ACCIDENT);
    assert(event.getRouteId() == 1);
    assert(event.getSeverity() == 0.8f);
    assert(event.isActive());
    std::cout << "Test creation evenement: OK" << std::endl;
}

void testEventUpdate() {
    Event event(0, EventType::TRAFFIC_JAM, 1, 0.7f, 10.0f);
    
    // Mise à jour pendant 5 secondes
    event.update(5.0f);
    assert(event.isActive());
    
    // Mise à jour pour terminer l'événement
    event.update(10.0f);
    assert(event.isFinished());
    
    std::cout << "Test mise a jour evenement: OK" << std::endl;
}

void testEventApplyToRoute() {
    Route route(1, 0, 1, 100.0f, 60.0f, 20);
    Event event(0, EventType::ACCIDENT, 1, 0.8f, 30.0f);
    
    event.applyToRoute(&route);
    assert(route.getState() == RouteState::ACCIDENT);
    assert(!route.isUsable());
    
    std::cout << "Test application evenement: OK" << std::endl;
}

int main() {
    std::cout << "=== Tests Event ===" << std::endl;
    testEventCreation();
    testEventUpdate();
    testEventApplyToRoute();
    std::cout << "Tous les tests Event sont passes!" << std::endl;
    return 0;
}

