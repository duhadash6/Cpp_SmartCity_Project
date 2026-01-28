#include "../include/Simulation.h"
#include "../include/Renderer.h"
#include "../include/Event.h"
#include <raylib.h>
#include <iostream>
#include <algorithm>

int main() {
    // Forcer l'affichage immédiat
    std::cout << "=== DEBUT DU PROGRAMME ===" << std::endl;
    std::cout << "Initialisation de la simulation..." << std::endl;
    std::cout.flush();
    fflush(stdout);
    
    try {
        // Initialisation de la simulation
        Simulation simulation;
        std::cout << "Creation du graphe..." << std::endl;
        std::cout.flush();
        simulation.initialize(""); // Utilise le graphe de test
        
        // Configuration
        simulation.setVehicleCount(50); // Plus de véhicules pour ville dynamique
        simulation.setEventCount(2);
        simulation.setMode(SimulationMode::DYNAMIC);
        std::cout << "Simulation configuree." << std::endl;
        
        // Vérifier qu'il y a des véhicules après création
        if (simulation.getVehicles().empty()) {
            std::cout << "ERREUR: Aucun vehicule n'a pu etre cree!" << std::endl;
            std::cout << "Le graphe peut ne pas etre correctement connecte." << std::endl;
        } else {
            std::cout << "Vehicules actifs: " << simulation.getVehicles().size() << std::endl;
        }
        
        // Initialisation du rendu - FENÊTRE AGRANDIE
        std::cout << "Initialisation de Raylib..." << std::endl;
        Renderer renderer(1920, 1080); // Fenêtre agrandie pour meilleure visibilité
        renderer.initialize();
        
        // Ajuster la caméra pour afficher toute la ville dans la fenêtre
        const Graph* graph = simulation.getGraph();
        if (graph) {
            renderer.fitGraphToWindow(*graph);
        }
        
        if (IsWindowReady()) {
            std::cout << "Fenetre Raylib creee avec succes!" << std::endl;
            std::cout << "Taille: " << GetScreenWidth() << "x" << GetScreenHeight() << std::endl;
        } else {
            std::cout << "ERREUR: Impossible de creer la fenetre Raylib!" << std::endl;
            std::cout << "Appuyez sur Entree pour quitter..." << std::endl;
            std::cin.get();
            return 1;
        }
        
        std::cout << "=== Simulation de Routage Dynamique ===" << std::endl;
        std::cout << "Controles:" << std::endl;
        std::cout << "  SPACE - Declencher un evenement aleatoire" << std::endl;
        std::cout << "  R - Basculer entre mode Normal et Dynamique" << std::endl;
        std::cout << "  +/- - Augmenter/Reduire le nombre de vehicules" << std::endl;
        std::cout << "  Fleches/WASD - Deplacer la camera" << std::endl;
        std::cout << "  Molette - Zoom" << std::endl;
        std::cout << "  ESC - Quitter" << std::endl;
        std::cout << "Debut de la boucle principale..." << std::endl;
        std::cout.flush();
        
        float lastTime = GetTime();
        
        // Boucle principale
        int frameCount = 0;
        int errorCount = 0;
        const int MAX_ERRORS = 10; // Maximum d'erreurs avant arrêt
        
        std::cout << "=== DEBUT DE LA BOUCLE PRINCIPALE ===" << std::endl;
        std::cout << "Nombre de vehicules actifs: " << simulation.getVehicles().size() << std::endl;
        std::cout.flush();
        
        // Vérifier qu'il y a des véhicules
        if (simulation.getVehicles().empty()) {
            std::cout << "ATTENTION: Aucun vehicule cree! Le jeu peut etre vide." << std::endl;
            std::cout << "Appuyez sur Entree pour continuer quand meme..." << std::endl;
            std::cin.get();
        }
        
        while (!WindowShouldClose()) {
            frameCount++;
            
            // Diagnostic toutes les 60 frames
            if (frameCount % 60 == 0) {
                std::cout << "Frame " << frameCount << " - Fenetre active" << std::endl;
                std::cout << "  WindowShouldClose: " << (WindowShouldClose() ? "true" : "false") << std::endl;
                std::cout << "  IsWindowReady: " << (IsWindowReady() ? "true" : "false") << std::endl;
                std::cout << "  Nombre de vehicules: " << simulation.getVehicles().size() << std::endl;
                std::cout << "  Nombre d'evenements: " << simulation.getEvents().size() << std::endl;
                std::cout.flush();
            }
            
            // Vérifier que la fenêtre est toujours valide
            if (!IsWindowReady()) {
                std::cout << "ERREUR: Fenetre n'est plus valide! (Frame " << frameCount << ")" << std::endl;
                std::cout.flush();
                errorCount++;
                if (errorCount >= MAX_ERRORS) {
                    std::cout << "Trop d'erreurs, arret du programme." << std::endl;
                    break;
                }
                // Attendre un peu avant de réessayer
                continue;
            }
            
            errorCount = 0; // Réinitialiser le compteur si tout va bien
            
            float currentTime = GetTime();
            if (!std::isfinite(currentTime)) {
                std::cout << "ERREUR: GetTime() retourne une valeur invalide!" << std::endl;
                continue;
            }
            
            float deltaTime = currentTime - lastTime;
            lastTime = currentTime;
            
            // Limiter le deltaTime pour éviter les sauts
            if (deltaTime <= 0.0f || !std::isfinite(deltaTime)) {
                std::cout << "ATTENTION: deltaTime invalide (" << deltaTime << "), utilisation de 0.016f" << std::endl;
                deltaTime = 0.016f; // ~60 FPS
            }
            deltaTime = std::min(deltaTime, 0.1f);
            
            // Gestion des entrées
            if (IsKeyPressed(KEY_SPACE)) {
                simulation.triggerRandomEvent();
                std::cout << "Evenement declenche!" << std::endl;
                
                // Notification visuelle
                const auto& events = simulation.getEvents();
                if (!events.empty()) {
                    const auto& lastEvent = events.back();
                    std::string eventName;
                    Color eventColor;
                    switch (lastEvent->getType()) {
                        case EventType::ACCIDENT:
                            eventName = "ACCIDENT detecte!";
                            eventColor = RED;
                            break;
                        case EventType::TRAFFIC_JAM:
                            eventName = "EMBOUTEILLAGE detecte!";
                            eventColor = ORANGE;
                            break;
                        case EventType::ROAD_CLOSURE:
                            eventName = "ROUTE FERMEE!";
                            eventColor = MAROON;
                            break;
                        case EventType::EMERGENCY:
                            eventName = "URGENCE!";
                            eventColor = BLUE;
                            break;
                    }
                    renderer.addNotification(eventName, eventColor, 4.0f);
                }
            }
            
            if (IsKeyPressed(KEY_R)) {
                SimulationMode currentMode = simulation.getMode();
                SimulationMode newMode = (currentMode == SimulationMode::DYNAMIC) ? 
                                        SimulationMode::NORMAL : SimulationMode::DYNAMIC;
                simulation.setMode(newMode);
                std::cout << "Mode change: " << 
                    (newMode == SimulationMode::DYNAMIC ? "Dynamique" : "Normal") << std::endl;
            }
            
            if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD)) {
                int currentCount = simulation.getVehicles().size();
                simulation.setVehicleCount(currentCount + 5);
                std::cout << "Nombre de vehicules: " << currentCount + 5 << std::endl;
            }
            
            if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT)) {
                int currentCount = simulation.getVehicles().size();
                if (currentCount > 5) {
                    simulation.setVehicleCount(currentCount - 5);
                    std::cout << "Nombre de vehicules: " << currentCount - 5 << std::endl;
                }
            }
            
            // Mise à jour de la simulation
            try {
                simulation.update(deltaTime);
            } catch (const std::exception& e) {
                std::cout << "ERREUR dans simulation.update (Frame " << frameCount << "): " << e.what() << std::endl;
                std::cout.flush();
                errorCount++;
                if (errorCount >= MAX_ERRORS) {
                    std::cout << "Trop d'erreurs dans simulation.update, arret." << std::endl;
                    break;
                }
            } catch (...) {
                std::cout << "ERREUR inconnue dans simulation.update (Frame " << frameCount << ")" << std::endl;
                std::cout.flush();
                errorCount++;
                if (errorCount >= MAX_ERRORS) {
                    std::cout << "Trop d'erreurs inconnues, arret." << std::endl;
                    break;
                }
            }
            
            // Mise à jour de la caméra
            try {
                renderer.updateCamera();
            } catch (...) {
                // Ignorer les erreurs de caméra
            }
            
            // Mise à jour de la musique
            renderer.updateMusic();
            
            // Gestion des clics sur les boutons
            renderer.handleButtonClicks(simulation);
            
            // Mise à jour des notifications
            renderer.updateNotifications(deltaTime);
            
            // Rendu
            try {
                renderer.beginFrame();
                renderer.renderSimulation(simulation);
                renderer.endFrame();
            } catch (const std::exception& e) {
                std::cout << "ERREUR dans le rendu (Frame " << frameCount << "): " << e.what() << std::endl;
                std::cout.flush();
                errorCount++;
                if (errorCount >= MAX_ERRORS) {
                    std::cout << "Trop d'erreurs de rendu, arret." << std::endl;
                    break;
                }
                // Essayer de fermer proprement le frame même en cas d'erreur
                try {
                    EndMode2D();
                    EndDrawing();
                } catch (...) {
                    // Ignorer
                }
            } catch (...) {
                std::cout << "ERREUR inconnue dans le rendu (Frame " << frameCount << ")" << std::endl;
                std::cout.flush();
                errorCount++;
                if (errorCount >= MAX_ERRORS) {
                    std::cout << "Trop d'erreurs inconnues de rendu, arret." << std::endl;
                    break;
                }
                try {
                    EndMode2D();
                    EndDrawing();
                } catch (...) {
                    // Ignorer
                }
            }
            
            // Vérifier que la fenêtre est toujours ouverte
            if (!IsWindowReady()) {
                std::cout << "ATTENTION: Fenetre fermee de maniere inattendue! (Frame " << frameCount << ")" << std::endl;
                std::cout.flush();
                break;
            }
            
            // Vérifier si WindowShouldClose a changé
            if (WindowShouldClose()) {
                std::cout << "WindowShouldClose() retourne true (Frame " << frameCount << ")" << std::endl;
                std::cout.flush();
                break;
            }
        }
        
        // Nettoyage
        std::cout << "Fermeture de la fenetre..." << std::endl;
        renderer.cleanup();
        
        // Affichage des statistiques finales
        simulation.printStatistics();
        
        std::cout << "Programme termine normalement." << std::endl;
        std::cout << "Appuyez sur Entree pour quitter..." << std::endl;
        std::cin.get();
        
        return 0;
    } catch (const std::exception& e) {
        std::cout << "ERREUR EXCEPTION: " << e.what() << std::endl;
        std::cout << "Appuyez sur Entree pour quitter..." << std::endl;
        std::cin.get();
        return 1;
    } catch (...) {
        std::cout << "ERREUR INCONNUE!" << std::endl;
        std::cout << "Appuyez sur Entree pour quitter..." << std::endl;
        std::cin.get();
        return 1;
    }
}

