cmake_minimum_required(VERSION 3.10)
project(HangmanRace)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -Wextra -pthread")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
# Usamos pkg-config para obtener la ruta de los headers de SDL2
execute_process(COMMAND pkg-config --cflags sdl2 OUTPUT_VARIABLE SDL2_CFLAGS)

# Incluimos los directorios que nos da pkg-config
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
