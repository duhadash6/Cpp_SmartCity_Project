/**
 * @file Event.h
 * @brief Représentation d'un événement affectant le trafic
 * 
 * Cette classe modélise les événements qui peuvent affecter le réseau routier
 * (accidents, embouteillages, fermetures de route, urgences).
 */

#ifndef EVENT_H
#define EVENT_H

#include "Route.h"
#include <string>
#include <chrono>

/**
 * @enum EventType
 * @brief Types d'événements pouvant affecter le trafic
 */
enum class EventType {
    ACCIDENT,        ///< Accident sur la route
    TRAFFIC_JAM,     ///< Embouteillage
    ROAD_CLOSURE,    ///< Fermeture de route
    EMERGENCY        ///< Urgence (véhicule prioritaire)
};

/**
 * @class Event
 * @brief Classe représentant un événement qui affecte le trafic
 * 
 * Modélise un événement temporaire qui modifie l'état d'une route
 * et affecte la circulation des véhicules.
 */
class Event {
private:
    int id;
    EventType type;
    int routeId;           // Route affectée
    float severity;        // Sévérité (0.0 à 1.0)
    float duration;        // Durée en secondes
    float elapsedTime;     // Temps écoulé depuis le début
    bool active;
    
public:
    Event(int id, EventType type, int routeId, float severity, float duration);
    
    // Getters
    int getId() const { return id; }
    EventType getType() const { return type; }
    int getRouteId() const { return routeId; }
    float getSeverity() const { return severity; }
    float getDuration() const { return duration; }
    float getElapsedTime() const { return elapsedTime; }
    bool isActive() const { return active; }
    
    // Mise à jour
    void update(float deltaTime);
    
    // Application de l'événement à une route
    void applyToRoute(Route* route) const;
    
    // Vérification si l'événement est terminé
    bool isFinished() const;
    
    // String representation
    std::string getTypeString() const;
};

#endif // EVENT_H

