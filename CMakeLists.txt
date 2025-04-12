cmake_minimum_required(VERSION 3.12)
project(PongGame)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Include the SFML library
set(SFML_ROOT ${CMAKE_SOURCE_DIR}/SFML-3.0.0)
set(CMAKE_MODULE_PATH "${SFML_ROOT}/lib/cmake/SFML")

find_package(SFML 3.0 COMPONENTS system window graphics network audio REQUIRED)

# Add your source files
file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(PongGame ${SOURCES})

# Link SFML libraries
target_link_libraries(PongGame sfml-system sfml-window sfml-graphics sfml-audio sfml-network)