#include "Event.h"
#include <sstream>

Event::Event(int id, EventType type, int routeId, float severity, float duration)
    : id(id), type(type), routeId(routeId), severity(severity),
      duration(duration), elapsedTime(0.0f), active(true) {
}

void Event::update(float deltaTime) {
    if (active) {
        elapsedTime += deltaTime;
        if (elapsedTime >= duration) {
            active = false;
        }
    }
}

void Event::applyToRoute(Route* route) const {
    if (!route || route->getId() != routeId) {
        return;
    }
    
    switch (type) {
        case EventType::ACCIDENT:
            route->setState(RouteState::ACCIDENT);
            break;
        case EventType::TRAFFIC_JAM:
            route->setState(RouteState::CONGESTED);
            route->setCongestion(0.8f + 0.2f * severity);
            break;
        case EventType::ROAD_CLOSURE:
            route->setState(RouteState::BLOCKED);
            break;
        case EventType::EMERGENCY:
            route->setState(RouteState::CONGESTED);
            route->setCongestion(0.6f + 0.3f * severity);
            break;
    }
}

bool Event::isFinished() const {
    return !active;
}

std::string Event::getTypeString() const {
    switch (type) {
        case EventType::ACCIDENT:
            return "Accident";
        case EventType::TRAFFIC_JAM:
            return "Embouteillage";
        case EventType::ROAD_CLOSURE:
            return "Route fermee";
        case EventType::EMERGENCY:
            return "Urgence";
        default:
            return "Inconnu";
    }
}

