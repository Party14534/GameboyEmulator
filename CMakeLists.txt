cmake_minimum_required(VERSION 3.15...4.2)
project(gameboy)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")

# Clear any cached SFML paths to avoid finding old version
unset(SFML_DIR CACHE)
unset(SFML_ROOT CACHE)

# Get SFML 3.0 via FetchContent
include(FetchContent)

message(STATUS "Fetching SFML 3.0...")
FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0  # Use the latest 3.0 release
)

# Set SFML build options BEFORE making it available
set(SFML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
set(SFML_USE_SYSTEM_DEPS OFF CACHE BOOL "" FORCE)

# Make SFML available - DON'T call find_package after this
FetchContent_MakeAvailable(sfml)

# Try to find ImGui-SFML
find_package(ImGui-SFML QUIET)

if(NOT ImGui-SFML_FOUND)
    message(STATUS "ImGui-SFML not found, downloading it...")
    
    # Fetch ImGui
    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.91.1
    )
    
    # Make ImGui available first (needed for ImGuiFileDialog)
    FetchContent_MakeAvailable(imgui)
    
    # Fetch ImGui-SFML (master branch for SFML 3.0 support)
    FetchContent_Declare(
        imgui-sfml
        GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
        GIT_TAG master
    )
    
    # Set ImGui-SFML options
    set(IMGUI_DIR ${CMAKE_BINARY_DIR}/_deps/imgui-src)
    set(IMGUI_SFML_FIND_SFML OFF)  # Tell ImGui-SFML not to look for SFML
    set(IMGUI_SFML_IMGUI_DEMO ON)
    
    FetchContent_MakeAvailable(imgui-sfml)
endif()

# Fetch ImGuiFileDialog
message(STATUS "Fetching ImGuiFileDialog...")
FetchContent_Declare(
    ImGuiFileDialog
    GIT_REPOSITORY https://github.com/aiekick/ImGuiFileDialog.git
    GIT_TAG v0.6.7
)

# Set options for ImGuiFileDialog
set(USE_IMGUI_TABLES ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(ImGuiFileDialog)

# Manually add ImGui include directory to ImGuiFileDialog
target_include_directories(ImGuiFileDialog PUBLIC 
    ${CMAKE_BINARY_DIR}/_deps/imgui-src
)

# Enable testing
enable_testing()

# Option 1: Try to find installed GTest first
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    # Fallback: Download GTest if not found
    message(STATUS "GTest not found, downloading it...")
    
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
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

# Link SFML, ImGui-SFML, and ImGuiFileDialog to library
# Use the targets from FetchContent, not find_package
target_link_libraries(${PROJECT_NAME}_lib PUBLIC
    sfml-graphics
    sfml-window
    sfml-system
    ImGui-SFML::ImGui-SFML
    ImGuiFileDialog
)

# Link library to main executable
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
