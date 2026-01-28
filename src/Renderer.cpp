#include "Renderer.h"
#include "Simulation.h"
#include "Graph.h"
#include "Vehicle.h"
#include "Event.h"
#include <cmath>
#include <sstream>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <unordered_set>
#include "raylib.h"

Renderer::Renderer(int width, int height)
    : screenWidth(width), screenHeight(height),
      routeNormalColor(RAYWHITE),
      routeCongestedColor(YELLOW),
      routeBlockedColor(RED),
      vehicleColor(BLUE),
      nodeColor(LIGHTGRAY),
      backgroundColor(BLACK),
      scale(1.0f), offset({0.0f, 0.0f}),
      texturesLoaded(false) {
    
    camera.target = {0.0f, 0.0f};
    camera.offset = {width / 2.0f, height / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    
    
    vehicleCarTexture = {0};
    vehicleTruckTexture = {0};
    vehicleBusTexture = {0};
    eventAccidentTexture = {0};
    eventTrafficJamTexture = {0};
    eventRoadClosureTexture = {0};
    eventEmergencyTexture = {0};
    
  
    tilemapTexture = {0};
    roadStraightTexture = {0};
    roadHorizontalTexture = {0};
    roadVerticalTexture = {0};
    roadIntersectionTexture = {0};
    roadCurveTexture = {0};
    roadRoundaboutTexture = {0};
    grassTexture = {0};
    pavementTexture = {0};
    buildingTexture = {0};
    mapTilesLoaded = false;
    tileSize = 32; 
    
  
    gameFont = GetFontDefault();
    fontLoaded = false;
    backgroundMusic = {0};
    musicLoaded = false;
}

Renderer::~Renderer() {
    unloadTextures();
}

void Renderer::initialize() {
    InitWindow(screenWidth, screenHeight, "Simulation Routage Dynamique");
    SetTargetFPS(60);
    
   
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        std::cout << "⚠️  Impossible d'initialiser le peripherique audio" << std::endl;
    } else {
        std::cout << "✅ Peripherique audio initialise" << std::endl;
    }
    
   
    std::vector<std::string> fontPaths = {
        "assets/fonts/font.ttf",
        "assets/fonts/arial.ttf",
        "assets/fonts/roboto.ttf",
        "assets/fonts/calibri.ttf",
        "assets/fonts/verdana.ttf",
        "assets/fonts/tahoma.ttf",
        "../assets/fonts/font.ttf",
        "../assets/fonts/arial.ttf",
        "build/assets/fonts/font.ttf",
        "build/assets/fonts/arial.ttf"
    };
    
    fontLoaded = false;
    for (const auto& path : fontPaths) {
        if (FileExists(path.c_str())) {
            std::cout << "Tentative de chargement de la police: " << path << std::endl;
            gameFont = LoadFontEx(path.c_str(), 48, nullptr, 0); // Taille plus grande pour meilleure qualité
            if (gameFont.texture.id != 0 && IsFontValid(gameFont)) {
                fontLoaded = true;
                std::cout << "✅ Police chargee avec succes: " << path << std::endl;
                std::cout << "   Taille de base: " << gameFont.baseSize << "px" << std::endl;
                break;
            } else {
                std::cout << "❌ Echec du chargement de la police: " << path << std::endl;
            }
        }
    }
    
    if (!fontLoaded) {
     
        gameFont = GetFontDefault();
        std::cout << "⚠️  Utilisation de la police par defaut (aucune police personnalisee trouvee)" << std::endl;
        std::cout << "   Pour utiliser une police personnalisee, placez un fichier .ttf dans assets/fonts/" << std::endl;
    }
    
    SetTextLineSpacing(2);
    
   
    loadTextures();
    loadMapTiles();
    
   
    loadMusic();
}

void Renderer::cleanup() {
    unloadTextures();
    unloadMapTiles();
    
  
    if (musicLoaded) {
        StopMusicStream(backgroundMusic);
        UnloadMusicStream(backgroundMusic);
        std::cout << "✅ Musique dechargee" << std::endl;
    }
    
    if (fontLoaded && gameFont.texture.id != 0) {
        UnloadFont(gameFont);
    }
  
    if (IsAudioDeviceReady()) {
        CloseAudioDevice();
    }
    
    CloseWindow();
}

void Renderer::beginFrame() {
    if (!IsWindowReady()) {
        return; 
    }
    BeginDrawing();
    if (mapTilesLoaded && grassTexture.id != 0) {
        ClearBackground({34, 139, 34, 255}); 
    } else {
        ClearBackground(backgroundColor); 
    }
    BeginMode2D(camera);
}

void Renderer::endFrame() {
    if (!IsWindowReady()) {
        return;
    }
    try {
    EndMode2D();
    EndDrawing();
    } catch (...) {
    
    }
}

void Renderer::renderSimulation(const Simulation& simulation) {
    
    try {
        const Graph* graph = simulation.getGraph();
        if (graph) {
            renderGraph(*graph);  
        }
    } catch (...) {
        
    }
    
    try {
        
        renderVehicles(simulation.getVehicles());
    } catch (...) {
       
    }
    
    try {
        const Graph* graph = simulation.getGraph();
        if (graph) {
           
            renderEvents(simulation.getEvents(), *graph);
        }
    } catch (...) {
        
    }
    
    try {
    renderUI(simulation);
    } catch (...) {
    
    }
    
    try {
        renderNotifications();
    } catch (...) {
        
    }
}

void Renderer::renderGraph(const Graph& graph) {
    float currentTime = GetTime();
    float dayCycle = sinf(currentTime * 0.1f) * 0.3f + 0.7f; 
    Color bgColor = {
        (unsigned char)(50 * dayCycle + 20),   
        (unsigned char)(100 * dayCycle + 30),  
        (unsigned char)(50 * dayCycle + 20),   
        255
    };
    ClearBackground(bgColor);
    
   
    for (const auto& route : graph.getRoutes()) {
        if (!route) continue;
        
        Node* fromNode = graph.getNode(route->getFromNode());
        Node* toNode = graph.getNode(route->getToNode());
        
        if (!fromNode || !toNode) continue;
        
        
        if (!std::isfinite(fromNode->x) || !std::isfinite(fromNode->y) ||
            !std::isfinite(toNode->x) || !std::isfinite(toNode->y)) {
            continue;
        }
        
        Vector2 start = {fromNode->x, fromNode->y};
        Vector2 end = {toNode->x, toNode->y};
        
      
        float dx = end.x - start.x;
        float dy = end.y - start.y;
        float length = sqrtf(dx * dx + dy * dy);
        
        if (length < 0.1f) continue;
        
       
        float perpX = -dy / length;
        float perpY = dx / length;
        
        // Largeur de la route (AUGMENTÉE pour meilleure visibilité)
        float roadWidth = 35.0f;  // Augmenté de 28 à 35
        float borderWidth = 3.0f;  // Augmenté de 2.5 à 3
        
        // Couleur de base selon l'état (PLUS CONTRASTÉE)
        Color asphaltColor = {55, 55, 60, 255};  // Plus sombre pour meilleur contraste
        Color statusColor = {0, 0, 0, 0};
        
        switch (route->getState()) {
            case RouteState::NORMAL:
                asphaltColor = {55, 55, 60, 255};  // Asphalte plus sombre
                break;
            case RouteState::CONGESTED:
                statusColor = {255, 200, 0, 180};  // Jaune plus visible pour congestion
                break;
            case RouteState::BLOCKED:
            case RouteState::ACCIDENT:
                // Routes bloquées/accidentées : rouge très visible avec effet de clignotement
                asphaltColor = {80, 20, 20, 255};  // Asphalte rouge foncé
                statusColor = {255, 0, 0, 200};     // Rouge très visible
                break;
        }
        
        // 1. Dessiner la base de la route (asphalte sombre avec ombre)
        DrawLineEx(start, end, roadWidth + borderWidth * 2 + 2, {35, 35, 40, 255});
        
        // 2. Dessiner l'asphalte principal (PLUS VISIBLE)
        DrawLineEx(start, end, roadWidth, asphaltColor);
        
        // 3. Dessiner les bordures blanches (PLUS ÉPAISSES ET VISIBLES)
        float borderOffset = roadWidth / 2.0f;
        Vector2 border1Start = {start.x + perpX * borderOffset, start.y + perpY * borderOffset};
        Vector2 border1End = {end.x + perpX * borderOffset, end.y + perpY * borderOffset};
        Vector2 border2Start = {start.x - perpX * borderOffset, start.y - perpY * borderOffset};
        Vector2 border2End = {end.x - perpX * borderOffset, end.y - perpY * borderOffset};
        
        DrawLineEx(border1Start, border1End, 3.5f, {255, 255, 255, 255});  // Plus épais et plus blanc
        DrawLineEx(border2Start, border2End, 3.5f, {255, 255, 255, 255});  // Plus épais et plus blanc
        
        // 4. Dessiner la ligne centrale pointillée ANIMÉE (effet de mouvement amélioré)
        int numDashes = (int)(length / 15.0f);  // Plus de tirets pour meilleure visibilité
        if (numDashes > 0) {
            float dashTime = GetTime();
            float dashSpeed = 25.0f;  // Vitesse d'animation augmentée
            float dashOffset = fmodf(dashTime * dashSpeed, 30.0f); // Animation de défilement
            
            for (int i = 0; i < numDashes; i += 2) {
                float t1 = (float)i / numDashes;
                float t2 = (float)(i + 1) / numDashes;
                
                // Décalage pour effet de mouvement
                float offset1 = dashOffset / length;
                float offset2 = dashOffset / length;
                
                Vector2 dashStart = {start.x + dx * (t1 + offset1), start.y + dy * (t1 + offset1)};
                Vector2 dashEnd = {start.x + dx * (t2 + offset2), start.y + dy * (t2 + offset2)};
                
                // Vérifier que le dash est dans les limites de la route
                if (t1 + offset1 >= 0 && t2 + offset2 <= 1.0f) {
                    // Tirets plus épais et plus visibles
                    DrawLineEx(dashStart, dashEnd, 2.5f, {255, 255, 255, 255});  // Blanc pur pour meilleur contraste
                }
            }
        }
        
        // 5. Indicateur d'état pour routes congestionnées ou bloquées (AMÉLIORÉ)
        if (statusColor.a > 0) {
            // Effet de pulsation pour routes bloquées
            if (route->getState() == RouteState::BLOCKED || route->getState() == RouteState::ACCIDENT) {
                float pulse = sinf(currentTime * 3.0f) * 0.3f + 0.7f;
                statusColor.a = (unsigned char)(150 * pulse);
            }
            DrawLineEx(start, end, roadWidth * 0.8f, statusColor);
            
            // Halo autour des routes bloquées
            if (route->getState() == RouteState::BLOCKED || route->getState() == RouteState::ACCIDENT) {
                DrawLineEx(start, end, roadWidth * 1.2f, {255, 50, 50, 60});
            }
        }
    }
    
    // Ajouter des MAISONS 2D DÉTAILLÉES dans les CARRÉS VERTS (espaces entre les routes)
    // Structure : Grille 6x6 avec espacement de 130px
    // Les carrés sont formés par 4 nœuds adjacents
    const float spacing = 130.0f;
    const int gridSize = 6;
    
    // Parcourir tous les carrés de la grille (5x5 carrés pour une grille 6x6)
    for (int row = 0; row < gridSize - 1; row++) {
        for (int col = 0; col < gridSize - 1; col++) {
            // Trouver les 4 nœuds formant ce carré
            Node* topLeft = nullptr;
            Node* topRight = nullptr;
            Node* bottomLeft = nullptr;
            Node* bottomRight = nullptr;
            
            // Chercher les nœuds correspondants dans le graphe
            for (const auto& node : graph.getNodes()) {
                if (!node) continue;
                float expectedX = col * spacing;
                float expectedY = row * spacing;
                float expectedX2 = (col + 1) * spacing;
                float expectedY2 = (row + 1) * spacing;
                
                // Tolérance pour les comparaisons flottantes
                const float tolerance = 1.0f;
                
                if (std::abs(node->x - expectedX) < tolerance && std::abs(node->y - expectedY) < tolerance) {
                    topLeft = node.get();
                } else if (std::abs(node->x - expectedX2) < tolerance && std::abs(node->y - expectedY) < tolerance) {
                    topRight = node.get();
                } else if (std::abs(node->x - expectedX) < tolerance && std::abs(node->y - expectedY2) < tolerance) {
                    bottomLeft = node.get();
                } else if (std::abs(node->x - expectedX2) < tolerance && std::abs(node->y - expectedY2) < tolerance) {
                    bottomRight = node.get();
                }
            }
            
            // Si on a trouvé les 4 coins, placer une maison au centre du carré
            if (topLeft && topRight && bottomLeft && bottomRight) {
                // Centre du carré (espace vert) - bien au milieu pour éviter les routes
                float centerX = (topLeft->x + topRight->x) / 2.0f;
                float centerY = (topLeft->y + bottomLeft->y) / 2.0f;
                
                // Placer 1 seule maison bien centrée dans chaque bloc
                int numHouses = 1;
                
                for (int i = 0; i < numHouses; i++) {
                    // Variété de maisons : différentes tailles et styles
                    int houseType = (row + col + i) % 3;
                    float buildingWidth, buildingHeight;
                    Color houseColor;
                    
                    switch (houseType) {
                        case 0:  // Petite maison
                            buildingWidth = 22.0f;
                            buildingHeight = 30.0f;
                            houseColor = {120, 100, 80, 255};  // Beige
                            break;
                        case 1:  // Maison moyenne
                            buildingWidth = 30.0f;
                            buildingHeight = 42.0f;
                            houseColor = {100, 120, 100, 255};  // Vert clair
                            break;
                        default:  // Grande maison grise
                            buildingWidth = 38.0f;
                            buildingHeight = 55.0f;
                            houseColor = {90, 90, 110, 255};  // Gris bleu
                            break;
                    }
                    
                    // Ajustement de position - la grande maison grise est décalée vers le haut
                    float offsetX = 0.0f;
                    float offsetY = (houseType == 2) ? -12.0f : 0.0f;  // Grande maison grise 12 pixels plus haut
                    float buildingX = centerX + offsetX;
                    float buildingY = centerY + offsetY;
            
                    // Ombre du bâtiment
                    DrawRectangle(buildingX + 4, buildingY + 4, buildingWidth, buildingHeight, {0, 0, 0, 120});
                    
                    // Corps principal du bâtiment
                    DrawRectangle(buildingX, buildingY, buildingWidth, buildingHeight, houseColor);
                    DrawRectangleLines(buildingX, buildingY, buildingWidth, buildingHeight, {60, 60, 70, 255});
                    
                    // Toit triangulaire
                    Vector2 roofTop = {buildingX + buildingWidth / 2.0f, buildingY - 6.0f};
                    Vector2 roofLeft = {buildingX - 1.0f, buildingY};
                    Vector2 roofRight = {buildingX + buildingWidth + 1.0f, buildingY};
                    DrawTriangle(roofTop, roofLeft, roofRight, {100, 50, 50, 255});  // Toit rouge
                    DrawTriangleLines(roofTop, roofLeft, roofRight, {80, 40, 40, 255});
                    
                    // Porte
                    float doorWidth = 5.0f;
                    float doorHeight = 10.0f;
                    float doorX = buildingX + buildingWidth / 2.0f - doorWidth / 2.0f;
                    float doorY = buildingY + buildingHeight - doorHeight;
                    DrawRectangle(doorX, doorY, doorWidth, doorHeight, {60, 40, 30, 255});
                    DrawRectangleLines(doorX, doorY, doorWidth, doorHeight, {40, 30, 20, 255});
                    
                    // Fenêtres animées
                    int windowRows = 2;
                    int windowCols = 2;
                    float windowSize = 4.0f;
                    float windowSpacing = 9.0f;
                    float windowStartX = buildingX + (buildingWidth - (windowCols - 1) * windowSpacing - windowSize) / 2.0f;
                    float windowStartY = buildingY + 8.0f;
                    
                    for (int winRow = 0; winRow < windowRows; winRow++) {
                        for (int winCol = 0; winCol < windowCols; winCol++) {
                            float winX = windowStartX + winCol * windowSpacing;
                            float winY = windowStartY + winRow * windowSpacing;
                            
                            // Animation : fenêtres qui s'allument/éteignent
                            float windowTime = currentTime + (row * 0.5f) + (col * 0.3f) + (winRow * 0.4f) + (winCol * 0.3f) + (i * 0.2f);
                            float windowCycle = sinf(windowTime * 0.6f) + cosf(windowTime * 0.4f) * 0.5f;
                            
                            Color windowColor;
                            if (windowCycle > 0.5f) {
                                windowColor = {255, 255, 180, 255};
                                DrawCircle(winX + windowSize/2, winY + windowSize/2, windowSize * 0.8f, {255, 255, 150, 40});
                            } else if (windowCycle > 0.0f) {
                                windowColor = {180, 180, 120, 200};
                            } else {
                                windowColor = {30, 30, 40, 255};
                            }
                            
                            DrawRectangle(winX, winY, windowSize, windowSize, windowColor);
                            DrawRectangleLines(winX, winY, windowSize, windowSize, {20, 20, 30, 255});
                            DrawLine(winX + windowSize/2, winY, winX + windowSize/2, winY + windowSize, {20, 20, 30, 200});
                            DrawLine(winX, winY + windowSize/2, winX + windowSize, winY + windowSize/2, {20, 20, 30, 200});
                        }
                    }
                }
            }
        }
    }
    
    // Rendre les intersections avec style moderne et FEUX DE CIRCULATION
    // D'abord, identifier tous les nœuds qui font partie d'un rond-point
    std::unordered_set<int> roundaboutNodes;
    for (const auto& node : graph.getNodes()) {
        if (!node) continue;
        std::vector<int> routes = graph.getRoutesFromNode(node->id);
        // Si un nœud a plus de 4 connexions, c'est le centre d'un rond-point
        if (routes.size() > 4) {
            roundaboutNodes.insert(node->id);
            // Ajouter tous les nœuds connectés à ce nœud central comme faisant partie du rond-point
            for (int routeId : routes) {
                Route* route = graph.getRoute(routeId);
                if (route) {
                    if (route->getFromNode() == node->id) {
                        roundaboutNodes.insert(route->getToNode());
                    } else if (route->getToNode() == node->id) {
                        roundaboutNodes.insert(route->getFromNode());
                    }
                }
            }
        }
    }
    
    for (const auto& node : graph.getNodes()) {
        if (!node) continue;
        
        if (!std::isfinite(node->x) || !std::isfinite(node->y)) {
            continue;
        }
        
        std::vector<int> routes = graph.getRoutesFromNode(node->id);
        int numConnections = routes.size();
        
        if (numConnections > 0) {
            // Déterminer si c'est un rond-point (nœud central ou nœud connecté à un rond-point)
            bool isRoundabout = (numConnections > 4) || (roundaboutNodes.find(node->id) != roundaboutNodes.end());
            
            if (isRoundabout) {
                // Rond-point : seulement rendre le nœud central (celui avec plus de 4 connexions)
                // Les nœuds internes ne sont pas rendus comme intersections
                if (numConnections > 4) {
                    // C'est le nœud central du rond-point
                    float roundaboutSize = 32.0f;
                    
                    // Fond asphalte
                    DrawCircle(node->x, node->y, roundaboutSize, {65, 65, 70, 255});
                    
                    // Bordure extérieure blanche
                    DrawCircleLines(node->x, node->y, roundaboutSize, {250, 250, 250, 220});
                    
                    // Cercle central (îlot)
                    DrawCircle(node->x, node->y, roundaboutSize * 0.35f, {60, 120, 60, 255});
                    DrawCircleLines(node->x, node->y, roundaboutSize * 0.35f, {220, 220, 220, 200});
                    
                    // Lignes de direction (flèches circulaires)
                    for (int i = 0; i < 4; i++) {
                        float angle = i * PI / 2.0f + PI / 4.0f;
                        float innerRadius = roundaboutSize * 0.45f;
                        float outerRadius = roundaboutSize * 0.75f;
                        
                        float x1 = node->x + cosf(angle) * innerRadius;
                        float y1 = node->y + sinf(angle) * innerRadius;
                        float x2 = node->x + cosf(angle) * outerRadius;
                        float y2 = node->y + sinf(angle) * outerRadius;
                        
                        DrawLineEx({x1, y1}, {x2, y2}, 1.2f, {230, 230, 230, 180});
                    }
                }
                // Les nœuds internes du rond-point ne sont pas rendus (pas de feux, pas d'intersection)
            } else {
                // Intersection normale : cercle simple avec FEUX DE CIRCULATION
                float intersectionSize = 18.0f;
                
                // Fond asphalte
                DrawCircle(node->x, node->y, intersectionSize, {65, 65, 70, 255});
                
                // Bordure subtile
                DrawCircleLines(node->x, node->y, intersectionSize, {250, 250, 250, 180});
                
                // Marquage central
                DrawCircle(node->x, node->y, 3.5f, {230, 230, 230, 200});
                
                // FEUX DE CIRCULATION ANIMÉS (si intersection importante)
                if (numConnections >= 3) {
                    float lightCycle = fmodf(currentTime * 2.0f, 4.0f); // Cycle de 4 secondes
                    
                    // Feux aux 4 directions
                    for (int i = 0; i < 4; i++) {
                        float angle = i * PI / 2.0f;
                        float lightX = node->x + cosf(angle) * intersectionSize * 0.7f;
                        float lightY = node->y + sinf(angle) * intersectionSize * 0.7f;
                        
                        // Cycle : Rouge (0-1s) -> Orange (1-2s) -> Vert (2-3s) -> Orange (3-4s)
                        Color lightColor = {100, 100, 100, 255}; // Éteint par défaut
                        if (lightCycle < 1.0f) {
                            // Rouge
                            lightColor = {255, 0, 0, 255};
                        } else if (lightCycle < 2.0f) {
                            // Orange
                            lightColor = {255, 165, 0, 255};
                        } else if (lightCycle < 3.0f) {
                            // Vert
                            lightColor = {0, 255, 0, 255};
                        } else {
                            // Orange
                            lightColor = {255, 165, 0, 255};
                        }
                        
                        // Dessiner le feu de circulation
                        DrawCircle(lightX, lightY, 4.0f, lightColor);
                        DrawCircleLines(lightX, lightY, 4.0f, {255, 255, 255, 200});
                        
                        // Halo lumineux
                        DrawCircle(lightX, lightY, 6.0f, {lightColor.r, lightColor.g, lightColor.b, 80});
                    }
                    
                    // Marquages de passage piéton (lignes blanches)
                    for (int i = 0; i < 4; i++) {
                        float angle = i * PI / 2.0f;
                        float x1 = node->x + cosf(angle) * intersectionSize * 0.6f;
                        float y1 = node->y + sinf(angle) * intersectionSize * 0.6f;
                        float x2 = node->x + cosf(angle) * intersectionSize * 0.9f;
                        float y2 = node->y + sinf(angle) * intersectionSize * 0.9f;
                        
                        DrawLineEx({x1, y1}, {x2, y2}, 1.5f, {230, 230, 230, 180});
                    }
                }
            }
        }
    }
}

void Renderer::renderVehicles(const std::vector<std::unique_ptr<Vehicle>>& vehicles) {
    for (const auto& vehicle : vehicles) {
        if (!vehicle) continue;
        
        if (vehicle->hasReachedDestination()) {
            continue;
        }
        
        // Vérifier que les coordonnées sont valides
        float x = vehicle->getX();
        float y = vehicle->getY();
        float angle = vehicle->getAngle();
        int type = vehicle->getVehicleType();
        
        // Ignorer les positions invalides (NaN ou infinies)
        if (!std::isfinite(x) || !std::isfinite(y)) {
            continue;
        }
        
        // Couleur selon le type de véhicule
        Color vColor;
        float size;
        switch (type) {
            case 0: // Voiture
                vColor = {
                    static_cast<unsigned char>(50 + (vehicle->getId() * 37) % 200),
                    static_cast<unsigned char>(100 + (vehicle->getId() * 23) % 150),
                    static_cast<unsigned char>(150 + (vehicle->getId() * 41) % 100),
                    255
                };
                size = 15.0f; // Agrandi de 7 à 15
                break;
            case 1: // Camion
                vColor = {150, 100, 50, 255};
                size = 18.0f; // Agrandi de 9 à 18
                break;
            case 2: // Bus
                vColor = {200, 50, 50, 255};
                size = 16.0f; // Agrandi de 8 à 16
                break;
            default:
                vColor = vehicleColor;
                size = 14.0f; // Agrandi de 6 à 14
        }
        
        // Dessiner le véhicule comme un rectangle orienté (sprite simple)
        Vector2 center = {x, y};
        float width = size * 1.5f;
        float height = size;
        
        // Vérifier que l'angle est valide
        if (!std::isfinite(angle)) {
            angle = 0.0f;
        }
        
        // Rectangle orienté selon la direction
        float cosAngle = cosf(angle - PI/2);
        float sinAngle = sinf(angle - PI/2);
        
        if (!std::isfinite(cosAngle) || !std::isfinite(sinAngle)) {
            // En cas d'erreur, dessiner un simple rectangle
            DrawRectangle((int)(x - width/2), (int)(y - height/2), (int)width, (int)height, vColor);
            DrawRectangleLines((int)(x - width/2), (int)(y - height/2), (int)width, (int)height, WHITE);
        } else {
            Vector2 corners[4] = {
                {center.x + cosAngle * width/2 - sinAngle * height/2,
                 center.y + sinAngle * width/2 + cosAngle * height/2},
                {center.x + cosAngle * width/2 + sinAngle * height/2,
                 center.y + sinAngle * width/2 - cosAngle * height/2},
                {center.x - cosAngle * width/2 + sinAngle * height/2,
                 center.y - sinAngle * width/2 - cosAngle * height/2},
                {center.x - cosAngle * width/2 - sinAngle * height/2,
                 center.y - sinAngle * width/2 + cosAngle * height/2}
            };
            
            // Vérifier que tous les coins sont valides
            bool valid = true;
            for (int i = 0; i < 4; i++) {
                if (!std::isfinite(corners[i].x) || !std::isfinite(corners[i].y)) {
                    valid = false;
                    break;
                }
            }
            
            if (valid) {
                // Déterminer la direction selon l'angle (en degrés)
                // L'angle est en radians, 0 = droite, PI/2 = bas, PI = gauche, 3*PI/2 = haut
                float angleDeg = angle * 180.0f / PI;
                while (angleDeg < 0) angleDeg += 360.0f;
                while (angleDeg >= 360) angleDeg -= 360.0f;
                
                // Déterminer la direction cardinale selon l'angle
                // 0° = droite, 90° = bas, 180° = gauche, 270° = haut
                std::string direction = "droite"; // Par défaut
                
                // Zones d'angles pour chaque direction (avec seuils de 45°)
                if (angleDeg >= 315.0f || angleDeg < 45.0f) {
                    direction = "droite";  // 315°-45° = droite
                } else if (angleDeg >= 45.0f && angleDeg < 135.0f) {
                    direction = "bas";     // 45°-135° = bas
                } else if (angleDeg >= 135.0f && angleDeg < 225.0f) {
                    direction = "gauche";  // 135°-225° = gauche
                } else if (angleDeg >= 225.0f && angleDeg < 315.0f) {
                    direction = "haut";    // 225°-315° = haut
                }
                
                // Utiliser le cache de textures - SÉLECTION CRÉATIVE SELON DIRECTION
                Texture2D* vehicleTexture = nullptr;
                std::string cacheKey = "";
                
                switch (type) {
                    case 0: // Voiture - Alterner entre bleue et rouge selon l'ID
                        if (vehicle->getId() % 2 == 0) {
                            cacheKey = "voiture_bleue_vers_" + direction;
                        } else {
                            cacheKey = "voiture_rouge_vers_" + direction;
                        }
                        if (vehicleTextureCache.find(cacheKey) != vehicleTextureCache.end()) {
                            vehicleTexture = &vehicleTextureCache[cacheKey];
                        } else if (vehicleCarTexture.id != 0) {
                            vehicleTexture = &vehicleCarTexture;
                        }
                        break;
                    case 1: // Camion - Toujours rouge
                        cacheKey = "camion_rouge_vers_" + direction;
                        if (vehicleTextureCache.find(cacheKey) != vehicleTextureCache.end()) {
                            vehicleTexture = &vehicleTextureCache[cacheKey];
                        } else if (vehicleTruckTexture.id != 0) {
                            vehicleTexture = &vehicleTruckTexture;
                        }
                        break;
                    case 2: // Bus/Taxi - Toujours jaune
                        cacheKey = "taxi_jaune_vers_" + direction;
                        if (vehicleTextureCache.find(cacheKey) != vehicleTextureCache.end()) {
                            vehicleTexture = &vehicleTextureCache[cacheKey];
                        } else if (vehicleBusTexture.id != 0) {
                            vehicleTexture = &vehicleBusTexture;
                        }
                        break;
                }
                
                if (vehicleTexture && vehicleTexture->id != 0) {
                    // Dessiner avec texture (sprite) - SANS ROTATION car texture déjà orientée
                    float spriteSize = size * 2.8f;
                    
                    // Ajuster la taille selon le type de véhicule
                    if (type == 1) { // Camion plus grand
                        spriteSize = size * 3.2f;
                    } else if (type == 2) { // Taxi taille moyenne
                        spriteSize = size * 2.6f;
                    }
                    
                    Rectangle source = {0, 0, (float)vehicleTexture->width, (float)vehicleTexture->height};
                    Rectangle dest = {x, y, spriteSize, spriteSize};
                    Vector2 origin = {spriteSize / 2.0f, spriteSize / 2.0f};
                    
                    // PAS DE ROTATION - La texture est déjà orientée dans la bonne direction
                    DrawTexturePro(*vehicleTexture, source, dest, origin, 0.0f, WHITE);
                    
                    // Ajouter une ombre subtile pour plus de profondeur
                    DrawCircle(x + 2, y + 2, spriteSize * 0.3f, {0, 0, 0, 60});
                } else {
                    // Dessiner avec forme géométrique professionnelle (fallback)
                    // Corps du véhicule
                    DrawTriangle(corners[0], corners[1], corners[2], vColor);
                    DrawTriangle(corners[0], corners[2], corners[3], vColor);
                    DrawTriangleLines(corners[0], corners[1], corners[2], {255, 255, 255, 200});
                    DrawTriangleLines(corners[0], corners[2], corners[3], {255, 255, 255, 200});
                    
                    // Pare-brise (ligne claire)
                    Vector2 frontCenter = {
                        (corners[0].x + corners[1].x) / 2.0f,
                        (corners[0].y + corners[1].y) / 2.0f
                    };
                    DrawLineEx(frontCenter, center, 1.5f, {255, 255, 255, 150});
                    
                    // Flèche de direction (avant du véhicule)
                    float frontX = x + cosf(angle) * size * 0.8f;
                    float frontY = y + sinf(angle) * size * 0.8f;
                    if (std::isfinite(frontX) && std::isfinite(frontY)) {
                        Vector2 front = {frontX, frontY};
                        DrawLineEx(center, front, 2.5f, {255, 255, 255, 255});
                        // Pointe de flèche
                        float arrowAngle1 = angle + 2.5f;
                        float arrowAngle2 = angle - 2.5f;
                        Vector2 arrow1 = {frontX + cosf(arrowAngle1) * 5.0f, frontY + sinf(arrowAngle1) * 5.0f};
                        Vector2 arrow2 = {frontX + cosf(arrowAngle2) * 5.0f, frontY + sinf(arrowAngle2) * 5.0f};
                        DrawLineEx(front, arrow1, 2.0f, {255, 255, 255, 255});
                        DrawLineEx(front, arrow2, 2.0f, {255, 255, 255, 255});
                    }
                }
            } else {
                // Fallback : rectangle simple
                DrawRectangle((int)(x - width/2), (int)(y - height/2), (int)width, (int)height, vColor);
                DrawRectangleLines((int)(x - width/2), (int)(y - height/2), (int)width, (int)height, WHITE);
            }
        }
    }
}

void Renderer::renderEvents(const std::vector<std::unique_ptr<Event>>& events, const Graph& graph) {
    for (const auto& event : events) {
        if (!event || !event->isActive()) continue;
        
        Route* route = graph.getRoute(event->getRouteId());
        if (!route) continue;
        
        Node* fromNode = graph.getNode(route->getFromNode());
        Node* toNode = graph.getNode(route->getToNode());
        
        if (!fromNode || !toNode) continue;
        
        // Vérifier que les coordonnées sont valides
        if (!std::isfinite(fromNode->x) || !std::isfinite(fromNode->y) ||
            !std::isfinite(toNode->x) || !std::isfinite(toNode->y)) {
            continue;
        }
        
        // Position au milieu de la route
        float midX = (fromNode->x + toNode->x) / 2.0f;
        float midY = (fromNode->y + toNode->y) / 2.0f;
        
        // Vérifier que le résultat est valide
        if (!std::isfinite(midX) || !std::isfinite(midY)) {
            continue;
        }
        
        // Icône selon le type d'événement - VERSION AMÉLIORÉE
        Color eventColor = RED;
        Color borderColor = WHITE;
        float size = 15.0f;
        
        switch (event->getType()) {
            case EventType::ACCIDENT:
                // ACCIDENT : Affichage créatif avec texture
                if (eventAccidentTexture.id != 0) {
                    float texSize = 48.0f; // Plus grand pour meilleure visibilité
                    
                    // Effet de pulsation
                    float time = GetTime();
                    float pulse = 1.0f + 0.15f * sinf(time * 3.0f);
                    texSize *= pulse;
                    
                    // Halo rouge clignotant
                    float haloAlpha = 100 + 80 * sinf(time * 4.0f);
                    DrawCircle(midX, midY, texSize * 0.8f, {255, 0, 0, (unsigned char)haloAlpha});
                    
                    // Texture de l'accident
                    Rectangle source = {0, 0, (float)eventAccidentTexture.width, (float)eventAccidentTexture.height};
                    Rectangle dest = {midX, midY, texSize, texSize};
                    Vector2 origin = {texSize / 2.0f, texSize / 2.0f};
                    DrawTexturePro(eventAccidentTexture, source, dest, origin, 0.0f, WHITE);
                    
                    // Panneau d'information
                    DrawRectangle(midX - 40, midY - 45, 80, 20, {0, 0, 0, 180});
                    DrawTextEx(gameFont, "ACCIDENT", {midX - 35.0f, midY - 42.0f}, 14.0f, 1.0f, {255, 255, 255, 255});
                } else {
                    // Fallback : Rouge vif avec X et cercle épais
                    eventColor = RED;
                    borderColor = {139, 0, 0, 255};
                    size = 18.0f;
                    DrawCircle(midX, midY, size, eventColor);
                    DrawCircleLines(midX, midY, size, borderColor);
                    DrawLineEx({midX - 8, midY - 8}, {midX + 8, midY + 8}, 3.0f, WHITE);
                    DrawLineEx({midX + 8, midY - 8}, {midX - 8, midY + 8}, 3.0f, WHITE);
                    DrawTextEx(gameFont, "ACCIDENT", {midX - 35.0f, midY - 30.0f}, 14.0f, 1.0f, WHITE);
                }
                break;
                
            case EventType::TRAFFIC_JAM:
                // EMBOUTEILLAGE : Affichage créatif avec texture
                if (eventTrafficJamTexture.id != 0) {
                    float texSize = 45.0f;
                    
                    // Halo orange/jaune
                    DrawCircle(midX, midY, texSize * 0.75f, {255, 200, 0, 120});
                    
                    // Texture de l'embouteillage
                    Rectangle source = {0, 0, (float)eventTrafficJamTexture.width, (float)eventTrafficJamTexture.height};
                    Rectangle dest = {midX, midY, texSize, texSize};
                    Vector2 origin = {texSize / 2.0f, texSize / 2.0f};
                    DrawTexturePro(eventTrafficJamTexture, source, dest, origin, 0.0f, WHITE);
                    
                    // Panneau d'information avec police personnalisée
                    DrawRectangle(midX - 55, midY - 45, 110, 20, {0, 0, 0, 180});
                    DrawTextEx(gameFont, "EMBOUTEILLAGE", {midX - 50.0f, midY - 42.0f}, 13.0f, 1.0f, {255, 200, 0, 255});
                } else {
                    // Fallback : Orange avec icône de voitures
                    eventColor = ORANGE;
                    borderColor = {255, 140, 0, 255};
                    size = 16.0f;
                    DrawCircle(midX, midY, size, eventColor);
                    DrawCircleLines(midX, midY, size, borderColor);
                    for (int i = 0; i < 3; i++) {
                        DrawRectangle(midX - 10 + i * 4, midY - 3, 3, 6, WHITE);
                    }
                    DrawTextEx(gameFont, "EMBOUTEILLAGE", {midX - 50.0f, midY - 30.0f}, 13.0f, 1.0f, WHITE);
                }
                break;
                
            case EventType::ROAD_CLOSURE:
                // ROUTE FERMÉE : Utiliser texture si disponible
                if (eventRoadClosureTexture.id != 0) {
                    float texSize = 32.0f;
                    Rectangle source = {0, 0, (float)eventRoadClosureTexture.width, (float)eventRoadClosureTexture.height};
                    Rectangle dest = {midX, midY, texSize, texSize};
                    Vector2 origin = {texSize / 2.0f, texSize / 2.0f};
                    DrawTexturePro(eventRoadClosureTexture, source, dest, origin, 0.0f, WHITE);
                } else {
                    // Fallback : Rouge foncé avec barrière
                eventColor = MAROON;
                    borderColor = {139, 0, 0, 255}; // DARKRED
                    size = 16.0f;
                    DrawRectangle(midX - size, midY - size, size * 2, size * 2, eventColor);
                    DrawRectangleLines(midX - size, midY - size, size * 2, size * 2, borderColor);
                    DrawLineEx({midX - 10, midY - 10}, {midX + 10, midY + 10}, 2.0f, WHITE);
                    DrawLineEx({midX + 10, midY - 10}, {midX - 10, midY + 10}, 2.0f, WHITE);
                }
                DrawTextEx(gameFont, "FERME", {midX - 25.0f, midY - 30.0f}, 13.0f, 1.0f, WHITE);
                break;
                
            case EventType::EMERGENCY:
                // URGENCE : Affichage créatif avec texture
                if (eventEmergencyTexture.id != 0) {
                    float texSize = 50.0f;
                    float time = GetTime();
                    
                    // Effet de rotation pour attirer l'attention
                    float rotation = time * 60.0f; // Rotation lente
                    
                    // Halo bleu clignotant (comme gyrophare)
                    float haloAlpha = 120 + 100 * sinf(time * 6.0f);
                    DrawCircle(midX, midY, texSize * 0.85f, {0, 100, 255, (unsigned char)haloAlpha});
                    
                    // Deuxième halo pour effet de profondeur
                    float haloAlpha2 = 80 + 60 * sinf(time * 6.0f + 1.5f);
                    DrawCircle(midX, midY, texSize * 1.1f, {0, 150, 255, (unsigned char)haloAlpha2});
                    
                    // Texture de l'urgence avec clignotement
                    float alpha = 200 + 55 * sinf(time * 5.0f);
                    Color tint = {255, 255, 255, (unsigned char)alpha};
                    Rectangle source = {0, 0, (float)eventEmergencyTexture.width, (float)eventEmergencyTexture.height};
                    Rectangle dest = {midX, midY, texSize, texSize};
                    Vector2 origin = {texSize / 2.0f, texSize / 2.0f};
                    DrawTexturePro(eventEmergencyTexture, source, dest, origin, rotation, tint);
                    
                    // Panneau d'information avec clignotement et police personnalisée
                    unsigned char panelAlpha = (unsigned char)(180 + 75 * sinf(time * 4.0f));
                    DrawRectangle(midX - 40, midY - 50, 80, 22, {0, 0, 0, panelAlpha});
                    DrawTextEx(gameFont, "URGENCE", {midX - 35.0f, midY - 47.0f}, 14.0f, 1.0f, {255, 50, 50, 255});
                } else {
                    // Fallback : Bleu clignotant avec étoile
                    eventColor = BLUE;
                    borderColor = {0, 0, 139, 255};
                    size = 18.0f;
                    float time = GetTime();
                    float alpha = 180 + 75 * sinf(time * 5.0f);
                    Color flashColor = {eventColor.r, eventColor.g, eventColor.b, (unsigned char)alpha};
                    DrawCircle(midX, midY, size, flashColor);
                    DrawCircleLines(midX, midY, size, borderColor);
                    for (int i = 0; i < 8; i++) {
                        float angle = i * PI / 4.0f;
                        float x1 = midX + cosf(angle) * 8.0f;
                        float y1 = midY + sinf(angle) * 8.0f;
                        float x2 = midX + cosf(angle + PI / 8.0f) * 12.0f;
                        float y2 = midY + sinf(angle + PI / 8.0f) * 12.0f;
                        DrawLineEx({midX, midY}, {x1, y1}, 2.0f, WHITE);
                        DrawLineEx({x1, y1}, {x2, y2}, 2.0f, WHITE);
                    }
                    DrawTextEx(gameFont, "URGENCE", {midX - 35.0f, midY - 30.0f}, 13.0f, 1.0f, WHITE);
                }
                break;
        }
        
        // Afficher la durée restante avec style amélioré
        float remainingTime = event->getDuration() - event->getElapsedTime();
        if (remainingTime > 0) {
            std::stringstream timeStr;
            timeStr << (int)remainingTime << "s";
            
            // Panneau pour le temps restant avec police personnalisée
            const char* timeText = timeStr.str().c_str();
            Vector2 textSize = MeasureTextEx(gameFont, timeText, 13.0f, 1.0f);
            DrawRectangle(midX - textSize.x/2 - 5, midY + 30, (int)textSize.x + 10, 18, {0, 0, 0, 160});
            DrawRectangleLines(midX - textSize.x/2 - 5, midY + 30, (int)textSize.x + 10, 18, {255, 255, 255, 200});
            DrawTextEx(gameFont, timeText, {midX - textSize.x/2, midY + 33.0f}, 13.0f, 1.0f, {255, 220, 0, 255});
        }
    }
}

// Fonction pour dessiner un bouton - Texte plus grand avec police
bool DrawButton(const char* text, int x, int y, int width, int height, Color color, Color textColor, Font font) {
    Vector2 mousePos = GetMousePosition();
    bool hover = (mousePos.x >= x && mousePos.x <= x + width && 
                  mousePos.y >= y && mousePos.y <= y + height);
    bool clicked = hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    
    // Couleur selon l'état
    Color buttonColor = hover ? Color{color.r + 20, color.g + 20, color.b + 20, color.a} : color;
    
    // Dessiner le bouton
    DrawRectangle(x, y, width, height, buttonColor);
    DrawRectangleLines(x, y, width, height, hover ? WHITE : GRAY);
    
    // Centrer le texte - Plus grand avec police personnalisée
    float fontSize = 15.0f;
    float spacing = 1.0f; // Espacement par défaut
    Vector2 textSize = MeasureTextEx(font, text, fontSize, spacing);
    int textX = x + (width - (int)textSize.x) / 2;
    int textY = y + (height - (int)fontSize) / 2;
    DrawTextEx(font, text, {(float)textX, (float)textY}, fontSize, spacing, textColor);
    
    return clicked;
}

void Renderer::renderUI(const Simulation& simulation) {
    try {
    // Interface utilisateur en mode 2D terminé
    EndMode2D();
    
    // Panneau d'informations professionnel (fond semi-transparent) - EN HAUT À GAUCHE - AGRANDI
    int panelWidth = 320;  // Agrandi de 250 à 320
    int panelHeight = 240; // Agrandi de 200 à 240
    DrawRectangle(5, 5, panelWidth, panelHeight, {20, 20, 20, 220});
    DrawRectangleLines(5, 5, panelWidth, panelHeight, {100, 100, 100, 255});
    
    // Titre du panneau - Avec police personnalisée
    float titleSpacing = fontLoaded ? 2.0f : 1.5f;
    DrawTextEx(gameFont, "SIMULATION", {15.0f, 12.0f}, 20.0f, titleSpacing, {200, 200, 200, 255});
    DrawLineEx({15.0f, 32.0f}, {(float)(panelWidth - 10), 32.0f}, 1.5f, {100, 100, 100, 255});
    
    // Affichage des informations avec style - Texte plus grand
    std::stringstream ss;
    int yPos = 50;
    int lineHeight = 28; // Espacement augmenté
    
    // Temps - Texte plus grand avec police personnalisée
    float textSpacing = fontLoaded ? 1.5f : 1.0f;
    float textSize = 17.0f;
    DrawTextEx(gameFont, "Temps:", {15.0f, (float)yPos}, textSize, textSpacing, {150, 150, 150, 255});
    ss << (int)simulation.getSimulationTime() << "s";
    DrawTextEx(gameFont, ss.str().c_str(), {140.0f, (float)yPos}, textSize, textSpacing, WHITE);
    yPos += lineHeight;
    
    // Véhicules
    ss.str("");
    DrawTextEx(gameFont, "Vehicules:", {15.0f, (float)yPos}, textSize, textSpacing, {150, 150, 150, 255});
    ss << simulation.getVehicles().size();
    DrawTextEx(gameFont, ss.str().c_str(), {140.0f, (float)yPos}, textSize, textSpacing, {100, 200, 255, 255});
    yPos += lineHeight;
    
    // Événements actifs
    ss.str("");
    DrawTextEx(gameFont, "Evenements:", {15.0f, (float)yPos}, textSize, textSpacing, {150, 150, 150, 255});
    ss << simulation.getEvents().size();
    Color eventCountColor = simulation.getEvents().size() > 0 ? ORANGE : WHITE;
    DrawTextEx(gameFont, ss.str().c_str(), {140.0f, (float)yPos}, textSize, textSpacing, eventCountColor);
    yPos += lineHeight;
    
    // Reroutages
    ss.str("");
    DrawTextEx(gameFont, "Reroutages:", {15.0f, (float)yPos}, textSize, textSpacing, {150, 150, 150, 255});
    ss << simulation.getTotalReroutings();
    DrawTextEx(gameFont, ss.str().c_str(), {140.0f, (float)yPos}, textSize, textSpacing, {100, 255, 100, 255});
    yPos += lineHeight;
    
    // Mode
    ss.str("");
    DrawTextEx(gameFont, "Mode:", {15.0f, (float)yPos}, textSize, textSpacing, {150, 150, 150, 255});
    ss << (simulation.getMode() == SimulationMode::DYNAMIC ? "Dynamique" : "Normal");
    Color modeColor = simulation.getMode() == SimulationMode::DYNAMIC ? 
                      (Color){100, 255, 100, 255} : (Color){200, 200, 200, 255};
    DrawTextEx(gameFont, ss.str().c_str(), {140.0f, (float)yPos}, textSize, textSpacing, modeColor);
    yPos += lineHeight;
    
    // État pause/play
    ss.str("");
    DrawTextEx(gameFont, "Etat:", {15.0f, (float)yPos}, textSize, textSpacing, {150, 150, 150, 255});
    ss << (simulation.getIsPaused() ? "PAUSE" : "EN COURS");
    DrawTextEx(gameFont, ss.str().c_str(), {140.0f, (float)yPos}, textSize, textSpacing, simulation.getIsPaused() ? ORANGE : GREEN);
    yPos += lineHeight;
    
    // Vitesse de simulation
    ss.str("");
    DrawTextEx(gameFont, "Vitesse:", {15.0f, (float)yPos}, textSize, textSpacing, {150, 150, 150, 255});
    ss << std::fixed << std::setprecision(1) << simulation.getTimeScale() << "x";
    DrawTextEx(gameFont, ss.str().c_str(), {140.0f, (float)yPos}, textSize, textSpacing, YELLOW);
    
    // BOUTONS DE CONTRÔLE - En bas du panneau - Plus grands
    int buttonY = 210;
    int buttonWidth = 75;  // Agrandi de 70 à 75
    int buttonHeight = 28; // Agrandi de 25 à 28
    const int buttonSpacing = 5; // Espacement entre les boutons
    
    // Bouton Play/Pause - Plus grand
    int buttonX = 15;
    Color playPauseColor = simulation.getIsPaused() ? GREEN : ORANGE;
    DrawButton(simulation.getIsPaused() ? "PLAY" : "PAUSE", buttonX, buttonY, buttonWidth, buttonHeight, playPauseColor, WHITE, gameFont);
    buttonX += buttonWidth + buttonSpacing;
    
    // Bouton Vitesse - - Plus grand
    DrawButton("-", buttonX, buttonY, 35, buttonHeight, {100, 100, 100, 255}, WHITE, gameFont);
    buttonX += 35 + buttonSpacing;
    
    // Bouton Vitesse + - Plus grand
    DrawButton("+", buttonX, buttonY, 35, buttonHeight, {100, 100, 100, 255}, WHITE, gameFont);
    buttonX += 35 + buttonSpacing;
    
    // Bouton Mode - Plus grand
    Color modeButtonColor = simulation.getMode() == SimulationMode::DYNAMIC ? 
                           (Color){100, 200, 100, 255} : (Color){200, 200, 200, 255};
    DrawButton("MODE", buttonX, buttonY, 70, buttonHeight, modeButtonColor, WHITE, gameFont);
    buttonX += 70 + buttonSpacing;
    
    // Bouton Musique - Nouveau
    Color musicButtonColor = isMusicPlaying() ? 
                            (Color){100, 150, 255, 255} : (Color){150, 150, 150, 255};
    DrawButton("MUSIC", buttonX, buttonY, 70, buttonHeight, musicButtonColor, WHITE, gameFont);
    
    // Panneau des événements actifs (à droite) - AGRANDI
    const auto& events = simulation.getEvents();
    int eventPanelWidth = 350;  // Agrandi de 275 à 350
    int eventPanelX = screenWidth - eventPanelWidth - 5;
    int eventPanelY = 5;
    int eventPanelHeight = std::min(280, (int)events.size() * 60 + 50); // Plus haut et plus d'espace
    
    if (!events.empty()) {
        DrawRectangle(eventPanelX, eventPanelY, eventPanelWidth, eventPanelHeight, {20, 20, 20, 220});
        DrawRectangleLines(eventPanelX, eventPanelY, eventPanelWidth, eventPanelHeight, {100, 100, 100, 255});
        DrawTextEx(gameFont, "EVENEMENTS ACTIFS", {(float)(eventPanelX + 10), (float)(eventPanelY + 12)}, 20.0f, 1.5f, {200, 200, 200, 255});
        DrawLineEx({(float)(eventPanelX + 10), (float)(eventPanelY + 32)}, {(float)(eventPanelX + eventPanelWidth - 10), (float)(eventPanelY + 32)}, 1.5f, {100, 100, 100, 255});
        
        int eventY = eventPanelY + 45;
        int eventIndex = 0;
        for (const auto& event : events) {
            if (!event || !event->isActive() || eventIndex >= 4) break; // Max 4 événements affichés (augmenté)
            
            Color eventTypeColor;
            std::string eventName;
            switch (event->getType()) {
                case EventType::ACCIDENT:
                    eventTypeColor = RED;
                    eventName = "ACCIDENT";
                    break;
                case EventType::TRAFFIC_JAM:
                    eventTypeColor = ORANGE;
                    eventName = "EMBOUTEILLAGE";
                    break;
                case EventType::ROAD_CLOSURE:
                    eventTypeColor = MAROON;
                    eventName = "ROUTE FERMEE";
                    break;
                case EventType::EMERGENCY:
                    eventTypeColor = BLUE;
                    eventName = "URGENCE";
                    break;
            }
            
            // Indicateur de couleur - Plus large
            DrawRectangle(eventPanelX + 15, eventY, 6, 35, eventTypeColor);
            
            // Nom de l'événement - Plus grand avec police personnalisée
            DrawTextEx(gameFont, eventName.c_str(), {(float)(eventPanelX + 25), (float)(eventY + 5)}, 16.0f, 1.0f, WHITE);
            
            // Durée restante - Plus grand avec police personnalisée
            float remaining = event->getDuration() - event->getElapsedTime();
            ss.str("");
            ss << (int)remaining << "s restantes";
            DrawTextEx(gameFont, ss.str().c_str(), {(float)(eventPanelX + 25), (float)(eventY + 22)}, 14.0f, 1.0f, {150, 150, 150, 255});
            
            eventY += 60; // Plus d'espace entre les événements
            eventIndex++;
        }
    }
    
    // Légende améliorée (en bas à gauche) - AGRANDIE
    int legendWidth = 280;  // Agrandi de 200 à 280
    int legendHeight = 180; // Agrandi de 145 à 180
    int legendY = screenHeight - legendHeight - 5;
    DrawRectangle(5, legendY, legendWidth, legendHeight, {20, 20, 20, 220});
    DrawRectangleLines(5, legendY, legendWidth, legendHeight, {100, 100, 100, 255});
    DrawTextEx(gameFont, "LEGENDE", {15.0f, (float)(legendY + 5)}, 20.0f, 1.5f, {200, 200, 200, 255});
    DrawLineEx({15.0f, (float)(legendY + 25)}, {(float)(legendWidth - 10), (float)(legendY + 25)}, 1.5f, {100, 100, 100, 255});
    
    int legY = legendY + 35;
    int legLineHeight = 32; // Espacement augmenté
    float legTextSize = 15.0f;
    DrawLineEx({15.0f, (float)(legY)}, {40.0f, (float)(legY)}, 5.0f, {220, 220, 220, 255});
    DrawTextEx(gameFont, "Route normale", {45.0f, (float)(legY - 10)}, legTextSize, 1.0f, WHITE);
    legY += legLineHeight;
    
    DrawLineEx({15.0f, (float)(legY)}, {40.0f, (float)(legY)}, 5.0f, {255, 200, 0, 255});
    DrawTextEx(gameFont, "Route congestionnee", {45.0f, (float)(legY - 10)}, legTextSize, 1.0f, WHITE);
    legY += legLineHeight;
    
    DrawLineEx({15.0f, (float)(legY)}, {40.0f, (float)(legY)}, 5.0f, {255, 50, 50, 255});
    DrawTextEx(gameFont, "Route bloquee", {45.0f, (float)(legY - 10)}, legTextSize, 1.0f, WHITE);
    legY += legLineHeight;
    
    // Types de véhicules - Plus grands avec police personnalisée
    DrawRectangle(15, legY - 4, 10, 14, {100, 150, 200, 255});
    DrawTextEx(gameFont, "Voiture", {30.0f, (float)(legY - 10)}, legTextSize, 1.0f, WHITE);
    legY += legLineHeight;
    
    DrawRectangle(15, legY - 4, 10, 14, {150, 100, 50, 255});
    DrawTextEx(gameFont, "Camion", {30.0f, (float)(legY - 10)}, legTextSize, 1.0f, WHITE);
    legY += legLineHeight;
    
    DrawRectangle(15, legY - 4, 10, 14, {255, 255, 0, 255});
    DrawTextEx(gameFont, "Taxi", {30.0f, (float)(legY - 10)}, legTextSize, 1.0f, WHITE);
    
    // Instructions (en bas) - Plus grande avec police personnalisée
    int controlBarHeight = 35;
    DrawRectangle(5, screenHeight - controlBarHeight - 5, screenWidth - 10, controlBarHeight, {0, 0, 0, 200});
    DrawTextEx(gameFont, "SPACE: Evenement | R: Mode | +/-: Vehicules | ESC: Quitter", 
               {15.0f, (float)(screenHeight - controlBarHeight + 8)}, 15.0f, 1.0f, YELLOW);
    
    BeginMode2D(camera);
    } catch (const std::exception& e) {
        // En cas d'erreur, au moins remettre le mode 2D
        BeginMode2D(camera);
    } catch (...) {
        BeginMode2D(camera);
    }
}

void Renderer::handleButtonClicks(Simulation& simulation) {
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return;
    }
    
    Vector2 mousePos = GetMousePosition();
    
    // Coordonnées des boutons (doivent correspondre exactement à renderUI)
    int buttonY = 210;
    int buttonWidth = 75;
    int buttonHeight = 28;
    
    // Bouton Play/Pause (15, 210, 75, 28)
    if (mousePos.x >= 15 && mousePos.x <= 15 + buttonWidth && 
        mousePos.y >= buttonY && mousePos.y <= buttonY + buttonHeight) {
        simulation.togglePause();
        std::cout << "Bouton Play/Pause clique!" << std::endl;
        return;
    }
    
    // Bouton Vitesse - (95, 210, 35, 28)
    if (mousePos.x >= 95 && mousePos.x <= 95 + 35 && 
        mousePos.y >= buttonY && mousePos.y <= buttonY + buttonHeight) {
        float newScale = simulation.getTimeScale() - 0.5f;
        simulation.setTimeScale(newScale);
        std::cout << "Vitesse reduite a " << newScale << "x" << std::endl;
        return;
    }
    
    // Bouton Vitesse + (135, 210, 35, 28)
    if (mousePos.x >= 135 && mousePos.x <= 135 + 35 && 
        mousePos.y >= buttonY && mousePos.y <= buttonY + buttonHeight) {
        float newScale = simulation.getTimeScale() + 0.5f;
        simulation.setTimeScale(newScale);
        std::cout << "Vitesse augmentee a " << newScale << "x" << std::endl;
        return;
    }
    
    // Bouton Mode (175, 210, 70, 28) - Coordonnées corrigées
    if (mousePos.x >= 175 && mousePos.x <= 175 + 70 && 
        mousePos.y >= buttonY && mousePos.y <= buttonY + buttonHeight) {
        SimulationMode currentMode = simulation.getMode();
        SimulationMode newMode = (currentMode == SimulationMode::DYNAMIC) ? 
                                SimulationMode::NORMAL : SimulationMode::DYNAMIC;
        simulation.setMode(newMode);
        std::cout << "Mode change: " << (newMode == SimulationMode::DYNAMIC ? "Dynamique" : "Normal") << std::endl;
        return;
    }
    
    // Bouton Musique (250, 210, 70, 28) - Coordonnées corrigées
    if (mousePos.x >= 250 && mousePos.x <= 250 + 70 && 
        mousePos.y >= buttonY && mousePos.y <= buttonY + buttonHeight) {
        toggleMusic();
        std::cout << "Bouton Musique clique!" << std::endl;
        return;
    }
}

void Renderer::updateCamera() {
    // Zoom avec la molette
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        camera.zoom += wheel * 0.1f;
        camera.zoom = std::max(0.1f, std::min(3.0f, camera.zoom));
    }
    
    // Déplacement avec les flèches ou WASD
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        camera.target.x += 5.0f / camera.zoom;
    }
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        camera.target.x -= 5.0f / camera.zoom;
    }
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        camera.target.y -= 5.0f / camera.zoom;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        camera.target.y += 5.0f / camera.zoom;
    }
}

void Renderer::setCameraTarget(float x, float y) {
    camera.target.x = x;
    camera.target.y = y;
}

void Renderer::fitGraphToWindow(const Graph& graph) {
    // Calculer les limites du graphe
    float minX = 0.0f, maxX = 0.0f, minY = 0.0f, maxY = 0.0f;
    bool first = true;
    
    const auto& nodes = graph.getNodes();
    for (const auto& node : nodes) {
        if (first) {
            minX = maxX = node->x;
            minY = maxY = node->y;
            first = false;
        } else {
            minX = std::min(minX, node->x);
            maxX = std::max(maxX, node->x);
            minY = std::min(minY, node->y);
            maxY = std::max(maxY, node->y);
        }
    }
    
    // Ajouter une marge autour du graphe pour une meilleure visibilité
    float margin = 80.0f;
    minX -= margin;
    maxX += margin;
    minY -= margin;
    maxY += margin;
    
    // Calculer le centre du graphe
    float centerX = (minX + maxX) / 2.0f;
    float centerY = (minY + maxY) / 2.0f;
    
    // Calculer les dimensions du graphe
    float graphWidth = maxX - minX;
    float graphHeight = maxY - minY;
    
    // Calculer l'espace disponible (en tenant compte des panneaux UI)
    // Panneaux: 320px gauche, 350px droite, 240px haut, 180px bas
    float availableWidth = screenWidth - 670.0f;  // 320px gauche + 350px droite
    float availableHeight = screenHeight - 420.0f; // 240px haut + 180px bas
    
    // Calculer le zoom pour que le graphe remplisse la fenêtre
    float zoomX = availableWidth / graphWidth;
    float zoomY = availableHeight / graphHeight;
    float zoom = std::min(zoomX, zoomY);
    
    // Limiter le zoom pour éviter qu'il soit trop petit ou trop grand
    zoom = std::max(0.5f, std::min(3.0f, zoom));
    
    // Appliquer le zoom et centrer
    camera.zoom = zoom;
    camera.target.x = centerX;
    camera.target.y = centerY;
    camera.offset.x = screenWidth / 2.0f;
    camera.offset.y = screenHeight / 2.0f;
    
    std::cout << "✅ Camera ajustee pour afficher la ville:" << std::endl;
    std::cout << "   Centre: (" << centerX << ", " << centerY << ")" << std::endl;
    std::cout << "   Dimensions: " << graphWidth << " x " << graphHeight << std::endl;
    std::cout << "   Zoom: " << zoom << std::endl;
}

Vector2 Renderer::worldToScreen(float x, float y) const {
    Vector2 result;
    result.x = (x - camera.target.x) * camera.zoom + camera.offset.x;
    result.y = (y - camera.target.y) * camera.zoom + camera.offset.y;
    return result;
}

Vector2 Renderer::screenToWorld(int x, int y) const {
    Vector2 result;
    result.x = (x - camera.offset.x) / camera.zoom + camera.target.x;
    result.y = (y - camera.offset.y) / camera.zoom + camera.target.y;
    return result;
}

void Renderer::addNotification(const std::string& text, Color color, float duration) {
    Notification notif;
    notif.text = text;
    notif.color = color;
    notif.time = 0.0f;
    notif.duration = duration;
    notifications.push_back(notif);
}

void Renderer::updateNotifications(float deltaTime) {
    try {
        if (!std::isfinite(deltaTime) || deltaTime < 0) return;
        
        for (auto it = notifications.begin(); it != notifications.end();) {
            if (!std::isfinite(it->time) || !std::isfinite(it->duration)) {
                it = notifications.erase(it);
                continue;
            }
            
            it->time += deltaTime;
            if (it->time >= it->duration) {
                it = notifications.erase(it);
            } else {
                ++it;
            }
        }
    } catch (...) {
        // En cas d'erreur, vider les notifications
        notifications.clear();
    }
}

void Renderer::renderNotifications() {
    try {
        if (notifications.empty()) return;
        
        float startY = 200.0f;
        float spacing = 35.0f;
        
        size_t maxNotifications = std::min(notifications.size(), size_t(5));
        for (size_t i = 0; i < maxNotifications; i++) {
            if (i >= notifications.size()) break;
            
            const auto& notif = notifications[i];
            if (notif.text.empty()) continue;
            
            float alpha = 255.0f;
            if (notif.duration > 0.5f && notif.time > notif.duration - 0.5f) {
                // Fade out
                float fadeTime = notif.time - (notif.duration - 0.5f);
                alpha = std::max(0.0f, 255.0f * (1.0f - fadeTime / 0.5f));
            }
            
            float y = startY + i * spacing;
            if (y < 0 || y > screenHeight) continue;
            
            int textWidth = MeasureText(notif.text.c_str(), 16);
            if (textWidth <= 0) textWidth = 100; // Valeur par défaut
            
            // Limiter la taille pour éviter les débordements
            int maxWidth = screenWidth - 40;
            if (textWidth > maxWidth) textWidth = maxWidth;
            
            // Fond semi-transparent
            unsigned char bgAlpha = static_cast<unsigned char>(std::min(255.0f, alpha * 0.9f));
            Color bgColor = {20, 20, 20, bgAlpha};
            DrawRectangle(15, (int)y - 2, textWidth + 20, 28, bgColor);
            
            unsigned char borderAlpha = static_cast<unsigned char>(std::min(255.0f, alpha));
            DrawRectangleLines(15, (int)y - 2, textWidth + 20, 28, {100, 100, 100, borderAlpha});
            
            // Indicateur de couleur
            Color indicatorColor = notif.color;
            indicatorColor.a = static_cast<unsigned char>(std::min(255.0f, alpha));
            DrawRectangle(17, (int)y, 4, 24, indicatorColor);
            
            // Texte avec police personnalisée
            unsigned char textAlpha = static_cast<unsigned char>(std::min(255.0f, alpha));
            Color textColor = {notif.color.r, notif.color.g, notif.color.b, textAlpha};
            float notifSpacing = 1.0f;
            DrawTextEx(gameFont, notif.text.c_str(), {25.0f, (float)((int)y + 4)}, 17.0f, notifSpacing, textColor);
        }
    } catch (...) {
        // Ignorer les erreurs de rendu des notifications
    }
}

void Renderer::loadTextures() {
    // Vider le cache
    vehicleTextureCache.clear();
    
    // Charger toutes les textures des véhicules avec directions dans le cache
    std::vector<std::string> directions = {"haut", "bas", "gauche", "droite"};
    std::vector<std::string> vehicleTypes = {
        "voiture_bleue", "voiture_rouge", "camion_rouge", "taxi_jaune"
    };
    
    std::cout << "=== CHARGEMENT DES TEXTURES DE VEHICULES ===" << std::endl;
    int loadedCount = 0;
    
    // Charger toutes les combinaisons possibles
    for (const auto& vehicleType : vehicleTypes) {
        for (const auto& direction : directions) {
            std::string key = vehicleType + "_vers_" + direction;
            
            // Pour "haut", essayer aussi "haute" (nom de fichier réel)
            std::vector<std::string> fileVariants;
            if (direction == "haut") {
                fileVariants.push_back("assets/vehicles/" + vehicleType + "_vers_haute.png");
                fileVariants.push_back("assets/vehicles/" + vehicleType + "_vers_haut.png");
                fileVariants.push_back("../assets/vehicles/" + vehicleType + "_vers_haute.png");
                fileVariants.push_back("../assets/vehicles/" + vehicleType + "_vers_haut.png");
            } else {
                fileVariants.push_back("assets/vehicles/" + vehicleType + "_vers_" + direction + ".png");
                fileVariants.push_back("../assets/vehicles/" + vehicleType + "_vers_" + direction + ".png");
            }
            
            bool loaded = false;
            for (const auto& file : fileVariants) {
                if (FileExists(file.c_str())) {
                    Texture2D texture = LoadTexture(file.c_str());
                    if (texture.id != 0) {
                        SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
                        vehicleTextureCache[key] = texture;
                        std::cout << "✅ " << key << " <- " << file << std::endl;
                        loaded = true;
                        loadedCount++;
                        break;
                    }
                }
            }
            
            if (!loaded) {
                std::cout << "⚠️  " << key << " (non trouve)" << std::endl;
            }
        }
    }
    
    std::cout << "Total textures vehicules chargees: " << loadedCount << "/" << (vehicleTypes.size() * directions.size()) << std::endl;
    
    // Anciens noms pour compatibilité
    std::vector<std::string> carFiles = {
        "assets/vehicles/car1.png",
        "assets/vehicles/car2.png",
        "assets/vehicles/car.png"
    };
    
    std::vector<std::string> truckFiles = {
        "assets/vehicles/truc1.png",
        "assets/vehicles/truck1.png",
        "assets/vehicles/truck.png"
    };
    
    std::vector<std::string> busFiles = {
        "assets/vehicles/bus1.png",
        "assets/vehicles/bus2.png",
        "assets/vehicles/bus.png"
    };
    
    // Afficher le répertoire de travail actuel pour diagnostic
    const char* workingDir = GetWorkingDirectory();
    std::cout << "=== CHARGEMENT DES TEXTURES ===" << std::endl;
    std::cout << "Repertoire de travail: " << (workingDir ? workingDir : "inconnu") << std::endl;
    
    // Charger textures par défaut (fallback)
    bool carLoaded = false;
    for (const auto& file : carFiles) {
        if (FileExists(file.c_str())) {
            vehicleCarTexture = LoadTexture(file.c_str());
            if (vehicleCarTexture.id != 0) {
                SetTextureFilter(vehicleCarTexture, TEXTURE_FILTER_BILINEAR);
                carLoaded = true;
                break;
            }
        }
    }
    
    bool truckLoaded = false;
    for (const auto& file : truckFiles) {
        if (FileExists(file.c_str())) {
            vehicleTruckTexture = LoadTexture(file.c_str());
            if (vehicleTruckTexture.id != 0) {
                SetTextureFilter(vehicleTruckTexture, TEXTURE_FILTER_BILINEAR);
                truckLoaded = true;
                break;
            }
        }
    }
    
    bool busLoaded = false;
    for (const auto& file : busFiles) {
        if (FileExists(file.c_str())) {
            vehicleBusTexture = LoadTexture(file.c_str());
            if (vehicleBusTexture.id != 0) {
                SetTextureFilter(vehicleBusTexture, TEXTURE_FILTER_BILINEAR);
                busLoaded = true;
                break;
            }
        }
    }
    
    std::cout << "✅ Cache de textures: " << vehicleTextureCache.size() << " textures chargees" << std::endl;
    
  
    std::vector<std::string> accidentFiles = {
        "assets/events/Accident.png",
        "../assets/events/Accident.png",
        "assets/events/accident.png",
        "../assets/events/accident.png"
    };
    for (const auto& file : accidentFiles) {
        if (FileExists(file.c_str())) {
            eventAccidentTexture = LoadTexture(file.c_str());
            if (eventAccidentTexture.id != 0) {
                SetTextureFilter(eventAccidentTexture, TEXTURE_FILTER_BILINEAR);
                std::cout << "✅ Texture accident chargee: " << file << std::endl;
                break;
            }
        }
    }
    
    std::vector<std::string> trafficJamFiles = {
        "assets/events/embouteillage.png",
        "../assets/events/embouteillage.png",
        "assets/events/Embouteillage.png",
        "../assets/events/Embouteillage.png",
        "assets/events/traffic_jam.png",
        "../assets/events/traffic_jam.png"
    };
    for (const auto& file : trafficJamFiles) {
        if (FileExists(file.c_str())) {
            eventTrafficJamTexture = LoadTexture(file.c_str());
            if (eventTrafficJamTexture.id != 0) {
                SetTextureFilter(eventTrafficJamTexture, TEXTURE_FILTER_BILINEAR);
                std::cout << "✅ Texture embouteillage chargee: " << file << std::endl;
                break;
            }
        }
    }
    
    std::vector<std::string> emergencyFiles = {
        "assets/events/urgance.png",
        "../assets/events/urgance.png",
        "assets/events/emergency.png",
        "../assets/events/emergency.png"
    };
    for (const auto& file : emergencyFiles) {
        if (FileExists(file.c_str())) {
            eventEmergencyTexture = LoadTexture(file.c_str());
            if (eventEmergencyTexture.id != 0) {
                SetTextureFilter(eventEmergencyTexture, TEXTURE_FILTER_BILINEAR);
                std::cout << "✅ Texture urgence chargee: " << file << std::endl;
                break;
            }
        }
    }
    
    texturesLoaded = (vehicleCarTexture.id != 0 || vehicleTruckTexture.id != 0 || vehicleBusTexture.id != 0);
    
    if (texturesLoaded) {
        std::cout << "Textures chargees avec succes!" << std::endl;
    } else {
        std::cout << "Aucune texture trouvee - utilisation du rendu par defaut (formes geometriques)" << std::endl;
    }
}

void Renderer::loadMusic() {
    std::vector<std::string> musicPaths = {
        "assets/music/Music.mp3",
        "assets/music/music.mp3",
        "../assets/music/Music.mp3",
        "build/assets/music/Music.mp3"
    };
    
    musicLoaded = false;
    for (const auto& path : musicPaths) {
        if (FileExists(path.c_str())) {
            std::cout << "Tentative de chargement de la musique: " << path << std::endl;
            backgroundMusic = LoadMusicStream(path.c_str());
            if (IsMusicValid(backgroundMusic)) {
                musicLoaded = true;
        
                backgroundMusic.looping = true;
                
                SetMusicVolume(backgroundMusic, 0.5f);
                
                PlayMusicStream(backgroundMusic);
                std::cout << "✅ Musique chargee et lancee: " << path << std::endl;
                std::cout << "   Duree: " << GetMusicTimeLength(backgroundMusic) << " secondes" << std::endl;
                break;
            } else {
                std::cout << "❌ Echec du chargement de la musique: " << path << std::endl;
            }
        }
    }
    
    if (!musicLoaded) {
        std::cout << "⚠️  Aucune musique trouvee. Placez Music.mp3 dans assets/music/" << std::endl;
    }
}

void Renderer::updateMusic() {
    // Mettre à jour la musique (nécessaire pour le streaming)
    if (musicLoaded && IsMusicStreamPlaying(backgroundMusic)) {
        UpdateMusicStream(backgroundMusic);
    }
}

void Renderer::toggleMusic() {
    if (!musicLoaded) {
        std::cout << "⚠️  Aucune musique chargee" << std::endl;
        return;
    }
    
    if (IsMusicStreamPlaying(backgroundMusic)) {
        PauseMusicStream(backgroundMusic);
        std::cout << "🔇 Musique en pause" << std::endl;
    } else {
        ResumeMusicStream(backgroundMusic);
        std::cout << "🔊 Musique reprise" << std::endl;
    }
}

bool Renderer::isMusicPlaying() const {
    if (!musicLoaded) return false;
    return IsMusicStreamPlaying(backgroundMusic);
}

void Renderer::unloadTextures() {
   
    for (auto& pair : vehicleTextureCache) {
        if (pair.second.id != 0) {
            UnloadTexture(pair.second);
        }
    }
    vehicleTextureCache.clear();
    
    if (vehicleCarTexture.id != 0) {
        UnloadTexture(vehicleCarTexture);
    }
    if (vehicleTruckTexture.id != 0) {
        UnloadTexture(vehicleTruckTexture);
    }
    if (vehicleBusTexture.id != 0) {
        UnloadTexture(vehicleBusTexture);
    }
    if (eventAccidentTexture.id != 0) {
        UnloadTexture(eventAccidentTexture);
    }
    if (eventTrafficJamTexture.id != 0) {
        UnloadTexture(eventTrafficJamTexture);
    }
    if (eventRoadClosureTexture.id != 0) {
        UnloadTexture(eventRoadClosureTexture);
    }
    if (eventEmergencyTexture.id != 0) {
        UnloadTexture(eventEmergencyTexture);
    }
    texturesLoaded = false;
}

void Renderer::loadMapTiles() {
    std::cout << "=== CHARGEMENT DES TILES DE CARTE ===" << std::endl;
    
    
    std::vector<std::string> tilemapFiles = {
        "assets/map/kenney_roguelike-modern-city/Tilemap/tilemap.png",
        "../assets/map/kenney_roguelike-modern-city/Tilemap/tilemap.png",
        "assets/map/kenney_roguelike-modern-city/Tilemap/tilemap_packed.png",
        "../assets/map/kenney_roguelike-modern-city/Tilemap/tilemap_packed.png"
    };
    
    for (const auto& file : tilemapFiles) {
        if (FileExists(file.c_str())) {
            tilemapTexture = LoadTexture(file.c_str());
            if (tilemapTexture.id != 0) {
                SetTextureFilter(tilemapTexture, TEXTURE_FILTER_BILINEAR);
                std::cout << "✅ Tilemap charge: " << file << " (" 
                          << tilemapTexture.width << "x" << tilemapTexture.height << ")" << std::endl;
                tileSize = 16; 
                break;
            }
        }
    }
    
   
    std::vector<std::string> roadStraightFiles = {
        "assets/routes/route_horizontale_droite.png",
        "../assets/routes/route_horizontale_droite.png",
        "assets/routes/route_verticale_droite.png",
        "../assets/routes/route_verticale_droite.png",
        "assets/routes/road-straight.png",
        "../assets/routes/road-straight.png",
        "assets/routes/road_h.png",
        "../assets/routes/road_h.png",
        "assets/routes/road_v.png",
        "../assets/routes/road_v.png",
        "assets/map/kenney_city-kit-roads/Previews/road-straight.png",
        "../assets/map/kenney_city-kit-roads/Previews/road-straight.png"
    };
    
    // Charger route horizontale
    std::vector<std::string> roadHorizontalFiles = {
        "assets/routes/route_horizontale_droite.png",
        "../assets/routes/route_horizontale_droite.png"
    };
    for (const auto& file : roadHorizontalFiles) {
        if (FileExists(file.c_str())) {
            roadHorizontalTexture = LoadTexture(file.c_str());
            if (roadHorizontalTexture.id != 0) {
                SetTextureFilter(roadHorizontalTexture, TEXTURE_FILTER_BILINEAR);
                std::cout << "✅ Route horizontale chargee: " << file << " (" 
                          << roadHorizontalTexture.width << "x" << roadHorizontalTexture.height << ")" << std::endl;
                break;
            }
        }
    }
    
    // Charger route verticale
    std::vector<std::string> roadVerticalFiles = {
        "assets/routes/route_verticale_droite.png",
        "../assets/routes/route_verticale_droite.png"
    };
    for (const auto& file : roadVerticalFiles) {
        if (FileExists(file.c_str())) {
            roadVerticalTexture = LoadTexture(file.c_str());
            if (roadVerticalTexture.id != 0) {
                SetTextureFilter(roadVerticalTexture, TEXTURE_FILTER_BILINEAR);
                std::cout << "✅ Route verticale chargee: " << file << " (" 
                          << roadVerticalTexture.width << "x" << roadVerticalTexture.height << ")" << std::endl;
                break;
            }
        }
    }
    
    // Charger route droite (fallback)
    for (const auto& file : roadStraightFiles) {
        if (FileExists(file.c_str())) {
            roadStraightTexture = LoadTexture(file.c_str());
            if (roadStraightTexture.id != 0) {
                SetTextureFilter(roadStraightTexture, TEXTURE_FILTER_BILINEAR);
                std::cout << "✅ Route droite chargee: " << file << " (" 
                          << roadStraightTexture.width << "x" << roadStraightTexture.height << ")" << std::endl;
                break;
            }
        }
    }
    
    // Charger virage
    std::vector<std::string> roadCurveFiles = {
        "assets/routes/virage_route_bas_vers_droite.png",
        "../assets/routes/virage_route_bas_vers_droite.png",
        "assets/routes/coin_route_bas_vers_droite.png",
        "../assets/routes/coin_route_bas_vers_droite.png"
    };
    for (const auto& file : roadCurveFiles) {
        if (FileExists(file.c_str())) {
            roadCurveTexture = LoadTexture(file.c_str());
            if (roadCurveTexture.id != 0) {
                SetTextureFilter(roadCurveTexture, TEXTURE_FILTER_BILINEAR);
                std::cout << "✅ Virage charge: " << file << std::endl;
                break;
            }
        }
    }
    
    // Charger rond-point
    std::vector<std::string> roadRoundaboutFiles = {
        "assets/routes/rond_point_quatre_directions.png",
        "../assets/routes/rond_point_quatre_directions.png"
    };
    for (const auto& file : roadRoundaboutFiles) {
        if (FileExists(file.c_str())) {
            roadRoundaboutTexture = LoadTexture(file.c_str());
            if (roadRoundaboutTexture.id != 0) {
                SetTextureFilter(roadRoundaboutTexture, TEXTURE_FILTER_BILINEAR);
                std::cout << "✅ Rond-point charge: " << file << std::endl;
                break;
            }
        }
    }
    
    std::vector<std::string> roadIntersectionFiles = {
        "assets/routes/rond_point_quatre_directions.png",
        "../assets/routes/rond_point_quatre_directions.png",
        "assets/routes/road-intersection.png",
        "../assets/routes/road-intersection.png",
        "assets/routes/intersection.png",
        "../assets/routes/intersection.png",
        "assets/routes/crossroad.png",
        "../assets/routes/crossroad.png",
        "assets/map/kenney_city-kit-roads/Previews/road-intersection.png",
        "../assets/map/kenney_city-kit-roads/Previews/road-intersection.png",
        "assets/map/kenney_city-kit-roads/Previews/road-crossroad.png",
        "../assets/map/kenney_city-kit-roads/Previews/road-crossroad.png"
    };
    
    for (const auto& file : roadIntersectionFiles) {
        if (FileExists(file.c_str())) {
            roadIntersectionTexture = LoadTexture(file.c_str());
            if (roadIntersectionTexture.id != 0) {
                SetTextureFilter(roadIntersectionTexture, TEXTURE_FILTER_BILINEAR);
                std::cout << "✅ Intersection chargee: " << file << std::endl;
                break;
            }
        }
    }
    
    // Charger une tuile d'herbe depuis Roguelike Modern City
    std::vector<std::string> grassFiles = {
        "assets/map/kenney_roguelike-modern-city/Tiles/tile_0000.png",
        "../assets/map/kenney_roguelike-modern-city/Tiles/tile_0000.png"
    };
    
    for (const auto& file : grassFiles) {
        if (FileExists(file.c_str())) {
            grassTexture = LoadTexture(file.c_str());
            if (grassTexture.id != 0) {
                SetTextureFilter(grassTexture, TEXTURE_FILTER_BILINEAR);
                std::cout << "✅ Herbe chargee: " << file << " (" 
                          << grassTexture.width << "x" << grassTexture.height << ")" << std::endl;
                break;
            }
        } else {
            std::cout << "Fichier non trouve: " << file << std::endl;
        }
    }
    
    mapTilesLoaded = (tilemapTexture.id != 0 || roadStraightTexture.id != 0 || 
                      roadHorizontalTexture.id != 0 || roadVerticalTexture.id != 0 || 
                      roadIntersectionTexture.id != 0 || roadRoundaboutTexture.id != 0 || 
                      grassTexture.id != 0);
    
    if (mapTilesLoaded) {
        std::cout << "✅ Tiles de carte chargees avec succes!" << std::endl;
    } else {
        std::cout << "⚠️ Aucune tile de carte trouvee - utilisation du rendu par defaut" << std::endl;
    }
}

void Renderer::unloadMapTiles() {
    if (tilemapTexture.id != 0) {
        UnloadTexture(tilemapTexture);
    }
    if (roadStraightTexture.id != 0) {
        UnloadTexture(roadStraightTexture);
    }
    if (roadIntersectionTexture.id != 0) {
        UnloadTexture(roadIntersectionTexture);
    }
    if (roadHorizontalTexture.id != 0) {
        UnloadTexture(roadHorizontalTexture);
    }
    if (roadVerticalTexture.id != 0) {
        UnloadTexture(roadVerticalTexture);
    }
    if (roadCurveTexture.id != 0) {
        UnloadTexture(roadCurveTexture);
    }
    if (roadRoundaboutTexture.id != 0) {
        UnloadTexture(roadRoundaboutTexture);
    }
    if (grassTexture.id != 0) {
        UnloadTexture(grassTexture);
    }
    if (pavementTexture.id != 0) {
        UnloadTexture(pavementTexture);
    }
    if (buildingTexture.id != 0) {
        UnloadTexture(buildingTexture);
    }
    mapTilesLoaded = false;
}

void Renderer::renderMapBackground(const Graph& graph) {
    // Calculer les bornes de la carte
    float minX = 0, maxX = 0, minY = 0, maxY = 0;
    bool first = true;
    
    for (const auto& node : graph.getNodes()) {
        if (!node) continue;
        if (first) {
            minX = maxX = node->x;
            minY = maxY = node->y;
            first = false;
        } else {
            minX = std::min(minX, node->x);
            maxX = std::max(maxX, node->x);
            minY = std::min(minY, node->y);
            maxY = std::max(maxY, node->y);
        }
    }
    
    // Si pas de nœuds, utiliser une zone par défaut
    if (first) {
        minX = -500;
        maxX = 500;
        minY = -500;
        maxY = 500;
    }
    
    minX -= 300;
    maxX += 300;
    minY -= 300;
    maxY += 300;
    
    if (grassTexture.id != 0) {
        float tileWorldSize = 100.0f; 
        float startX = floorf(minX / tileWorldSize) * tileWorldSize;
        float startY = floorf(minY / tileWorldSize) * tileWorldSize;
        
        for (float y = startY; y <= maxY; y += tileWorldSize) {
            for (float x = startX; x <= maxX; x += tileWorldSize) {
                Rectangle source = {0, 0, (float)grassTexture.width, (float)grassTexture.height};
                Rectangle dest = {x, y, tileWorldSize, tileWorldSize};
                DrawTexturePro(grassTexture, source, dest, {0, 0}, 0.0f, WHITE);
            }
        }
    } else {
       
        DrawRectangle((int)minX, (int)minY, (int)(maxX - minX), (int)(maxY - minY), {34, 139, 34, 255});
    }
}

void Renderer::renderRoadTile(float x, float y, const std::string& tileType, float rotation) {
    Texture2D* texture = nullptr;
    
    if (tileType == "horizontal") {
        if (roadHorizontalTexture.id != 0) {
            texture = &roadHorizontalTexture;
        } else if (roadStraightTexture.id != 0) {
            texture = &roadStraightTexture;
        }
    } else if (tileType == "vertical") {
        if (roadVerticalTexture.id != 0) {
            texture = &roadVerticalTexture;
        } else if (roadStraightTexture.id != 0) {
            texture = &roadStraightTexture;
        }
    } else if (tileType == "straight") {
        if (roadStraightTexture.id != 0) {
            texture = &roadStraightTexture;
        }
    } else if (tileType == "roundabout") {
        if (roadRoundaboutTexture.id != 0) {
            texture = &roadRoundaboutTexture;
        } else if (roadIntersectionTexture.id != 0) {
            texture = &roadIntersectionTexture;
        }
    } else if (tileType == "intersection" || tileType == "crossroad") {
        if (roadIntersectionTexture.id != 0) {
            texture = &roadIntersectionTexture;
        } else if (roadRoundaboutTexture.id != 0) {
            texture = &roadRoundaboutTexture;
        }
    } else if (tileType == "curve") {
        if (roadCurveTexture.id != 0) {
            texture = &roadCurveTexture;
        }
    }
    
    if (texture && texture->id != 0) {
        // Taille plus grande pour que les routes soient bien visibles
        float tileSize = 100.0f; // Taille en unités du monde (alignée avec l'espacement du graphe)
        Rectangle source = {0, 0, (float)texture->width, (float)texture->height};
        Rectangle dest = {x, y, tileSize, tileSize};
        Vector2 origin = {tileSize / 2.0f, tileSize / 2.0f};
        // Utiliser un tint blanc pour voir les couleurs originales
        Color tint = WHITE;
        DrawTexturePro(*texture, source, dest, origin, rotation * 180.0f / PI, tint);
    } else {
        // Fallback : dessiner un rectangle simple si texture non disponible
        DrawRectangle((int)(x - 50), (int)(y - 50), 100, 100, {100, 100, 100, 255});
    }
}

