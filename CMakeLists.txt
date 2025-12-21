cmake_minimum_required(VERSION 3.16)
project(MentalSDK VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find required packages
find_package(OpenGL REQUIRED)

# Try to find GLFW using different methods
find_package(glfw3 QUIET)
if(NOT glfw3_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(GLFW glfw3)
    endif()
endif()

# Find GLEW
find_package(GLEW QUIET)
if(NOT GLEW_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(GLEW glew)
    endif()
endif()

# Check if we found all dependencies
if(NOT glfw3_FOUND AND NOT GLFW_FOUND)
    message(FATAL_ERROR "GLFW not found. Please install GLFW:\n"
                        "  macOS: brew install glfw\n"
                        "  Ubuntu/Debian: sudo apt-get install libglfw3-dev\n"
                        "  Fedora: sudo dnf install glfw-devel")
endif()

if(NOT GLEW_FOUND)
    message(FATAL_ERROR "GLEW not found. Please install GLEW:\n"
                        "  macOS: brew install glew\n"
                        "  Ubuntu/Debian: sudo apt-get install libglew-dev\n"
                        "  Fedora: sudo dnf install glew-devel")
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/SDK)

# Collect source files (only .cpp files that aren't templates)
set(SOURCES
    # Add non-template implementation files here if needed
)

# For header-only library, we still want to track headers
file(GLOB_RECURSE HEADERS
    "SDK/*.hpp"
)

# Create the SDK library as INTERFACE (header-only)
add_library(MentalSDK INTERFACE)

# Set target properties for header-only library
target_include_directories(MentalSDK INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/SDK
    ${OPENGL_INCLUDE_DIRS}
)

# Link libraries based on what was found
if(glfw3_FOUND)
    target_link_libraries(MentalSDK INTERFACE
        glfw
        ${OPENGL_LIBRARIES}
    )
else()
    target_include_directories(MentalSDK INTERFACE ${GLFW_INCLUDE_DIRS})
    target_link_libraries(MentalSDK INTERFACE
        ${GLFW_LIBRARIES}
        ${OPENGL_LIBRARIES}
    )
endif()

# Link GLEW
if(GLEW_FOUND AND TARGET GLEW::GLEW)
    target_link_libraries(MentalSDK INTERFACE GLEW::GLEW)
elseif(GLEW_FOUND)
    target_include_directories(MentalSDK INTERFACE ${GLEW_INCLUDE_DIRS})
    target_link_libraries(MentalSDK INTERFACE ${GLEW_LIBRARIES})
endif()

# Create the example executable
add_executable(mental_engine GameEngine/mental.cpp)

target_link_libraries(mental_engine PRIVATE
    MentalSDK
)

# Installation
install(TARGETS MentalSDK
    EXPORT MentalSDKTargets
)

install(TARGETS mental_engine
    RUNTIME DESTINATION bin
)

install(DIRECTORY SDK/
    DESTINATION include/MentalSDK
    FILES_MATCHING PATTERN "*.hpp"
)

# Enable testing (optional)
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    # Add test subdirectory if it exists
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
        add_subdirectory(tests)
    endif()
endif()