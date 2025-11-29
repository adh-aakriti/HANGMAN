cmake_minimum_required(VERSION 3.10)
project(HangmanRace)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -Wextra -pthread")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# -----------------------------------------------------------
# BLOQUE DE CONFIGURACIÓN SDL2 UNIVERSAL
# -----------------------------------------------------------

# 1. Búsqueda Estándar de SDL2 (funciona en la mayoría de los casos)
find_package(SDL2 REQUIRED)

# 2. Lógica Condicional para macOS (Homebrew)
# Esto solo se ejecuta si el sistema operativo detectado es macOS.
if(APPLE)
    # Incluir las rutas de Homebrew para encontrar los headers y librerías de SDL2
    include_directories(/opt/homebrew/include/SDL2)
    link_directories(/opt/homebrew/lib)
endif()

# 3. Solución de Respaldo para Entornos Problemáticos (tu WSL/Linux)
# Esta línea fuerza la inclusión de la ruta estándar de Linux,
# asegurando que siempre funcione en tu WSL.
include_directories(${SDL2_CFLAGS})
include_directories(/usr/include/SDL2)

set(SDL2_LIBRARIES SDL2)
set(SDL2_TTF_LIBRARY SDL2_ttf)

include_directories(
    ${PROJECT_SOURCE_DIR}/common
    ${PROJECT_SOURCE_DIR}/server
    ${PROJECT_SOURCE_DIR}/client
)

add_executable(hangman_server
    server/server.c
    server/network.c
    server/words.c
    server/leaderboard.c
    server/timer.c 
    common/utils.c
)
target_link_libraries(hangman_server pthread)

add_executable(hangman_client
    client/client.c
    client/render.c
    client/gui.c
    client/network_client.c
    client/sdl_init.c
    common/utils.c
)

# target_link_libraries(hangman_client SDL2 SDL2_ttf pthread)
target_link_libraries(hangman_client ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARY} pthread)
