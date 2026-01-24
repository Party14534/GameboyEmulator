cmake_minimum_required(VERSION 3.15...4.2)
project(gameboy LANGUAGES CXX)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Release optimizations
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -DNDEBUG")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /GL /arch:AVX2 /DNDEBUG")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
endif()

# Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")

# ============================================================================
# DEPENDENCIES
# ============================================================================

# Clear cached paths
unset(SFML_DIR CACHE)
unset(SFML_ROOT CACHE)

include(FetchContent)

# Fetch and build SFML first
message(STATUS "Fetching SFML 3.0...")
FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
)

set(SFML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
set(SFML_USE_SYSTEM_DEPS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(sfml)

# Now fetch ImGui and ImGui-SFML together in the right order
message(STATUS "Fetching ImGui and ImGui-SFML...")

# First declare both
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.1
)

FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG master
)

# Set ImGui-SFML options BEFORE fetching
set(IMGUI_SFML_FIND_SFML OFF CACHE BOOL "" FORCE)
set(IMGUI_SFML_IMGUI_DEMO ON CACHE BOOL "" FORCE)

# Make ImGui available first
FetchContent_MakeAvailable(imgui)

# Now get the actual path to ImGui
set(IMGUI_DIR ${imgui_SOURCE_DIR} CACHE PATH "Path to ImGui source directory" FORCE)
message(STATUS "ImGui directory set to: ${IMGUI_DIR}")

# Now make ImGui-SFML available (it will find IMGUI_DIR)
FetchContent_MakeAvailable(imgui-sfml)

# Optional: Apply optimizations to ImGui-SFML
if(CMAKE_BUILD_TYPE STREQUAL "Release" AND TARGET ImGui-SFML)
    target_compile_options(ImGui-SFML PRIVATE 
        $<$<CXX_COMPILER_ID:GNU,Clang>:-O3>
        $<$<CXX_COMPILER_ID:MSVC>:/O2>
    )
endif()

# Fetch ImGuiFileDialog
message(STATUS "Fetching ImGuiFileDialog...")
FetchContent_Declare(
    ImGuiFileDialog
    GIT_REPOSITORY https://github.com/aiekick/ImGuiFileDialog.git
    GIT_TAG v0.6.7
)

set(USE_IMGUI_TABLES ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(ImGuiFileDialog)

# Add ImGui includes to ImGuiFileDialog
target_include_directories(ImGuiFileDialog PUBLIC ${IMGUI_DIR})

# Apply optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release" AND TARGET ImGuiFileDialog)
    target_compile_options(ImGuiFileDialog PRIVATE 
        $<$<CXX_COMPILER_ID:GNU,Clang>:-O3>
        $<$<CXX_COMPILER_ID:MSVC>:/O2>
    )
endif()

# Apply optimizations to SFML
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(TARGET sfml-graphics)
        target_compile_options(sfml-graphics PRIVATE 
            $<$<CXX_COMPILER_ID:GNU,Clang>:-O3>
            $<$<CXX_COMPILER_ID:MSVC>:/O2>
        )
    endif()
    if(TARGET sfml-window)
        target_compile_options(sfml-window PRIVATE 
            $<$<CXX_COMPILER_ID:GNU,Clang>:-O3>
            $<$<CXX_COMPILER_ID:MSVC>:/O2>
        )
    endif()
    if(TARGET sfml-system)
        target_compile_options(sfml-system PRIVATE 
            $<$<CXX_COMPILER_ID:GNU,Clang>:-O3>
            $<$<CXX_COMPILER_ID:MSVC>:/O2>
        )
    endif()
endif()

# ============================================================================
# MAIN PROJECT
# ============================================================================

# Find source files
file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS "src/**/*.cpp")
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
file(GLOB MAIN_SOURCE CONFIGURE_DEPENDS "src/main.cpp")

if(NOT LIB_SOURCES)
    message(FATAL_ERROR "No library source files found!")
endif()

if(NOT MAIN_SOURCE)
    message(FATAL_ERROR "main.cpp not found!")
endif()

# Create library
add_library(${PROJECT_NAME}_lib STATIC ${LIB_SOURCES})

# Add include directories
target_include_directories(${PROJECT_NAME}_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Apply optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME}_lib PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-O3>
        $<$<CXX_COMPILER_ID:GNU>:-ftree-vectorize>
        $<$<CXX_COMPILER_ID:Clang>:-fvectorize>
        $<$<CXX_COMPILER_ID:MSVC>:/O2 /Ob2 /Oi /Ot>
    )
endif()

# Link dependencies
target_link_libraries(${PROJECT_NAME}_lib PUBLIC
    sfml-graphics
    sfml-window
    sfml-system
    ImGui-SFML::ImGui-SFML
    ImGuiFileDialog
)

# Create main executable
add_executable(${PROJECT_NAME} ${MAIN_SOURCE})

# Apply optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-O3>
        $<$<CXX_COMPILER_ID:MSVC>:/O2>
    )
endif()

# Link library
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)

# ============================================================================
# TESTS
# ============================================================================

enable_testing()
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    message(STATUS "GTest not found, downloading it...")
    
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    
    FetchContent_MakeAvailable(googletest)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
endif()

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "tests/**/*.cpp")
if(TEST_SOURCES)
    add_executable(${PROJECT_NAME}_tests ${TEST_SOURCES})
    
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
