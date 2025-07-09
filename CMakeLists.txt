cmake_minimum_required(VERSION 3.15)
project(gameboy)

# Set C++ standard (SFML 3.0 requires at least C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SFML_STATIC_LIBRARIES ON)

# Find SFML 3.0
find_package(SFML 3.0 REQUIRED COMPONENTS Graphics Window System)

# Auto-find all source files (modern approach)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/**/*.cpp" "src/*.cpp")

# Verify sources were found
if(NOT SOURCES)
    message(FATAL_ERROR "No source files found in src/ directory!")
endif()

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link SFML libraries (must come AFTER add_executable)
target_link_libraries(${PROJECT_NAME} PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

