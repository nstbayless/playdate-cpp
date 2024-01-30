cmake_minimum_required(VERSION 3.19)
project("HelloWorldCPP")

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
add_playdate_application(HelloWorld)

# Add its sources, and you're good to go!
target_sources(HelloWorld PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
