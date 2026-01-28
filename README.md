# Simulation de Routage Dynamique

**Projet C++ - Simulation de véhicules avec reroutage dynamique en temps réel**

Ce projet implémente une simulation de routage dynamique où des véhicules se déplacent dans un réseau routier et sont automatiquement reroutés lors d'événements (accidents, embouteillages, etc.).

---.\recompiler.bat  .\lancer_jeu.bat

## Démonstation


https://github.com/user-attachments/assets/c5563178-f866-411f-80c3-b8d5bfa203c8

## Table des Matières

1. [Description](#description-du-projet)
2. [Fonctionnalités](#fonctionnalités)
3. [Installation](#installation-et-compilation)
4. [Utilisation](#utilisation)
5. [Architecture](#architecture-du-projet)
6. [Design Patterns](#design-patterns)
7. [Tests](#tests-unitaires)
8. [Technologies](#technologies-utilisées)

---

## Description du Projet

Ce projet implémente une simulation de routage dynamique en C++. Le système simule des véhicules qui se déplacent dans un réseau routier et qui peuvent être reroutés automatiquement lorsque des événements surviennent (accidents, embouteillages, etc.).

### Objectifs du projet

- Implémenter un algorithme de pathfinding (A*) pour planifier les trajets
- Gérer le reroutage dynamique lors d'événements
- Visualiser la simulation avec Raylib
- Comparer deux modes : Normal (sans reroutage) et Dynamique (avec reroutage)

Le projet utilise la Programmation Orientée Objet et implémente deux Design Patterns : Factory et Strategy.

---

## Fonctionnalités

### Algorithmes de Pathfinding
- Algorithme A* pour la planification de trajets
- Algorithme Dijkstra (implémenté via le pattern Strategy)
- Prise en compte du trafic dans le calcul des chemins
- Reroutage automatique lors d'événements

### Gestion du Trafic
- Simulation de multiples véhicules simultanément
- Calcul dynamique de la vitesse selon la congestion
- Événements aléatoires : accidents, embouteillages, fermetures de route, urgences
- Deux modes : Normal (sans reroutage) et Dynamique (avec reroutage)

### Visualisation
- Interface graphique 2D avec Raylib
- Animation des véhicules avec rotation
- Affichage des événements sur le réseau
- Caméra interactive (déplacement et zoom)
- Interface utilisateur avec statistiques

### Configuration
- Système de configuration JSON
- Paramètres ajustables (nombre de véhicules, fréquence d'événements)
- Réseaux routiers personnalisables

### Qualité du Code
- 5 tests unitaires couvrant les classes principales
- Documentation avec commentaires Doxygen
- 2 Design Patterns implémentés (Factory, Strategy)
- Architecture modulaire et organisée

---

## 🚀 Installation et Compilation

### 📋 Prérequis

#### Windows
- **CMake** (version 3.15+) : [Télécharger](https://cmake.org/download/)
- **MinGW-w64** (GCC) : [Télécharger](https://www.mingw-w64.org/downloads/)

#### Mac
- **Xcode Command Line Tools** :
  ```bash
  xcode-select --install
  ```
- **Homebrew** (si pas installé) :
  ```bash
  /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
  ```
- **CMake** :
  ```bash
  brew install cmake
  ```

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install build-essential cmake
```

### 🔨 Compilation

#### Windows
1. Double-cliquez sur `recompiler.bat`
   - Ou dans PowerShell : `.\recompiler.bat`
2. Le jeu sera compilé dans `build/RoutageDynamique.exe`

#### Mac / Linux
1. Ouvrez un terminal dans le dossier du projet
2. Rendez le script exécutable (première fois) :
   ```bash
   chmod +x build.sh
   ```
3. Compilez :
   ```bash
   ./build.sh
   ```
4. Lancez le jeu :
   ```bash
   ./build/RoutageDynamique
   ```

### ✅ Vérification

Le projet est **autonome** et contient toutes les dépendances :
- ✅ Raylib inclus dans `external/raylib/`
- ✅ Assets inclus dans `assets/`
- ✅ Configuration incluse dans `config/`

**Pas besoin de télécharger quoi que ce soit d'autre !**

---

## 🎮 Utilisation

### Lancement du Jeu

**Windows :**
```powershell
.\build\RoutageDynamique.exe
```

**Mac / Linux :**
```bash
./build/RoutageDynamique
```

### Contrôles

| Touche | Action |
|--------|--------|
| **SPACE** | Déclencher un événement aléatoire |
| **R** | Basculer entre mode Normal et Dynamique |
| **+ / -** | Augmenter/Reduire le nombre de véhicules |
| **Flèches / WASD** | Déplacer la caméra |
| **Molette** | Zoom avant/arrière |
| **ESC** | Quitter le jeu |

### Modes de Simulation

- **Mode Normal** : Les véhicules suivent leur chemin initial, même en cas d'événement
- **Mode Dynamique** : Les véhicules sont automatiquement reroutés lors d'événements

### Configuration

Modifiez `config/config.json` pour personnaliser :
- Nombre de véhicules
- Fréquence des événements
- Structure du réseau routier
- Paramètres de simulation

---

## 📁 Architecture du Projet

> 📖 **Pour une explication détaillée de l'architecture interne, consultez [STRUCTURE.md](STRUCTURE.md)**

Le projet suit une **architecture claire et modulaire** :

```
Sous_Projet_3_Routage_Dynamique_Cpp/
│
├── 📂 include/              # Fichiers d'en-tête (.h)
│   ├── Event.h              # Gestion des événements
│   ├── Factory.h            # Pattern Factory
│   ├── Graph.h              # Représentation du réseau routier
│   ├── PathPlanner.h        # Planificateur de trajets
│   ├── PathfindingStrategy.h # Pattern Strategy
│   ├── Route.h              # Représentation d'une route
│   ├── Simulation.h         # Classe principale
│   ├── Vehicle.h            # Représentation d'un véhicule
│   └── Renderer.h           # Rendu avec Raylib
│
├── 📂 src/                  # Code source (.cpp)
│   ├── Event.cpp
│   ├── Factory.cpp
│   ├── Graph.cpp
│   ├── PathPlanner.cpp
│   ├── PathfindingStrategy.cpp
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
├── 📂 demos/                 # Démo interactive
│   └── main.cpp
│
├── 📂 config/                # Configuration
│   └── config.json
│
├── 📂 assets/               # Ressources
│   ├── vehicles/            # Images de véhicules
│   ├── events/              # Images d'événements
│   ├── map/                 # Tiles de carte
│   └── music/               # Musique de fond
│
├── 📂 external/             # Dépendances
│   └── raylib/              # Bibliothèque Raylib
│       ├── include/
│       └── lib/
│
├── 📂 build/                # Fichiers de compilation
│
├── 📄 CMakeLists.txt        # Configuration CMake
├── 📄 recompiler.bat        # Script Windows
├── 📄 build.sh              # Script Mac/Linux
└── 📄 README.md             # Ce fichier
```

### Organisation du Code

- **Séparation des responsabilités** : Chaque classe a un rôle bien défini
- **Encapsulation** : Données privées avec accesseurs publics
- **Gestion mémoire** : Utilisation de `std::unique_ptr` (RAII)
- **Interfaces claires** : Méthodes publiques bien documentées

---

##  Design Patterns Implémentés

### 1. Factory Pattern

**Objectif** : Centraliser la création d'objets `Event` et `Vehicle`

**Implémentation** :
- `EventFactory` : Crée des événements aléatoires ou personnalisés
- `VehicleFactory` : Crée des véhicules avec différents types

**Avantages** :
- Création d'objets simplifiée
- Facilite l'ajout de nouveaux types
- Encapsule la logique de création

**Fichiers** : `include/Factory.h`, `src/Factory.cpp`

**Exemple d'utilisation** :
```cpp
EventFactory eventFactory;
auto event = eventFactory.createRandomEvent(routeId);

VehicleFactory vehicleFactory;
auto vehicle = vehicleFactory.createVehicle(startNode, targetNode);
```

### 2. Strategy Pattern

**Objectif** : Permettre l'utilisation de différents algorithmes de pathfinding

**Implémentation** :
- `PathfindingStrategy` : Interface commune
- `AStarStrategy` : Implémentation de l'algorithme A*
- `DijkstraStrategy` : Implémentation de l'algorithme Dijkstra

**Avantages** :
- Changement d'algorithme à l'exécution
- Extensibilité (ajout facile de nouveaux algorithmes)
- Séparation de la logique de pathfinding

**Fichiers** : `include/PathfindingStrategy.h`, `src/PathfindingStrategy.cpp`

**Exemple d'utilisation** :
```cpp
// Utilisation par défaut (A*)
PathPlanner planner(&graph);

// Changement de stratégie
planner.setStrategy(std::make_unique<DijkstraStrategy>());
```

---

## 🧪 Tests Unitaires

Le projet contient **5 tests unitaires** couvrant les classes principales :

| Test | Classe Testée | Fonctionnalités Vérifiées |
|------|---------------|---------------------------|
| `test_Event.cpp` | `Event` | Création, mise à jour, application aux routes |
| `test_Graph.cpp` | `Graph` | Création de graphe, recherche de chemins |
| `test_PathPlanner.cpp` | `PathPlanner` | Planification avec et sans trafic |
| `test_Route.cpp` | `Route` | Création, gestion du trafic, états |
| `test_Vehicle.cpp` | `Vehicle` | Création, chemin, mise à jour |

### Exécution des Tests

**Windows :**
```powershell
cd build
ctest
```

**Mac / Linux :**
```bash
cd build
ctest
```

**Ou individuellement :**
```bash
./build/test_Event
./build/test_Graph
./build/test_PathPlanner
./build/test_Route
./build/test_Vehicle
```

---

## 🔧 Technologies Utilisées

### Langages et Standards
- **C++17** : Langage de programmation moderne
- **CMake 3.15+** : Système de build multiplateforme

### Bibliothèques
- **Raylib 5.5** : Bibliothèque de visualisation 2D
  - Rendu graphique
  - Gestion des entrées
  - Audio

### Formats
- **JSON** : Configuration du projet

### Concepts
- **Programmation Orientée Objet** : Encapsulation, abstraction, héritage, polymorphisme
- **Design Patterns** : Factory, Strategy
- **RAII** : Gestion automatique de la mémoire
- **Smart Pointers** : `std::unique_ptr` pour la sécurité mémoire

---

## 📊 Principes POO Appliqués

### Encapsulation
- Données privées avec accesseurs publics
- Protection de l'intégrité des données
- Interface claire pour chaque classe

### Abstraction
- Interfaces bien définies
- Masquage de la complexité interne
- Utilisation simple des classes

### Héritage
- Utilisé dans le pattern Strategy
- `AStarStrategy` et `DijkstraStrategy` héritent de `PathfindingStrategy`

### Polymorphisme
- Via les stratégies de pathfinding
- Méthodes virtuelles pour le comportement dynamique

### Gestion Mémoire
- `std::unique_ptr` pour la propriété exclusive
- RAII pour la libération automatique
- Pas de fuites mémoire

---

## 🎨 Visualisation avec Raylib

Le projet utilise **Raylib** pour une visualisation 2D moderne :

### Fonctionnalités Visuelles
- **Rendu des routes** : Affichage du réseau routier
- **Animation des véhicules** : Déplacement fluide avec rotation
- **Événements visuels** : Affichage des accidents et embouteillages
- **Interface utilisateur** : Statistiques et contrôles
- **Caméra** : Déplacement et zoom interactifs
- **Musique de fond** : Ambiance sonore

### Avantages de Raylib
- Simple d'utilisation
- Performant
- Multiplateforme (Windows, Mac, Linux)
- Pas de dépendances externes complexes

---

## 📤 Partage du Projet

### Préparation

Le projet est **autonome** et contient toutes les dépendances :
- ✅ Raylib inclus
- ✅ Assets inclus
- ✅ Configuration incluse

### Partage

1. **Zipper le dossier** (sans `build/` pour réduire la taille)
2. **Envoyer le ZIP** à vos amis
3. **Instructions pour le destinataire** :
   - Extraire le ZIP
   - Suivre les instructions d'installation ci-dessus
   - Compiler avec `recompiler.bat` (Windows) ou `./build.sh` (Mac)
   - Jouer !

### Fichiers à Inclure

**Inclure :**
- ✅ Tous les dossiers (`include/`, `src/`, `tests/`, `demos/`, `assets/`, `config/`, `external/`)
- ✅ Fichiers de configuration (`CMakeLists.txt`, `recompiler.bat`, `build.sh`)
- ✅ Documentation (`README.md`)

**Exclure (optionnel) :**
- ❌ `build/` (sera recréé lors de la compilation)
- ❌ `.git/` (si vous utilisez Git)

---

## 📚 Documentation

### Code
- **Commentaires Doxygen** : Documentation complète des classes et méthodes
- **Noms explicites** : Code auto-documenté
- **Structure claire** : Organisation logique

### Guides
- **README.md** : Ce fichier (vue d'ensemble)
- **Commentaires dans le code** : Explications des algorithmes complexes

---

## 🐛 Dépannage

### Erreur "CMake not found"
- **Windows** : Installez CMake et ajoutez-le au PATH
- **Mac** : `brew install cmake`
- **Linux** : `sudo apt-get install cmake`

### Erreur "Raylib not found"
- Vérifiez que `external/raylib/include/raylib.h` existe
- Sur Mac, essayez : `brew install raylib`

### Erreur de compilation
- Vérifiez que vous avez un compilateur C++17 (GCC 7+ ou Clang 5+)
- Nettoyez le cache : supprimez `build/` et recommencez

### Le jeu ne trouve pas les assets
- Vérifiez que `build/assets/` existe
- Lancez le jeu depuis le dossier racine

---

## 📝 Exemple de Configuration

Fichier `config/config.json` :

```json
{
  "simulation": {
    "vehicleCount": 15,
    "eventCount": 1,
    "timeScale": 1.0,
    "mode": "DYNAMIC"
  },
  "graph": {
    "nodes": [
      {"id": 0, "x": 0, "y": 0},
      {"id": 1, "x": 100, "y": 0}
    ],
    "routes": [
      {"id": 0, "from": 0, "to": 1, "length": 100, "speed": 60, "capacity": 20}
    ]
  },
  "events": {
    "interval": 30.0,
    "types": ["ACCIDENT", "TRAFFIC_JAM", "ROAD_CLOSURE", "EMERGENCY"]
  }
}
```

---

## Contexte Académique

Ce projet a été réalisé dans le cadre d'un cours de programmation C++.

### Consignes Respectées

- ✅ Utilisation de C++ et principes de la POO
- ✅ Architecture claire (include/, src/, tests/, demos/)
- ✅ Intégration de Raylib pour la visualisation
- ✅ Documentation du code (commentaires + README)
- ✅ Système de configuration JSON
- ✅ 5 tests unitaires minimum
- ✅ 2 Design Patterns implémentés (Factory, Strategy)
- ✅ Démo interactive avec Raylib
