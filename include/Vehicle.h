/**
 * @file Vehicle.h
 * @brief Représentation d'un véhicule dans la simulation
 * 
 * Cette classe modélise un véhicule qui se déplace dans le réseau routier,
 * suit un chemin planifié et peut demander un reroutage dynamique.
 */

#ifndef VEHICLE_H
#define VEHICLE_H

#include "Route.h"
#include <vector>
#include <memory>

/**
 * @class Vehicle
 * @brief Classe représentant un véhicule dans la simulation
 * 
 * Gère le déplacement du véhicule, sa position, son chemin planifié
 * et la détection de la nécessité d'un reroutage.
 */
class Vehicle {
private:
    int id;
    int currentNode;           // Nœud actuel
    int targetNode;             // Nœud de destination
    std::vector<int> path;      // Chemin planifié
    int currentRouteIndex;      // Index dans le chemin
    float progress;             // Progression sur la route actuelle (0.0 à 1.0)
    float speed;                // Vitesse actuelle
    bool needsRerouting;        // Flag indiquant si un reroutage est nécessaire
    
    // Position pour la visualisation
    float x, y;
    float angle;                // Angle de direction (en radians)
    int vehicleType;            // Type de véhicule (0=voiture, 1=camion, 2=bus)
    
public:
    Vehicle(int id, int startNode, int targetNode);
    
    // Getters
    int getId() const { return id; }
    int getCurrentNode() const { return currentNode; }
    int getTargetNode() const { return targetNode; }
    float getX() const { return x; }
    float getY() const { return y; }
    float getAngle() const { return angle; }
    int getVehicleType() const { return vehicleType; }
    bool needsReroutingCheck() const { return needsRerouting; }
    
    // Planification de trajet
    void setPath(const std::vector<int>& newPath);
    const std::vector<int>& getPath() const { return path; }
    
    // Mise à jour de la position
    void update(float deltaTime, const class Graph& graph);
    
    // Reroutage
    void requestRerouting() { needsRerouting = true; }
    void clearReroutingFlag() { needsRerouting = false; }
    
    // Vérification si le véhicule a atteint sa destination
    bool hasReachedDestination() const;
    
    // Calcul de la position visuelle
    void calculatePosition(const class Graph& graph);
};

#endif // VEHICLE_H

