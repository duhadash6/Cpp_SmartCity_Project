#!/bin/bash
# Script de compilation pour Mac et Linux

echo "========================================"
echo "Compilation du jeu (Mac/Linux)"
echo "========================================"
echo ""

# Vérifier que CMake est installé
if ! command -v cmake &> /dev/null; then
    echo "ERREUR: CMake n'est pas installe!"
    echo "Installez CMake avec:"
    echo "  - Mac: brew install cmake"
    echo "  - Ubuntu/Debian: sudo apt-get install cmake"
    echo "  - Fedora: sudo dnf install cmake"
    exit 1
fi

# Vérifier que make est installé
if ! command -v make &> /dev/null; then
    echo "ERREUR: make n'est pas installe!"
    echo "Installez make avec votre gestionnaire de paquets."
    exit 1
fi

# Créer le dossier build s'il n'existe pas
mkdir -p build
cd build

# Nettoyer le cache CMake si nécessaire
if [ -f CMakeCache.txt ]; then
    echo "Nettoyage du cache CMake..."
    rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake Makefile CTestTestfile.cmake
fi

# Configurer CMake
echo ""
echo "Configuration de CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo ""
    echo "========================================"
    echo "Erreur de configuration CMake."
    echo "========================================"
    exit 1
fi

# Compiler
echo ""
echo "Compilation..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "========================================"
if [ $? -eq 0 ]; then
    echo "Compilation reussie !"
    echo "Vous pouvez maintenant lancer le jeu avec:"
    echo "  ./build/RoutageDynamique"
    echo ""
    echo "Ou executer les tests avec:"
    echo "  cd build && ctest"
else
    echo "Erreur de compilation."
fi
echo "========================================"

