/**
 * @file Simulation.h
 * @brief Classe principale de gestion de la simulation de routage dynamique
 * 
 * Cette classe orchestre toute la simulation : graphe, véhicules, événements,
 * planification de trajets et reroutage dynamique.
 */

#ifndef SIMULATION_H
#define SIMULATION_H

#include "Graph.h"
#include "Vehicle.h"
#include "PathPlanner.h"
#include "Event.h"
#include <vector>
#include <memory>
#include <random>

/**
 * @enum SimulationMode
 * @brief Mode de fonctionnement de la simulation
 */
enum class SimulationMode {
    NORMAL,         ///< Mode normal sans reroutage dynamique
    DYNAMIC         ///< Mode dynamique avec reroutage automatique
};

/**
 * @class Simulation
 * @brief Classe principale de gestion de la simulation
 * 
 * Gère l'ensemble de la simulation : réseau routier, véhicules,
 * événements, planification et reroutage. Supporte deux modes :
 * normal (sans reroutage) et dynamique (avec reroutage automatique).
 */
class Simulation {
private:
    std::unique_ptr<Graph> graph;
    std::unique_ptr<PathPlanner> pathPlanner;
    std::vector<std::unique_ptr<Vehicle>> vehicles;
    std::vector<std::unique_ptr<Event>> events;
    
    SimulationMode mode;
    float simulationTime;
    float timeScale;        // Facteur d'accélération du temps
    bool isPaused;          // État pause/play
    
    // Paramètres configurables
    int vehicleCount;
    int eventCount;
    bool reroutingEnabled;
    
    // Gestion des événements
    std::mt19937 rng;
    float nextEventTime;
    float eventInterval;
    
    // Statistiques
    int totalReroutings;
    float averageTravelTime;
    
public:
    Simulation();
    ~Simulation();
    
    // Initialisation
    void initialize(const std::string& configPath);
    void setMode(SimulationMode mode);
    void setVehicleCount(int count);
    void setEventCount(int count);
    void setPaused(bool paused) { isPaused = paused; }
    void togglePause() { isPaused = !isPaused; }
    bool getIsPaused() const { return isPaused; }
    void setTimeScale(float scale) { timeScale = std::max(0.1f, std::min(5.0f, scale)); }
    float getTimeScale() const { return timeScale; }
    
    // Mise à jour de la simulation
    void update(float deltaTime);
    
    // Gestion des événements
    void triggerRandomEvent();
    void addEvent(EventType type, int routeId, float severity, float duration);
    
    // Reroutage des véhicules affectés
    void rerouteAffectedVehicles(int routeId);
    
    // Getters
    const Graph* getGraph() const { return graph.get(); }
    const std::vector<std::unique_ptr<Vehicle>>& getVehicles() const { return vehicles; }
    const std::vector<std::unique_ptr<Event>>& getEvents() const { return events; }
    SimulationMode getMode() const { return mode; }
    float getSimulationTime() const { return simulationTime; }
    int getTotalReroutings() const { return totalReroutings; }
    
    // Statistiques
    void updateStatistics();
    void printStatistics() const;
    
private:
    // Méthodes privées
    void createTestGraph();
    void createVehicles();
};

#endif // SIMULATION_H

