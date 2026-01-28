#include "Vehicle.h"
#include "Graph.h"
#include <algorithm>
#include <cmath>

#ifndef PI
#define PI 3.14159265359f
#endif

Vehicle::Vehicle(int id, int startNode, int targetNode)
    : id(id), currentNode(startNode), targetNode(targetNode),
      currentRouteIndex(0), progress(0.0f), speed(50.0f),
      needsRerouting(false), x(0.0f), y(0.0f), angle(0.0f),
      vehicleType(id % 3) { // 3 types de véhicules différents (0=voiture, 1=camion, 2=bus)
    // Initialiser la position au nœud de départ
    // (sera calculée dans calculatePosition)
}

void Vehicle::setPath(const std::vector<int>& newPath) {
    path = newPath;
    currentRouteIndex = 0;
    progress = 0.0f;
    needsRerouting = false;
}

void Vehicle::update(float deltaTime, const Graph& graph) {
    // Toujours calculer la position même si en pause (pour le rendu)
    calculatePosition(graph);
    
    if (path.empty() || hasReachedDestination()) {
        return;
    }
    
    if (currentRouteIndex >= static_cast<int>(path.size()) - 1) {
        // Arrivé à destination
        currentNode = targetNode;
        return;
    }
    
    // Vérifier si une route future dans le chemin est bloquée ou accidentée
    // Cela permet de détecter les accidents avant d'y arriver
    for (size_t i = currentRouteIndex; i < path.size() - 1; i++) {
        int fromNode = path[i];
        int toNode = path[i + 1];
        
        Route* route = nullptr;
        for (const auto& r : graph.getRoutes()) {
            if ((r->getFromNode() == fromNode && r->getToNode() == toNode) ||
                (r->getFromNode() == toNode && r->getToNode() == fromNode)) {
                route = r.get();
                break;
            }
        }
        
        if (route && (route->getState() == RouteState::BLOCKED || 
                      route->getState() == RouteState::ACCIDENT ||
                      !route->isUsable())) {
            // Route bloquée ou accidentée dans le chemin - demander reroutage
            needsRerouting = true;
            // Si c'est la route actuelle, arrêter immédiatement
            if (i == static_cast<size_t>(currentRouteIndex)) {
                return; // Arrêter le véhicule
            }
            // Sinon, continuer vers la route bloquée mais demander reroutage
            break;
        }
    }
    
    int fromNode = path[currentRouteIndex];
    int toNode = path[currentRouteIndex + 1];
    
    // Trouver la route entre ces deux nœuds
    Route* currentRoute = nullptr;
    for (const auto& route : graph.getRoutes()) {
        if ((route->getFromNode() == fromNode && route->getToNode() == toNode) ||
            (route->getFromNode() == toNode && route->getToNode() == fromNode)) {
            currentRoute = route.get();
            break;
        }
    }
    
    if (!currentRoute) {
        // Route non trouvée - essayer de passer à la suivante
        progress = 0.0f;
        currentRouteIndex++;
        if (currentRouteIndex < static_cast<int>(path.size())) {
            currentNode = path[currentRouteIndex];
        }
        return;
    }
    
    // Vérifier l'état de la route actuelle
    if (!currentRoute->isUsable() || 
        currentRoute->getState() == RouteState::BLOCKED || 
        currentRoute->getState() == RouteState::ACCIDENT) {
        // Route bloquée ou accidentée - ARRÊTER le véhicule et demander reroutage
        needsRerouting = true;
        // Le véhicule s'arrête (ne progresse plus)
        return;
    }
    
    // Mise à jour de la progression
    float routeLength = currentRoute->getLength();
    float routeSpeed = currentRoute->getCurrentSpeed();
    
    // Vérifier que les valeurs sont valides
    if (routeLength <= 0.0f || !std::isfinite(routeLength)) {
        // Route invalide, passer à la suivante
        progress = 0.0f;
        currentRouteIndex++;
        if (currentRouteIndex < static_cast<int>(path.size())) {
            currentNode = path[currentRouteIndex];
        }
        return;
    }
    
    // Utiliser la vitesse de la route ou la vitesse du véhicule
    float effectiveSpeed = (routeSpeed > 0 && std::isfinite(routeSpeed)) ? routeSpeed : speed;
    
    // S'assurer qu'on a une vitesse valide
    if (effectiveSpeed <= 0 || !std::isfinite(effectiveSpeed)) {
        effectiveSpeed = 30.0f; // Vitesse par défaut si problème
    }
    
    if (deltaTime > 0 && std::isfinite(deltaTime)) {
        // Conversion km/h -> m/s puis calcul de la distance
        float distance = (effectiveSpeed / 3.6f) * deltaTime;
        
        if (std::isfinite(distance) && distance > 0 && std::isfinite(routeLength) && routeLength > 0) {
            progress += distance / routeLength;
            
            // Limiter progress entre 0 et 1
            if (progress >= 1.0f) {
                // Route terminée, passer à la suivante
                progress = 0.0f;
                currentRouteIndex++;
                currentNode = toNode;
                
                if (currentRouteIndex >= static_cast<int>(path.size()) - 1) {
                    currentNode = targetNode;
                }
            } else if (progress < 0.0f) {
                progress = 0.0f;
            }
        }
    }
}

void Vehicle::calculatePosition(const Graph& graph) {
    if (path.empty() || currentRouteIndex >= static_cast<int>(path.size()) - 1) {
        Node* node = graph.getNode(currentNode);
        if (node) {
            x = node->x;
            y = node->y;
        }
        return;
    }
    
    int fromNodeId = path[currentRouteIndex];
    int toNodeId = path[currentRouteIndex + 1];
    
    Node* fromNode = graph.getNode(fromNodeId);
    Node* toNode = graph.getNode(toNodeId);
    
    if (fromNode && toNode) {
        // Vérifier que les coordonnées sont valides
        if (std::isfinite(fromNode->x) && std::isfinite(fromNode->y) &&
            std::isfinite(toNode->x) && std::isfinite(toNode->y)) {
            
            // Interpolation linéaire entre les deux nœuds
            x = fromNode->x + (toNode->x - fromNode->x) * progress;
            y = fromNode->y + (toNode->y - fromNode->y) * progress;
            
            // Calculer l'angle de direction avec lissage pour rotation fluide
            float dx = toNode->x - fromNode->x;
            float dy = toNode->y - fromNode->y;
            float targetAngle = std::atan2(dy, dx);
            
            // Lissage de la rotation pour éviter les changements brusques
            float angleDiff = targetAngle - angle;
            // Normaliser l'angle entre -PI et PI
            while (angleDiff > PI) angleDiff -= 2.0f * PI;
            while (angleDiff < -PI) angleDiff += 2.0f * PI;
            
            // Interpolation angulaire pour rotation fluide
            float rotationSpeed = 3.0f; // Vitesse de rotation (rad/s)
            float maxRotation = rotationSpeed * 0.016f; // Limite par frame (60 FPS)
            
            if (std::abs(angleDiff) > maxRotation) {
                angle += (angleDiff > 0 ? maxRotation : -maxRotation);
            } else {
                angle = targetAngle;
            }
            
            // Normaliser l'angle entre 0 et 2*PI
            while (angle < 0) angle += 2.0f * PI;
            while (angle >= 2.0f * PI) angle -= 2.0f * PI;
            
            // Vérifier que le résultat est valide
            if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(angle)) {
                // Utiliser la position du nœud de départ en cas d'erreur
                x = fromNode->x;
                y = fromNode->y;
                angle = std::atan2(dy, dx);
            }
        } else {
            // Utiliser la position du nœud actuel
            Node* currentNode = graph.getNode(this->currentNode);
            if (currentNode) {
                x = currentNode->x;
                y = currentNode->y;
            }
        }
    }
}

bool Vehicle::hasReachedDestination() const {
    return currentNode == targetNode && (path.empty() || currentRouteIndex >= static_cast<int>(path.size()) - 1);
}

