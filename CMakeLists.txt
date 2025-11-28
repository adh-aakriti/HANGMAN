cmake_minimum_required(VERSION 3.10)
project(HangmanRace)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -Wextra -pthread")

find_package(SDL2 REQUIRED)
set(TTF_INC /opt/homebrew/include/SDL2)
set(TTF_LIB /opt/homebrew/lib/libSDL2_ttf.dylib)

include_directories(
    common
    server
    client
    ${SDL2_INCLUDE_DIRS}
    ${TTF_INC}
)

add_executable(server_app
    server/server.c
    server/network.c
    server/words.c
    server/leaderboard.c
    common/utils.c
)
target_link_libraries(server_app pthread)

add_executable(client_app
    client/client.c
    client/render.c
    client/gui.c
    client/network_client.c
    client/sdl_init.c
    common/utils.c
)
target_link_libraries(client_app ${SDL2_LIBRARIES} ${TTF_LIB} pthread)


