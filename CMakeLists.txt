cmake_minimum_required(VERSION 3.0)

# Set project name and version
project(PROJECT VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Suppress regeneration of build files
set(CMAKE_SUPPRESS_REGENERATION ON)

# Find Boost
find_package(Boost REQUIRED COMPONENTS system thread)

message("Boost version: ${Boost_VERSION}")
message("Boost libraries: ${Boost_LIBRARIES}")

# Add your executable
add_executable(ApplicationC PenguinTCP-Main.cpp)

# Include directories
target_include_directories(ApplicationC
    PRIVATE
    ${Boost_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(ApplicationC
    ${Boost_LIBRARIES}
)
