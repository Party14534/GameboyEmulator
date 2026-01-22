cmake_minimum_required(VERSION 3.15)
project(gameboy)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
set(SFML_STATIC_LIBRARIES ON)

# Find SFML
find_package(SFML 3.0 REQUIRED COMPONENTS Graphics Window System)

# Enable testing
enable_testing()

# Option 1: Try to find installed GTest first
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    # Fallback: Download GTest if not found
    message(STATUS "GTest not found, downloading it...")
    include(FetchContent)
    
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1  # Known working version
    )
    
    FetchContent_MakeAvailable(googletest)
    
    # Set this to prevent conflicts
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
endif()

# Find all source files (except main.cpp)
file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS "src/**/*.cpp")
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# Find main.cpp separately
file(GLOB MAIN_SOURCE CONFIGURE_DEPENDS "src/main.cpp")

# Verify sources
if(NOT LIB_SOURCES)
    message(FATAL_ERROR "No library source files found!")
endif()

if(NOT MAIN_SOURCE)
    message(FATAL_ERROR "main.cpp not found!")
endif()

# Create library
add_library(${PROJECT_NAME}_lib STATIC ${LIB_SOURCES})

# Add cereal include directory
target_include_directories(${PROJECT_NAME}_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Create main executable
add_executable(${PROJECT_NAME} ${MAIN_SOURCE})

# Link SFML
target_link_libraries(${PROJECT_NAME}_lib PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${PROJECT_NAME}_lib
)

# Test configuration
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "tests/**/*.cpp")

if(TEST_SOURCES)
    add_executable(${PROJECT_NAME}_tests ${TEST_SOURCES})
    
    # Different linking based on how GTest was acquired
    if(GTest_FOUND)
        target_link_libraries(${PROJECT_NAME}_tests PRIVATE
            ${PROJECT_NAME}_lib
            GTest::GTest
            GTest::Main
        )
    else()
        target_link_libraries(${PROJECT_NAME}_tests PRIVATE
            ${PROJECT_NAME}_lib
            gtest_main
            gtest
        )
    endif()
    
    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME}_tests)
else()
    message(WARNING "No test files found in tests/ directory!")
endif()
