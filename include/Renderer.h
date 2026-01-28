#ifndef RENDERER_H
#define RENDERER_H

#include "Simulation.h"
#include "raylib.h"
#include <unordered_map>

// Classe de rendu utilisant Raylib
class Renderer {
private:
    int screenWidth;
    int screenHeight;
    Camera2D camera;
    
    // Couleurs
    Color routeNormalColor;
    Color routeCongestedColor;
    Color routeBlockedColor;
    Color vehicleColor;
    Color nodeColor;
    Color backgroundColor;
    
    // Échelle et offset pour l'affichage
    float scale;
    Vector2 offset;
    
    // Système de notifications
    struct Notification {
        std::string text;
        Color color;
        float time;
        float duration;
    };
    std::vector<Notification> notifications;
    
    // Textures (images)
    Texture2D vehicleCarTexture;
    Texture2D vehicleTruckTexture;
    Texture2D vehicleBusTexture;
    Texture2D eventAccidentTexture;
    Texture2D eventTrafficJamTexture;
    Texture2D eventRoadClosureTexture;
    Texture2D eventEmergencyTexture;
    bool texturesLoaded;
    
    // Cache de textures pour véhicules avec directions
    std::unordered_map<std::string, Texture2D> vehicleTextureCache;
    
    // Tiles de carte
    Texture2D tilemapTexture; // Tilemap complet (Roguelike Modern City)
    Texture2D roadStraightTexture;
    Texture2D roadHorizontalTexture; // Route horizontale
    Texture2D roadVerticalTexture; // Route verticale
    Texture2D roadIntersectionTexture;
    Texture2D roadCurveTexture; // Virage
    Texture2D roadRoundaboutTexture; // Rond-point
    Texture2D grassTexture;
    Texture2D pavementTexture;
    Texture2D buildingTexture;
    bool mapTilesLoaded;
    
    // Taille des tuiles
    int tileSize;
    
    // Police personnalisée
    Font gameFont;
    bool fontLoaded;
    
    // Musique de fond
    Music backgroundMusic;
    bool musicLoaded;
    
public:
    Renderer(int width = 1280, int height = 720);
    ~Renderer();
    
    // Initialisation et nettoyage
    void initialize();
    void cleanup();
    
    // Rendu
    void beginFrame();
    void endFrame();
    void renderSimulation(const Simulation& simulation);
    void renderGraph(const Graph& graph);
    void renderVehicles(const std::vector<std::unique_ptr<Vehicle>>& vehicles);
    void renderEvents(const std::vector<std::unique_ptr<Event>>& events, const Graph& graph);
    void renderUI(const Simulation& simulation);
    
    // Gestion des boutons
    void handleButtonClicks(Simulation& simulation);
    
    // Gestion de la musique
    void loadMusic();
    void updateMusic();
    void toggleMusic();
    bool isMusicPlaying() const;
    
    // Gestion de la caméra
    void updateCamera();
    void setCameraTarget(float x, float y);
    void fitGraphToWindow(const Graph& graph);
    
    // Utilitaires
    Vector2 worldToScreen(float x, float y) const;
    Vector2 screenToWorld(int x, int y) const;
    
    // Notifications
    void addNotification(const std::string& text, Color color, float duration = 3.0f);
    void updateNotifications(float deltaTime);
    void renderNotifications();
    
    // Gestion des textures
    void loadTextures();
    void unloadTextures();
    bool areTexturesLoaded() const { return texturesLoaded; }
    
    // Gestion des tiles de carte
    void loadMapTiles();
    void unloadMapTiles();
    void renderMapBackground(const Graph& graph);
    void renderRoadTile(float x, float y, const std::string& tileType, float rotation = 0.0f);
};

#endif // RENDERER_H

