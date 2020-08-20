# CMakeList.txt : CMake project for TicTacToe, include source and define
# project specific logic here.
#
include(CMakePrintHelpers)

cmake_minimum_required (VERSION 3.8)

project(server)

set(SFML_STATIC_LIBRARIES TRUE)

find_package(SFML 2.5 COMPONENTS graphics network window network system windows required)

# Add source to this project's executable.
add_executable (server 
	server_main.cpp server.h 
	"./../game/ttt_util.cpp" #Files from game
	"./../game/ttt_util.h"
	"./../game/history.cpp"
	"./../game/history.h")

target_link_libraries(server sfml-graphics sfml-window sfml-network)
target_include_directories(server PRIVATE SFML_DIR)


# TODO: Add tests and install targets if needed.
