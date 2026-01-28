#include "Simulation.h"
#include "Graph.h"
#include "Vehicle.h"
#include "PathPlanner.h"
#include "Event.h"
#include <random>
#include <algorithm>
#include <iostream>

Simulation::Simulation()
    : mode(SimulationMode::DYNAMIC), simulationTime(0.0f), timeScale(1.0f),
      isPaused(false),  // Initialiser isPaused à false
      vehicleCount(50), eventCount(2), reroutingEnabled(true),  // Plus de véhicules pour ville dynamique
      rng(std::random_device{}()), nextEventTime(10.0f), eventInterval(20.0f),  // Événements plus fréquents
      totalReroutings(0), averageTravelTime(0.0f) {
    
    graph = std::make_unique<Graph>();
    pathPlanner = std::make_unique<PathPlanner>(graph.get());
}

Simulation::~Simulation() {
}

void Simulation::initialize(const std::string& configPath) {
    // Création d'un graphe de test si pas de config
    if (configPath.empty()) {
        createTestGraph();
    } else {
        graph->loadFromConfig(configPath);
    }
    
    // Création des véhicules
    createVehicles();
}

void Simulation::createTestGraph() {
    // Création d'un réseau routier organisé avec structure claire
    // Structure : Grille principale avec quelques rond-points bien organisés au centre
    float spacing = 130.0f; // Espacement entre nœuds
    int nodeId = 0;
    int routeId = 0;
    
    // Créer une grille principale 6x6 pour structure claire et organisée
    int gridSize = 6;
    std::vector<std::vector<int>> nodeGrid(gridSize, std::vector<int>(gridSize, -1));
    
    // 1. Créer les nœuds en grille régulière
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            nodeGrid[i][j] = nodeId;
            graph->addNode(nodeId++, j * spacing, i * spacing);
        }
    }
    
    // 2. Créer les routes principales horizontales (toutes les routes)
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize - 1; j++) {
            int from = nodeGrid[i][j];
            int to = nodeGrid[i][j + 1];
            graph->addRoute(routeId++, from, to, spacing, 60.0f, 30);
        }
    }
    
    // 3. Créer les routes principales verticales (toutes les routes)
    for (int i = 0; i < gridSize - 1; i++) {
        for (int j = 0; j < gridSize; j++) {
            int from = nodeGrid[i][j];
            int to = nodeGrid[i + 1][j];
            graph->addRoute(routeId++, from, to, spacing, 60.0f, 30);
        }
    }
}

void Simulation::createVehicles() {
    vehicles.clear();
    
    const auto& nodes = graph->getNodes();
    std::cout << "Nombre de noeuds dans le graphe: " << nodes.size() << std::endl;
    if (nodes.size() < 2) {
        std::cout << "ERREUR: Pas assez de noeuds pour creer des vehicules!" << std::endl;
        return;
    }
    
    // Trouver les nœuds qui ont des connexions (routes)
    std::vector<int> connectedNodes;
    for (const auto& node : nodes) {
        std::vector<int> routes = graph->getRoutesFromNode(node->id);
        if (!routes.empty()) {
            connectedNodes.push_back(node->id);
        }
    }
    
    if (connectedNodes.empty()) {
        std::cout << "ERREUR: Aucun noeud connecte dans le graphe!" << std::endl;
        return;
    }
    
    std::cout << "Noeuds connectes: " << connectedNodes.size() << " sur " << nodes.size() << std::endl;
    
    std::uniform_int_distribution<int> nodeDist(0, connectedNodes.size() - 1);
    int vehiclesCreated = 0;
    int maxAttempts = vehicleCount * 10; // Maximum de tentatives
    int attempts = 0;
    
    for (int i = 0; i < vehicleCount && attempts < maxAttempts; i++) {
        attempts++;
        
        int startIdx = nodeDist(rng);
        int endIdx = nodeDist(rng);
        
        // S'assurer que start != end
        while (endIdx == startIdx && connectedNodes.size() > 1) {
            endIdx = nodeDist(rng);
        }
        
        int start = connectedNodes[startIdx];
        int end = connectedNodes[endIdx];
        
        // Planifier le chemin AVANT de créer le véhicule
        std::vector<int> path = pathPlanner->planPath(start, end);
        
        if (path.empty() || path.size() < 2) {
            // Essayer un autre nœud de destination
            for (int retry = 0; retry < 5 && path.empty(); retry++) {
                endIdx = nodeDist(rng);
                while (endIdx == startIdx && connectedNodes.size() > 1) {
                    endIdx = nodeDist(rng);
                }
                end = connectedNodes[endIdx];
                path = pathPlanner->planPath(start, end);
            }
        }
        
        if (!path.empty() && path.size() >= 2) {
            auto vehicle = std::make_unique<Vehicle>(vehiclesCreated, start, end);
            vehicle->setPath(path);
            // Initialiser la position au nœud de départ
            Node* startNode = graph->getNode(start);
            if (startNode) {
                vehicle->calculatePosition(*graph);
            }
            vehicles.push_back(std::move(vehicle));
            vehiclesCreated++;
        } else {
            std::cout << "ATTENTION: Pas de chemin trouve pour vehicule " << i 
                      << " (de " << start << " a " << end << ")" << std::endl;
        }
    }
    
    std::cout << "Vehicules crees avec succes: " << vehiclesCreated << " sur " << vehicleCount << " demandes" << std::endl;
}

void Simulation::setMode(SimulationMode mode) {
    this->mode = mode;
    reroutingEnabled = (mode == SimulationMode::DYNAMIC);
}

void Simulation::setVehicleCount(int count) {
    vehicleCount = count;
    createVehicles();
}

void Simulation::setEventCount(int count) {
    eventCount = count;
}

void Simulation::update(float deltaTime) {
    // Toujours mettre à jour le temps et les événements (même en pause pour l'affichage)
    // Mais ne pas faire avancer la simulation si en pause
    if (!isPaused) {
        simulationTime += deltaTime * timeScale;
    }
    
    // Mise à jour des événements (seulement si pas en pause)
    if (!isPaused) {
        for (auto& event : events) {
            if (!event) continue;
            
            try {
                event->update(deltaTime);
                
                Route* route = graph->getRoute(event->getRouteId());
                if (route && event->isActive()) {
                    event->applyToRoute(route);
                } else if (route && event->isFinished()) {
                    // Réinitialiser la route
                    route->setState(RouteState::NORMAL);
                }
            } catch (...) {
                // Ignorer les erreurs d'événement
                continue;
            }
        }
        
        // Suppression des événements terminés
        events.erase(
            std::remove_if(events.begin(), events.end(),
                [](const std::unique_ptr<Event>& e) { return e->isFinished(); }),
            events.end()
        );
        
        // Déclenchement d'événements aléatoires
        if (simulationTime >= nextEventTime && static_cast<int>(events.size()) < eventCount) {
            triggerRandomEvent();
            nextEventTime = simulationTime + eventInterval;
        }
    }
    
    // Mise à jour des véhicules (TOUJOURS, même en pause pour le rendu)
    for (auto& vehicle : vehicles) {
        if (!vehicle) continue;
        
        if (vehicle->hasReachedDestination()) {
            continue;
        }
        
        try {
            // Toujours mettre à jour les véhicules (même en pause pour le rendu)
            // Mais seulement avancer si pas en pause
            float effectiveDeltaTime = isPaused ? 0.0f : deltaTime;
            vehicle->update(effectiveDeltaTime, *graph);
            
            // Vérifier si reroutage nécessaire (seulement si pas en pause)
            if (!isPaused && reroutingEnabled && vehicle->needsReroutingCheck()) {
                int currentPos = vehicle->getCurrentNode();
                int target = vehicle->getTargetNode();
                
                if (currentPos >= 0 && target >= 0) {
                    std::vector<int> newPath = pathPlanner->replanPath(
                        currentPos, target, vehicle->getPath(), currentPos);
                    
                    if (!newPath.empty()) {
                        vehicle->setPath(newPath);
                        vehicle->clearReroutingFlag();
                        totalReroutings++;
                    } else {
                        // Pas de chemin alternatif trouvé - le véhicule reste arrêté
                        // Le flag needsRerouting reste à true pour réessayer plus tard
                    }
                }
            }
        } catch (const std::exception& e) {
            // Log l'erreur pour debug
            std::cout << "ERREUR dans vehicle->update: " << e.what() << std::endl;
            continue;
        } catch (...) {
            // Ignorer les erreurs de mise à jour d'un véhicule
            continue;
        }
    }
    
    // Mise à jour du trafic (seulement si pas en pause)
    if (!isPaused) {
        graph->updateTraffic();
    }
    
    // Supprimer les véhicules arrivés et créer de nouveaux véhicules
    if (!isPaused) {
        vehicles.erase(
            std::remove_if(vehicles.begin(), vehicles.end(),
                [](const std::unique_ptr<Vehicle>& v) { return v->hasReachedDestination(); }),
            vehicles.end()
        );
        
        // Créer de nouveaux véhicules pour maintenir le nombre cible
        while (static_cast<int>(vehicles.size()) < vehicleCount) {
            const auto& nodes = graph->getNodes();
            if (nodes.size() < 2) break;
            
            // Trouver les nœuds qui ont des connexions
            std::vector<int> connectedNodes;
            for (const auto& node : nodes) {
                std::vector<int> routes = graph->getRoutesFromNode(node->id);
                if (!routes.empty()) {
                    connectedNodes.push_back(node->id);
                }
            }
            
            if (connectedNodes.empty()) break;
            
            std::uniform_int_distribution<int> nodeDist(0, connectedNodes.size() - 1);
            int startIdx = nodeDist(rng);
            int endIdx = nodeDist(rng);
            
            // S'assurer que start != end
            int attempts = 0;
            while (endIdx == startIdx && connectedNodes.size() > 1 && attempts < 10) {
                endIdx = nodeDist(rng);
                attempts++;
            }
            
            if (endIdx == startIdx) break; // Impossible de trouver deux nœuds différents
            
            int start = connectedNodes[startIdx];
            int end = connectedNodes[endIdx];
            
            // Planifier le chemin
            std::vector<int> path = pathPlanner->planPath(start, end);
            
            if (!path.empty() && path.size() >= 2) {
                int newId = vehicles.empty() ? 0 : vehicles.back()->getId() + 1;
                auto vehicle = std::make_unique<Vehicle>(newId, start, end);
                vehicle->setPath(path);
                Node* startNode = graph->getNode(start);
                if (startNode) {
                    vehicle->calculatePosition(*graph);
                }
                vehicles.push_back(std::move(vehicle));
            } else {
                break; // Pas de chemin trouvé, arrêter d'essayer
            }
        }
    }
    
    // Mise à jour des statistiques
    updateStatistics();
}

void Simulation::triggerRandomEvent() {
    const auto& routes = graph->getRoutes();
    if (routes.empty()) return;
    
    std::uniform_int_distribution<int> routeDist(0, routes.size() - 1);
    std::uniform_real_distribution<float> severityDist(0.5f, 1.0f);
    std::uniform_real_distribution<float> durationDist(20.0f, 60.0f);
    
    int routeId = routeDist(rng);
    float severity = severityDist(rng);
    float duration = durationDist(rng);
    
    EventType type = static_cast<EventType>(
        std::uniform_int_distribution<int>(0, 3)(rng));
    
    addEvent(type, routeId, severity, duration);
}

void Simulation::addEvent(EventType type, int routeId, float severity, float duration) {
    int eventId = events.size();
    auto event = std::make_unique<Event>(eventId, type, routeId, severity, duration);
    
    Route* route = graph->getRoute(routeId);
    if (route) {
        event->applyToRoute(route);
    }
    
    events.push_back(std::move(event));
    
    // Reroutage des véhicules affectés
    if (reroutingEnabled) {
        rerouteAffectedVehicles(routeId);
    }
}

void Simulation::rerouteAffectedVehicles(int routeId) {
    for (auto& vehicle : vehicles) {
        if (vehicle->hasReachedDestination()) {
            continue;
        }
        
        const auto& path = vehicle->getPath();
        // Vérifier si le véhicule utilise ou va utiliser cette route
        bool affected = false;
        for (size_t i = 0; i < path.size() - 1; i++) {
            int from = path[i];
            int to = path[i + 1];
            
            Route* route = nullptr;
            for (const auto& r : graph->getRoutes()) {
                if ((r->getFromNode() == from && r->getToNode() == to) ||
                    (r->getFromNode() == to && r->getToNode() == from)) {
                    route = r.get();
                    break;
                }
            }
            
            if (route && route->getId() == routeId) {
                affected = true;
                break;
            }
        }
        
        if (affected) {
            int currentPos = vehicle->getCurrentNode();
            int target = vehicle->getTargetNode();
            std::vector<int> newPath = pathPlanner->replanPath(
                currentPos, target, path, currentPos);
            
            if (!newPath.empty()) {
                vehicle->setPath(newPath);
                totalReroutings++;
            }
        }
    }
}

void Simulation::updateStatistics() {
    // Calcul des statistiques de base
    // (peut être étendu)
}

void Simulation::printStatistics() const {
    std::cout << "=== Statistiques de simulation ===" << std::endl;
    std::cout << "Temps de simulation: " << simulationTime << "s" << std::endl;
    std::cout << "Nombre de véhicules: " << vehicles.size() << std::endl;
    std::cout << "Nombre d'événements actifs: " << events.size() << std::endl;
    std::cout << "Total reroutages: " << totalReroutings << std::endl;
    std::cout << "Mode: " << (mode == SimulationMode::DYNAMIC ? "Dynamique" : "Normal") << std::endl;
}

