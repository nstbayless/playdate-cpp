cmake_minimum_required(VERSION 3.14)
#set(CMAKE_C_STANDARD 11)
#
#set(ENVSDK $ENV{PLAYDATE_SDK_PATH})
#
#if (NOT ${ENVSDK} STREQUAL "")
#	# Convert path from Windows
#	file(TO_CMAKE_PATH ${ENVSDK} SDK)
#else()
#	execute_process(
#			COMMAND bash -c "egrep '^\\s*SDKRoot' $HOME/.Playdate/config"
#			COMMAND head -n 1
#			COMMAND cut -c9-
#			OUTPUT_VARIABLE SDK
#			OUTPUT_STRIP_TRAILING_WHITESPACE
#	)
#endif()
#
#if (NOT EXISTS ${SDK})
#	message(FATAL_ERROR "SDK Path not found; set ENV value PLAYDATE_SDK_PATH")
#	return()
#endif()
#
#set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
#set(CMAKE_XCODE_GENERATE_SCHEME TRUE)
#
## Game Name Customization
#set(PLAYDATE_GAME_NAME Particles)
#set(PLAYDATE_GAME_DEVICE Particles_DEVICE)
#
#project(${PLAYDATE_GAME_NAME} C ASM)
#
#file(GLOB IMAGES
#	"Source/images/*"
#)
#
#if (TOOLCHAIN STREQUAL "armgcc")
#	add_executable(${PLAYDATE_GAME_DEVICE} main.c)
#else()
#	add_library(${PLAYDATE_GAME_NAME} SHARED main.c ${IMAGES})
#endif()
#
#include(${SDK}/C_API/buildsupport/playdate_game.cmake)

cmake_minimum_required(VERSION 3.19)
project("ParticlesCPP")

# C++ 20 and up is required in order to include the playdate headers
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_XCODE_GENERATE_SCHEME TRUE)

# As this is part of the repo, we can't demonstrate this aptly, but for a full
# project, you will want to include this project in order to use the build
# system, which might looks something like this:
#
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/playdate-cpp)

# Now we can declare our application
add_playdate_application(Particles)

# Add its sources, and you're good to go!
target_sources(Particles PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
