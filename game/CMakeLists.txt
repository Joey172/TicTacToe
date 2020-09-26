# CMakeList.txt : CMake project for TicTacToe, include source and define
# project specific logic here.
#
include(CMakePrintHelpers)

cmake_minimum_required (VERSION 3.8)

project(game)
SET(CMAKE_CXX_STANDARD 11)



if(MSVC)
  set(SFML_STATIC_LIBRARIES TRUE)
endif(MSVC)
cmake_print_variables(SFML_DIR)

find_package(SFML 2.5 COMPONENTS graphics network window network system REQUIRED)
cmake_print_variables(SFML_STATIC_LIBRARIES)

# Add source to this project's executable.
add_executable (game
	Game.cpp Game.h "GUI/GameButton.cpp" "GUI/GameButton.h" GameSettings.h GameSettings.cpp 
	History.cpp History.h main.cpp Menu.cpp Menu.h OnlinePlay.cpp 
	OnlinePlay.h Player.cpp Player.h resource.h Subboard.cpp
	Subboard.h Tile.h ttt_util.cpp ttt_util.h "GUI/EditBox.h" "GUI/EditBox.cpp")

target_link_libraries(game sfml-graphics sfml-window sfml-network)
target_include_directories(game PRIVATE SFML_DIR)

#set_target_properties(game PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
if(WIN32)
   set_target_properties(game PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
   set_target_properties(game PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")

   set_target_properties(game PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
   set_target_properties(game PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")

   set_target_properties(game PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:windows")
   set_target_properties(game PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:windows")
endif(WIN32)
####
#configure_file(./../assets/p1.png "./resources/p1.png" COPYONLY)
add_custom_command(TARGET game PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:game>/assets)

# TODO: Add tests and install targets if needed.
