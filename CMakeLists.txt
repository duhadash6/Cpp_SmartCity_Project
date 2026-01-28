cmake_minimum_required(VERSION 3.15)
project(RoutageDynamique)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options de compilation
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Recherche de Raylib via vcpkg ou système
# Si vcpkg est utilisé, il configurera automatiquement les chemins
find_package(raylib QUIET)

if(raylib_FOUND)
    message(STATUS "Raylib trouve via find_package")
    # Raylib est trouvé via vcpkg ou installation système
else()
    # Recherche manuelle - d'abord dans external/ (pour version précompilée)
    find_library(RAYLIB_LIB 
        NAMES raylib libraylib libraylibdll
        PATHS
            "${CMAKE_SOURCE_DIR}/external/raylib/lib"
            "${CMAKE_SOURCE_DIR}/external/raylib/lib/Release"
            "${CMAKE_SOURCE_DIR}/external/raylib/lib/Debug"
            "${CMAKE_SOURCE_DIR}/vcpkg/installed/x64-windows/lib"
            "${CMAKE_SOURCE_DIR}/vcpkg/installed/x64-mingw-dynamic/lib"
            /usr/lib
            /usr/local/lib
            /mingw64/lib
    )
    
    find_path(RAYLIB_INCLUDE 
        NAMES raylib.h
        PATHS
            "${CMAKE_SOURCE_DIR}/external/raylib/include"
            "${CMAKE_SOURCE_DIR}/external/raylib/src"
            "${CMAKE_SOURCE_DIR}/vcpkg/installed/x64-windows/include"
            "${CMAKE_SOURCE_DIR}/vcpkg/installed/x64-mingw-dynamic/include"
            /usr/include
            /usr/local/include
            /mingw64/include
    )
    
    if(NOT RAYLIB_LIB OR NOT RAYLIB_INCLUDE)
        message(FATAL_ERROR 
            "Raylib non trouve!\n"
            "Options:\n"
            "  1. Installer avec vcpkg: cd vcpkg && .\\vcpkg install raylib:x64-windows\n"
            "  2. Télécharger version précompilée: https://github.com/raysan5/raylib/releases\n"
            "     Extraire dans external/raylib/\n"
        )
    else()
        message(STATUS "Raylib trouve: ${RAYLIB_LIB}")
        message(STATUS "Raylib include: ${RAYLIB_INCLUDE}")
    endif()
endif()

# Fichiers sources
set(SOURCES
    src/Route.cpp
    src/Graph.cpp
    src/Vehicle.cpp
    src/PathPlanner.cpp
    src/PathfindingStrategy.cpp
    src/Event.cpp
    src/Simulation.cpp
    src/Renderer.cpp
    src/Factory.cpp
)

# Fichiers d'en-tête
set(HEADERS
    include/Route.h
    include/Graph.h
    include/Vehicle.h
    include/PathPlanner.h
    include/PathfindingStrategy.h
    include/Event.h
    include/Simulation.h
    include/Renderer.h
    include/Factory.h
)

# Exécutable principal
add_executable(${PROJECT_NAME} 
    ${SOURCES}
    ${HEADERS}
    demos/main.cpp
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${RAYLIB_INCLUDE}
)

# Link libraries
if(raylib_FOUND)
    target_link_libraries(${PROJECT_NAME} raylib)
else()
    target_link_libraries(${PROJECT_NAME} ${RAYLIB_LIB})
endif()

# Configuration spécifique par plateforme
if(WIN32)
    # Bibliothèques Windows nécessaires pour Raylib
    target_link_libraries(${PROJECT_NAME} 
        winmm
        gdi32
        user32
        shell32
    )
    
    # Utiliser la version statique si disponible (pas besoin de DLL)
    # Sinon, copier la DLL seulement si on utilise la version DLL
    if(EXISTS "${CMAKE_SOURCE_DIR}/external/raylib/lib/libraylib.a")
        message(STATUS "Utilisation de la version statique de Raylib (pas de DLL necessaire)")
    elseif(EXISTS "${CMAKE_SOURCE_DIR}/external/raylib/lib/raylib.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/external/raylib/lib/raylib.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMENT "Copie de raylib.dll")
    endif()
elseif(APPLE)
    # Bibliothèques Mac nécessaires pour Raylib
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(OPENGL_FRAMEWORK OpenGL)
    find_library(QUARTZCORE_FRAMEWORK QuartzCore)
    
    if(COCOA_FRAMEWORK)
        target_link_libraries(${PROJECT_NAME} ${COCOA_FRAMEWORK})
    endif()
    if(IOKIT_FRAMEWORK)
        target_link_libraries(${PROJECT_NAME} ${IOKIT_FRAMEWORK})
    endif()
    if(OPENGL_FRAMEWORK)
        target_link_libraries(${PROJECT_NAME} ${OPENGL_FRAMEWORK})
    endif()
    if(QUARTZCORE_FRAMEWORK)
        target_link_libraries(${PROJECT_NAME} ${QUARTZCORE_FRAMEWORK})
    endif()
    
    # Chercher aussi dans les chemins Mac standards
    find_library(RAYLIB_MAC_LIB raylib
        PATHS
            "${CMAKE_SOURCE_DIR}/external/raylib/lib"
            /usr/local/lib
            /opt/homebrew/lib
    )
    
    if(RAYLIB_MAC_LIB)
        target_link_libraries(${PROJECT_NAME} ${RAYLIB_MAC_LIB})
        message(STATUS "Raylib Mac trouve: ${RAYLIB_MAC_LIB}")
    endif()
elseif(UNIX)
    # Bibliothèques Linux nécessaires pour Raylib
    target_link_libraries(${PROJECT_NAME}
        pthread
        dl
        m
    )
    
    # Chercher dans les chemins Linux standards
    find_library(RAYLIB_LINUX_LIB raylib
        PATHS
            "${CMAKE_SOURCE_DIR}/external/raylib/lib"
            /usr/lib
            /usr/local/lib
    )
    
    if(RAYLIB_LINUX_LIB)
        target_link_libraries(${PROJECT_NAME} ${RAYLIB_LINUX_LIB})
        message(STATUS "Raylib Linux trouve: ${RAYLIB_LINUX_LIB}")
    endif()
endif()

# Copier les assets dans le dossier build (toutes les plateformes)
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        COMMENT "Copie des assets (vehicules, map, routes, events)")
    message(STATUS "Assets seront copies dans build/assets/ (vehicules, map, routes, events)")
else()
    message(STATUS "ATTENTION: Dossier assets/ non trouve")
endif()

# Tests unitaires
enable_testing()

# Compilation des tests unitaires
add_executable(test_Event tests/test_Event.cpp src/Event.cpp src/Route.cpp)
target_include_directories(test_Event PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(test_Graph tests/test_Graph.cpp src/Graph.cpp src/Route.cpp)
target_include_directories(test_Graph PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(test_PathPlanner tests/test_PathPlanner.cpp 
    src/PathPlanner.cpp src/PathfindingStrategy.cpp src/Graph.cpp src/Route.cpp)
target_include_directories(test_PathPlanner PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(test_Route tests/test_Route.cpp src/Route.cpp)
target_include_directories(test_Route PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(test_Vehicle tests/test_Vehicle.cpp src/Vehicle.cpp src/Graph.cpp src/Route.cpp)
target_include_directories(test_Vehicle PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Ajout des tests à CTest
add_test(NAME EventTest COMMAND test_Event)
add_test(NAME GraphTest COMMAND test_Graph)
add_test(NAME PathPlannerTest COMMAND test_PathPlanner)
add_test(NAME RouteTest COMMAND test_Route)
add_test(NAME VehicleTest COMMAND test_Vehicle)

