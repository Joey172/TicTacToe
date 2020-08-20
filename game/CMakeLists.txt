# CMakeList.txt : CMake project for TicTacToe, include source and define
# project specific logic here.
#
include(CMakePrintHelpers)

cmake_minimum_required (VERSION 3.8)

project(game)

#set(SFML_STATIC_LIBRARIES TRUE)

cmake_print_variables(SFML_DIR)

find_package(SFML 2.5 COMPONENTS graphics network window network system REQUIRED)
cmake_print_variables(SFML_STATIC_LIBRARIES)

# Add source to this project's executable.
add_executable (game 
	Game.cpp Game.h GameButton.cpp GameButton.h GameSettings.h GameSettings.cpp 
	History.cpp History.h main.cpp Menu.cpp Menu.h OnlinePlay.cpp 
	OnlinePlay.h Player.cpp Player.h resource.h Subboard.cpp
	Subboard.h Tile.h ttt_util.cpp ttt_util.h)

target_link_libraries(game sfml-graphics sfml-window sfml-network)
target_include_directories(game PRIVATE SFML_DIR)

#configure_file(./../assets/p1.png "./resources/p1.png" COPYONLY)

file(GLOB "")
# TODO: Add tests and install targets if needed.
