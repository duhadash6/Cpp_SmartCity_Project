#ifndef FACTORY_H
#define FACTORY_H

/**
 * @file Factory.h
 * @brief Pattern Factory pour créer des Event et Vehicle
 * 
 * Ce fichier implémente le design pattern Factory pour centraliser
 * la création d'objets Event et Vehicle, facilitant l'extension
 * et la maintenance du code.
 */

#include "Event.h"
#include "Vehicle.h"
#include <memory>
#include <random>

/**
 * @class EventFactory
 * @brief Factory pour créer des événements de différents types
 * 
 * Implémente le pattern Factory pour la création d'événements.
 * Permet de créer des événements avec des paramètres aléatoires
 * ou personnalisés.
 */
class EventFactory {
private:
    static int nextEventId;
    std::mt19937 rng;
    
public:
    /**
     * @brief Constructeur de la factory
     */
    EventFactory();
    
    /**
     * @brief Crée un événement aléatoire
     * @param routeId ID de la route affectée
     * @return Pointeur unique vers l'événement créé
     */
    std::unique_ptr<Event> createRandomEvent(int routeId);
    
    /**
     * @brief Crée un événement de type spécifique
     * @param type Type d'événement à créer
     * @param routeId ID de la route affectée
     * @param severity Sévérité (0.0 à 1.0)
     * @param duration Durée en secondes
     * @return Pointeur unique vers l'événement créé
     */
    std::unique_ptr<Event> createEvent(EventType type, int routeId, 
                                       float severity, float duration);
};

/**
 * @class VehicleFactory
 * @brief Factory pour créer des véhicules de différents types
 * 
 * Implémente le pattern Factory pour la création de véhicules.
 * Permet de créer des véhicules avec des caractéristiques variées.
 */
class VehicleFactory {
private:
    static int nextVehicleId;
    std::mt19937 rng;
    
public:
    /**
     * @brief Constructeur de la factory
     */
    VehicleFactory();
    
    /**
     * @brief Crée un véhicule avec un point de départ et d'arrivée
     * @param startNode Nœud de départ
     * @param targetNode Nœud de destination
     * @return Pointeur unique vers le véhicule créé
     */
    std::unique_ptr<Vehicle> createVehicle(int startNode, int targetNode);
    
    /**
     * @brief Crée un véhicule avec un type spécifique
     * @param startNode Nœud de départ
     * @param targetNode Nœud de destination
     * @param vehicleType Type de véhicule (0=voiture, 1=camion, 2=bus)
     * @return Pointeur unique vers le véhicule créé
     */
    std::unique_ptr<Vehicle> createVehicle(int startNode, int targetNode, int vehicleType);
};

#endif // FACTORY_H

