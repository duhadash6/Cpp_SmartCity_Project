/**
 * @file Route.h
 * @brief Représentation d'une route dans le réseau routier
 * 
 * Cette classe modélise une route (arête) dans le graphe du réseau routier.
 * Elle gère le trafic, les états de la route et calcule les temps de parcours.
 */

#ifndef ROUTE_H
#define ROUTE_H

#include <string>
#include <vector>
#include <memory>

/**
 * @enum RouteState
 * @brief État d'une route dans le réseau
 */
enum class RouteState {
    NORMAL,      ///< Route normale, circulation fluide
    CONGESTED,   ///< Route congestionnée, circulation ralentie
    BLOCKED,     ///< Route bloquée, circulation impossible
    ACCIDENT     ///< Accident sur la route
};

/**
 * @class Route
 * @brief Classe représentant une route/arête dans le graphe
 * 
 * Modélise une route entre deux nœuds avec gestion du trafic,
 * des états et calcul des temps de parcours dynamiques.
 */
class Route {
private:
    int id;
    int fromNode;
    int toNode;
    float length;           // Longueur de la route
    float baseSpeed;        // Vitesse de base (km/h)
    float currentSpeed;     // Vitesse actuelle (prend en compte le trafic)
    int vehicleCount;       // Nombre de véhicules actuellement sur la route
    int capacity;           // Capacité maximale de véhicules
    RouteState state;       // État de la route
    
public:
    Route(int id, int from, int to, float len, float speed, int cap);
    
    // Getters
    int getId() const { return id; }
    int getFromNode() const { return fromNode; }
    int getToNode() const { return toNode; }
    float getLength() const { return length; }
    float getBaseSpeed() const { return baseSpeed; }
    float getCurrentSpeed() const { return currentSpeed; }
    int getVehicleCount() const { return vehicleCount; }
    int getCapacity() const { return capacity; }
    RouteState getState() const { return state; }
    
    // Calcul du temps de parcours
    float getTravelTime() const;
    
    // Gestion du trafic
    void addVehicle();
    void removeVehicle();
    void updateSpeed();
    
    // Gestion des événements
    void setState(RouteState newState);
    void setCongestion(float congestionLevel); // 0.0 à 1.0
    
    // Vérification si la route est utilisable
    bool isUsable() const;
};

#endif // ROUTE_H

