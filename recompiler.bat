@echo off
echo ========================================
echo Recompilation du jeu
echo ========================================
echo.

echo Fermeture du jeu en cours...
taskkill /F /IM RoutageDynamique.exe 2>nul
timeout /t 2 /nobreak >nul

echo.
echo Nettoyage du cache CMake...
if not exist "%~dp0build" mkdir "%~dp0build"
cd /d "%~dp0build"
if exist CMakeCache.txt del /Q CMakeCache.txt
if exist CMakeFiles rmdir /S /Q CMakeFiles
if exist cmake_install.cmake del /Q cmake_install.cmake
if exist Makefile del /Q Makefile
if exist CTestTestfile.cmake del /Q CTestTestfile.cmake

echo.
echo Configuration de CMake...
cmake .. -G "MinGW Makefiles"

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ========================================
    echo Erreur de configuration CMake.
    echo ========================================
    pause
    exit /b 1
)

echo.
echo Compilation...
mingw32-make

echo.
echo ========================================
if %ERRORLEVEL% EQU 0 (
    echo Compilation reussie !
    echo Vous pouvez maintenant lancer le jeu.
) else (
    echo Erreur de compilation.
)
echo ========================================
pause

