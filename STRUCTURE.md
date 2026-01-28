# 📐 Structure et Architecture du Projet

## Vue d'ensemble

Ce document décrit en détail la structure interne du projet, les relations entre les composants, et le flux de données dans la simulation de routage dynamique.

---

## 🏗️ Architecture Générale

```
┌─────────────────────────────────────────────────────────────┐
│                      APPLICATION                             │
│                    (main.cpp)                                │
└───────────────────────┬─────────────────────────────────────┘
                        │
        ┌───────────────┼───────────────┐
        │               │               │
        ▼               ▼               ▼
┌──────────────┐ ┌──────────────┐ ┌──────────────┐
│  Simulation  │ │   Renderer   │ │   Factory    │
│              │ │              │ │              │
│ - Graph      │ │ - Raylib     │ │ - Events     │
│ - Vehicles   │ │ - UI         │ │ - Vehicles   │
│ - Events     │ │ - Camera     │ │              │
│ - Planner    │ │              │ │              │
└──────┬───────┘ └──────────────┘ └──────────────┘
       │
       ├──────────────┬──────────────┬──────────────┐
       │              │              │              │
       ▼              ▼              ▼              ▼
┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐
│  Graph   │  │ Vehicle  │  │  Event   │  │PathPlanner│
│          │  │          │  │          │  │           │
│ - Nodes  │  │ - Path   │  │ - Type   │  │ - Strategy│
│ - Routes │  │ - Speed  │  │ - State  │  │ - A*      │
└────┬─────┘  └──────────┘  └────┬─────┘  └─────┬─────┘
     │                            │              │
     │                            │              │
     ▼                            ▼              ▼
┌──────────┐              ┌──────────┐  ┌──────────────┐
│  Route   │              │ Route    │  │Pathfinding   │
│          │              │ State    │  │Strategy      │
│ - Length │              │          │  │              │
│ - Speed  │              │ - NORMAL │  │ - AStar      │
│ - State  │              │ - CONGEST│  │ - Dijkstra   │
└──────────┘              │ - BLOCKED│  └──────────────┘
                          │ - ACCIDENT│
                          └──────────┘
```

---

## 📦 Composants Principaux

### 1. **Simulation** (`Simulation.h/cpp`)
**Rôle** : Orchestrateur principal de la simulation

**Responsabilités** :
- Gestion du cycle de vie de la simulation
- Création et mise à jour du graphe routier
- Gestion des véhicules (création, mise à jour, reroutage)
- Gestion des événements (création, application, expiration)
- Coordination entre tous les composants

**Relations** :
- **Contient** : `Graph`, `PathPlanner`, `std::vector<Vehicle>`, `std::vector<Event>`
- **Utilise** : `Factory` pour créer des objets
- **Communique avec** : `Renderer` via les données du graphe

**Flux de données** :
```
Simulation::update()
  ├─> Graph::updateTraffic()
  ├─> Event::update() pour chaque événement
  ├─> Vehicle::update() pour chaque véhicule
  └─> PathPlanner::replanPath() si reroutage nécessaire
```

---

### 2. **Graph** (`Graph.h/cpp`)
**Rôle** : Représentation du réseau routier

**Structure** :
- **Nodes** : Intersections du réseau (struct `Node` avec id, x, y)
- **Routes** : Segments routiers entre nœuds (classe `Route`)
- **AdjacencyList** : Liste d'adjacence pour navigation rapide

**Responsabilités** :
- Ajout/suppression de nœuds et routes
- Recherche de voisins
- Mise à jour du trafic (comptage des véhicules)
- Chargement depuis JSON

**Relations** :
- **Contient** : `std::vector<std::unique_ptr<Node>>`, `std::vector<std::unique_ptr<Route>>`
- **Utilisé par** : `Simulation`, `PathPlanner`, `Renderer`, `Vehicle`

**Structure interne** :
```cpp
Graph
├── nodes: vector<unique_ptr<Node>>
│   └── Node { id, x, y }
├── routes: vector<unique_ptr<Route>>
│   └── Route { id, fromNode, toNode, length, speed, state }
└── adjacencyList: unordered_map<int, vector<int>>
    └── nodeId -> [routeIds]
```

---

### 3. **Route** (`Route.h/cpp`)
**Rôle** : Représente un segment routier

**Propriétés** :
- **État** : `NORMAL`, `CONGESTED`, `BLOCKED`, `ACCIDENT`
- **Trafic** : Nombre de véhicules, capacité, vitesse actuelle
- **Coût** : Temps de parcours calculé dynamiquement

**Responsabilités** :
- Calcul du temps de parcours (pour pathfinding)
- Gestion de l'état (normal, congestionné, bloqué)
- Mise à jour de la vitesse selon le trafic
- Vérification de l'utilisabilité

**Relations** :
- **Appartient à** : `Graph`
- **Utilisée par** : `PathPlanner` (calcul de coût), `Vehicle` (navigation)

**Méthodes clés** :
```cpp
float getTravelTime()      // Coût pour pathfinding
bool isUsable()            // Vérifie si la route est utilisable
void updateSpeed()         // Met à jour la vitesse selon le trafic
void setState(RouteState)  // Change l'état (NORMAL, CONGESTED, etc.)
```

---

### 4. **Vehicle** (`Vehicle.h/cpp`)
**Rôle** : Représente un véhicule dans la simulation

**Propriétés** :
- **Position** : Coordonnées (x, y) et angle
- **Chemin** : Liste de nœuds à suivre
- **Progression** : Position actuelle sur la route
- **État** : Besoin de reroutage, destination atteinte

**Responsabilités** :
- Suivre le chemin planifié
- Détecter les routes bloquées dans le chemin futur
- Demander reroutage si nécessaire
- Calculer sa position et orientation

**Relations** :
- **Appartient à** : `Simulation` (dans un vector)
- **Utilise** : `Graph` pour naviguer, `Route` pour se déplacer
- **Communique avec** : `PathPlanner` via `Simulation`

**Cycle de vie** :
```
Vehicle créé
  └─> PathPlanner::planPath() → chemin initial
      └─> Vehicle::setPath()
          └─> Vehicle::update() (chaque frame)
              ├─> Vérifie routes bloquées
              ├─> Met à jour progression
              └─> Demande reroutage si nécessaire
```

---

### 5. **PathPlanner** (`PathPlanner.h/cpp`)
**Rôle** : Planifie les trajets des véhicules

**Responsabilités** :
- Planification de chemins initiaux (A* ou Dijkstra)
- Reroutage dynamique lors d'événements
- Utilisation du pattern Strategy pour algorithmes

**Relations** :
- **Contient** : `PathfindingStrategy` (A* ou Dijkstra)
- **Utilise** : `Graph` pour la navigation
- **Utilisé par** : `Simulation` pour planifier les trajets

**Pattern Strategy** :
```cpp
PathPlanner
└── strategy: unique_ptr<PathfindingStrategy>
    ├── AStarStrategy
    │   └── findPath() avec heuristique
    └── DijkstraStrategy
        └── findPath() sans heuristique
```

---

### 6. **PathfindingStrategy** (`PathfindingStrategy.h/cpp`)
**Rôle** : Interface pour algorithmes de pathfinding

**Implémentations** :
- **AStarStrategy** : Algorithme A* avec heuristique
- **DijkstraStrategy** : Algorithme de Dijkstra

**Pattern** : Strategy Pattern pour interchangeabilité

**Relations** :
- **Héritage** : `AStarStrategy` et `DijkstraStrategy` héritent de `PathfindingStrategy`
- **Utilisé par** : `PathPlanner`

---

### 7. **Event** (`Event.h/cpp`)
**Rôle** : Représente un événement (accident, embouteillage, etc.)

**Types d'événements** :
- `ACCIDENT` : Route bloquée
- `TRAFFIC_JAM` : Route congestionnée
- `ROAD_CLOSURE` : Route fermée
- `EMERGENCY` : Urgence (route prioritaire)

**Responsabilités** :
- Application d'effets sur les routes
- Gestion de la durée (événements temporaires)
- Mise à jour de l'état des routes

**Relations** :
- **Appartient à** : `Simulation` (dans un vector)
- **Affecte** : `Route` (change l'état)
- **Déclenche** : Reroutage des véhicules via `Simulation`

**Cycle de vie** :
```
Event créé
  └─> Event::applyToRoute() → Route::setState()
      └─> Simulation::rerouteAffectedVehicles()
          └─> PathPlanner::replanPath() pour chaque véhicule affecté
              └─> Event::update() (chaque frame)
                  └─> Event::isFinished() → suppression
```

---

### 8. **Renderer** (`Renderer.h/cpp`)
**Rôle** : Gestion de l'affichage avec Raylib

**Responsabilités** :
- Rendu du graphe (routes, nœuds, intersections)
- Rendu des véhicules (avec rotation)
- Rendu des événements (accidents, embouteillages)
- Interface utilisateur (statistiques, contrôles)
- Gestion de la caméra (déplacement, zoom)
- Chargement des textures et assets

**Relations** :
- **Utilise** : `Graph`, `Vehicle`, `Event` (lecture seule)
- **Communique avec** : `Simulation` pour obtenir les données

**Composants visuels** :
```
Renderer
├── Routes (lignes avec bordures)
├── Intersections (cercles avec feux de circulation)
├── Rond-points (cercles avec îlot central)
├── Véhicules (sprites avec rotation)
├── Événements (overlays colorés)
└── UI (statistiques, contrôles)
```

---

### 9. **Factory** (`Factory.h/cpp`)
**Rôle** : Création d'objets (Pattern Factory)

**Classes** :
- `EventFactory` : Crée des événements
- `VehicleFactory` : Crée des véhicules

**Relations** :
- **Utilisé par** : `Simulation` pour créer des objets

---

## 🔄 Flux de Données Principal

### Initialisation
```
main.cpp
  └─> Simulation::initialize()
      ├─> Graph::createTestGraph() ou Graph::loadFromConfig()
      └─> Simulation::createVehicles()
          └─> PathPlanner::planPath() pour chaque véhicule
```

### Boucle de Simulation
```
Chaque frame (60 FPS) :
  ├─> Simulation::update(deltaTime)
  │   ├─> Event::update() pour chaque événement
  │   │   └─> Route::setState() si événement actif
  │   ├─> Vehicle::update() pour chaque véhicule
  │   │   ├─> Vérifie routes bloquées
  │   │   ├─> Met à jour progression
  │   │   └─> Demande reroutage si nécessaire
  │   ├─> PathPlanner::replanPath() si reroutage demandé
  │   └─> Graph::updateTraffic()
  │
  └─> Renderer::render()
      ├─> Renderer::renderGraph()
      ├─> Renderer::renderVehicles()
      └─> Renderer::renderUI()
```

### Reroutage Dynamique
```
Événement créé
  └─> Event::applyToRoute() → Route::setState(BLOCKED/ACCIDENT)
      └─> Simulation::rerouteAffectedVehicles()
          └─> Pour chaque véhicule affecté :
              ├─> Vérifie si le véhicule utilise la route bloquée
              └─> Si oui :
                  └─> PathPlanner::replanPath()
                      └─> Vehicle::setPath() → nouveau chemin
```

---

## 🗂️ Organisation des Fichiers

### Structure des Répertoires

```
Sous_Projet_3_Routage_Dynamique_Cpp/
│
├── 📂 include/              # Déclarations (headers)
│   ├── Event.h              # Gestion des événements
│   ├── Factory.h            # Pattern Factory
│   ├── Graph.h              # Réseau routier
│   ├── PathfindingStrategy.h # Pattern Strategy
│   ├── PathPlanner.h        # Planification de trajets
│   ├── Route.h              # Segment routier
│   ├── Simulation.h         # Orchestrateur principal
│   ├── Vehicle.h            # Véhicule
│   └── Renderer.h           # Rendu graphique
│
├── 📂 src/                  # Implémentations
│   ├── Event.cpp
│   ├── Factory.cpp
│   ├── Graph.cpp
│   ├── PathfindingStrategy.cpp
│   ├── PathPlanner.cpp
│   ├── Route.cpp
│   ├── Simulation.cpp
│   ├── Vehicle.cpp
│   └── Renderer.cpp
│
├── 📂 tests/                # Tests unitaires
│   ├── test_Event.cpp
│   ├── test_Graph.cpp
│   ├── test_PathPlanner.cpp
│   ├── test_Route.cpp
│   └── test_Vehicle.cpp
│
├── 📂 demos/                # Application principale
│   └── main.cpp             # Point d'entrée
│
├── 📂 config/               # Configuration
│   └── config.json          # Paramètres JSON
│
├── 📂 assets/               # Ressources
│   ├── vehicles/            # Images de véhicules
│   ├── events/              # Images d'événements
│   ├── map/                 # Tiles de carte
│   └── music/               # Musique de fond
│
├── 📂 external/             # Dépendances externes
│   └── raylib/              # Bibliothèque Raylib
│
└── 📂 build/                # Fichiers compilés (généré)
```

---

## 🔗 Relations entre Classes

### Diagramme de Dépendances

```
Simulation
  ├─> Graph (composition)
  ├─> PathPlanner (composition)
  ├─> Vehicle[] (agrégation)
  └─> Event[] (agrégation)

Graph
  ├─> Node[] (composition)
  └─> Route[] (composition)

PathPlanner
  └─> PathfindingStrategy (composition)
      ├─> AStarStrategy (héritage)
      └─> DijkstraStrategy (héritage)

Vehicle
  └─> utilise Graph (référence)

Event
  └─> affecte Route (référence)

Renderer
  ├─> utilise Graph (référence)
  ├─> utilise Vehicle[] (référence)
  └─> utilise Event[] (référence)
```

---

## 🎯 Points Clés de l'Architecture

### 1. **Séparation des Responsabilités**
- Chaque classe a un rôle bien défini
- Pas de couplage fort entre composants
- Communication via interfaces claires

### 2. **Gestion Mémoire**
- Utilisation de `std::unique_ptr` pour propriété exclusive
- RAII pour libération automatique
- Pas de fuites mémoire

### 3. **Extensibilité**
- Pattern Strategy pour nouveaux algorithmes
- Pattern Factory pour nouveaux types d'objets
- Architecture modulaire

### 4. **Performance**
- Liste d'adjacence pour navigation rapide
- Calculs optimisés pour pathfinding
- Rendu efficace avec Raylib

### 5. **Maintenabilité**
- Code bien organisé et documenté
- Tests unitaires pour validation
- Configuration externe (JSON)

---

## 📊 États et Transitions

### État d'une Route
```
NORMAL
  └─> [TRAFFIC_JAM] → CONGESTED
  └─> [ACCIDENT] → ACCIDENT
  └─> [ROAD_CLOSURE] → BLOCKED

CONGESTED
  └─> [Trafic réduit] → NORMAL
  └─> [ACCIDENT] → ACCIDENT

ACCIDENT
  └─> [Event expiré] → NORMAL

BLOCKED
  └─> [Event expiré] → NORMAL
```

### État d'un Véhicule
```
En chemin
  └─> [Route bloquée détectée] → Demande reroutage
  └─> [Destination atteinte] → Arrivé

Demande reroutage
  └─> [Nouveau chemin trouvé] → En chemin
  └─> [Pas de chemin] → Attente
```

---

## 🔍 Détails Techniques

### Pathfinding
- **Algorithme par défaut** : A* avec heuristique de distance euclidienne
- **Coût** : Temps de parcours (`Route::getTravelTime()`)
- **Reroutage** : Recalcul complet du chemin depuis la position actuelle

### Gestion du Trafic
- **Comptage** : Chaque route compte les véhicules qui l'utilisent
- **Vitesse** : Réduction selon le ratio de congestion
- **Pénalité** : Routes congestionnées ont un coût plus élevé

### Rond-points
- **Structure** : Nœud central + 4 nœuds autour (N, S, E, W)
- **Détection** : Nœuds avec > 4 connexions
- **Comportement** : Pas de feux, circulation continue

---

## 📝 Notes d'Implémentation

### Points Importants
1. **Thread Safety** : Non thread-safe (simulation mono-thread)
2. **Performance** : Optimisé pour ~50 véhicules simultanés
3. **Précision** : Utilisation de `float` pour positions (suffisant pour visualisation)
4. **Temps** : Simulation en temps réel avec `deltaTime`

### Limitations Connues
- Pas de gestion de collisions entre véhicules
- Pas de priorité aux intersections (sauf rond-points)
- Pathfinding simple (pas de considération multi-agents)

---

## 🚀 Extensions Possibles

### Améliorations Futures
1. **Gestion de collisions** : Détection et résolution
2. **Feux de circulation intelligents** : Synchronisation
3. **Multi-threading** : Parallélisation de la simulation
4. **IA avancée** : Comportement plus réaliste des véhicules
5. **Réseaux plus complexes** : Autoroutes, échangeurs

---

*Document généré pour le projet de Routage Dynamique - C++ OOP*

