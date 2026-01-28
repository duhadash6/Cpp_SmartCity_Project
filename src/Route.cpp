#include "Route.h"
#include <algorithm>

Route::Route(int id, int from, int to, float len, float speed, int cap)
    : id(id), fromNode(from), toNode(to), length(len), 
      baseSpeed(speed), currentSpeed(speed), vehicleCount(0), 
      capacity(cap), state(RouteState::NORMAL) {
}

float Route::getTravelTime() const {
    // Routes bloquées ou accidentées ont un coût très élevé pour être évitées
    if (state == RouteState::BLOCKED || state == RouteState::ACCIDENT) {
        return 999999.0f; // Très grand nombre pour forcer l'évitement
    }
    if (currentSpeed <= 0 || !isUsable()) {
        return 999999.0f; // Très grand nombre pour les routes inutilisables
    }
    
    // Temps de parcours de base (en secondes)
    float baseTravelTime = (length / currentSpeed) * 3600.0f;
    
    // Load balancing: pénalité basée sur la congestion pour éviter que tous prennent la même route
    // Utilise la vitesse actuelle comme indicateur de congestion (vitesse réduite = congestion)
    float loadPenalty = 0.0f;
    if (baseSpeed > 0 && currentSpeed < baseSpeed) {
        // Plus la vitesse est réduite, plus la route est congestionnée
        float congestionLevel = 1.0f - (currentSpeed / baseSpeed);
        // Pénalité progressive: congestion légère (+10%), moyenne (+50%), forte (+150%)
        loadPenalty = baseTravelTime * congestionLevel * congestionLevel * 1.5f;
    }
    
    // Pénalité supplémentaire pour routes congestionnées explicitement
    if (state == RouteState::CONGESTED) {
        loadPenalty += baseTravelTime * 0.5f; // +50% supplémentaire
    }
    
    return baseTravelTime + loadPenalty;
}

void Route::addVehicle() {
    vehicleCount++;
    updateSpeed();
}

void Route::removeVehicle() {
    if (vehicleCount > 0) {
        vehicleCount--;
        updateSpeed();
    }
}

void Route::updateSpeed() {
    if (state == RouteState::BLOCKED || state == RouteState::ACCIDENT) {
        currentSpeed = 0;
        return;
    }
    
    // Calcul de la vitesse basée sur la congestion
    float congestionRatio = static_cast<float>(vehicleCount) / static_cast<float>(capacity);
    congestionRatio = std::min(congestionRatio, 1.0f);
    
    // Réduction de vitesse proportionnelle à la congestion
    // À 100% de capacité, la vitesse est réduite à 20% de la vitesse de base
    currentSpeed = baseSpeed * (1.0f - 0.8f * congestionRatio);
    
    if (state == RouteState::CONGESTED) {
        currentSpeed *= 0.5f; // Réduction supplémentaire pour congestion
    }
    
    currentSpeed = std::max(currentSpeed, 5.0f); // Vitesse minimale
}

void Route::setState(RouteState newState) {
    state = newState;
    updateSpeed();
}

void Route::setCongestion(float congestionLevel) {
    congestionLevel = std::clamp(congestionLevel, 0.0f, 1.0f);
    vehicleCount = static_cast<int>(capacity * congestionLevel);
    if (congestionLevel > 0.7f) {
        state = RouteState::CONGESTED;
    } else if (state == RouteState::CONGESTED) {
        state = RouteState::NORMAL;
    }
    updateSpeed();
}

bool Route::isUsable() const {
    // Route inutilisable si bloquée, accidentée, ou vitesse nulle
    return state != RouteState::BLOCKED && 
           state != RouteState::ACCIDENT && 
           currentSpeed > 0;
}

