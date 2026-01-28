#include "Factory.h"
#include <random>

// Initialisation des IDs statiques
int EventFactory::nextEventId = 0;
int VehicleFactory::nextVehicleId = 0;

EventFactory::EventFactory() : rng(std::random_device{}()) {
}

std::unique_ptr<Event> EventFactory::createRandomEvent(int routeId) {
    // Génération aléatoire du type d'événement
    std::uniform_int_distribution<int> typeDist(0, 3);
    EventType type = static_cast<EventType>(typeDist(rng));
    
    // Génération aléatoire de la sévérité (0.5 à 1.0)
    std::uniform_real_distribution<float> severityDist(0.5f, 1.0f);
    float severity = severityDist(rng);
    
    // Génération aléatoire de la durée (10 à 60 secondes)
    std::uniform_real_distribution<float> durationDist(10.0f, 60.0f);
    float duration = durationDist(rng);
    
    return createEvent(type, routeId, severity, duration);
}

std::unique_ptr<Event> EventFactory::createEvent(EventType type, int routeId, 
                                                  float severity, float duration) {
    int id = nextEventId++;
    return std::make_unique<Event>(id, type, routeId, severity, duration);
}

VehicleFactory::VehicleFactory() : rng(std::random_device{}()) {
}

std::unique_ptr<Vehicle> VehicleFactory::createVehicle(int startNode, int targetNode) {
    // Génération aléatoire du type de véhicule
    std::uniform_int_distribution<int> typeDist(0, 2);
    int vehicleType = typeDist(rng);
    
    return createVehicle(startNode, targetNode, vehicleType);
}

std::unique_ptr<Vehicle> VehicleFactory::createVehicle(int startNode, int targetNode, int vehicleType) {
    int id = nextVehicleId++;
    auto vehicle = std::make_unique<Vehicle>(id, startNode, targetNode);
    // Note: Le type de véhicule est déterminé par l'ID dans le constructeur Vehicle
    // Pour une implémentation complète, il faudrait ajouter un setter dans Vehicle
    return vehicle;
}

