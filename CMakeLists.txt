cmake_minimum_required(VERSION 3.16)
project(MentalSDK VERSION 1.0.0 LANGUAGES C CXX)

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

# Find GLM
find_package(glm QUIET)
if(NOT glm_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(GLM glm)
    endif()
endif()

# Add local ufbx library
add_library(ufbx_lib STATIC External/ufbx/ufbx.c)
target_include_directories(ufbx_lib PUBLIC External/ufbx)
set_target_properties(ufbx_lib PROPERTIES LINKER_LANGUAGE C)

# Add ImGui library
add_library(imgui_lib STATIC
    External/imgui/imgui.cpp
    External/imgui/imgui_demo.cpp
    External/imgui/imgui_draw.cpp
    External/imgui/imgui_tables.cpp
    External/imgui/imgui_widgets.cpp
    External/imgui/backends/imgui_impl_glfw.cpp
    External/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui_lib PUBLIC 
    External/imgui
    External/imgui/backends
)
target_compile_definitions(imgui_lib PUBLIC 
    IMGUI_IMPL_OPENGL_LOADER_GLEW
    IMGUI_ENABLE_DOCKING
    IMGUI_ENABLE_VIEWPORTS
)

# Link ImGui with GLFW and OpenGL
if(glfw3_FOUND)
    target_link_libraries(imgui_lib PUBLIC glfw ${OPENGL_LIBRARIES})
else()
    target_include_directories(imgui_lib PUBLIC ${GLFW_INCLUDE_DIRS})
    target_link_libraries(imgui_lib PUBLIC ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES})
endif()

# Link GLEW to ImGui
if(GLEW_FOUND AND TARGET GLEW::GLEW)
    target_link_libraries(imgui_lib PUBLIC GLEW::GLEW)
elseif(GLEW_FOUND)
    target_include_directories(imgui_lib PUBLIC ${GLEW_INCLUDE_DIRS})
    target_link_libraries(imgui_lib PUBLIC ${GLEW_LIBRARIES})
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

if(NOT glm_FOUND AND NOT GLM_FOUND)
    message(FATAL_ERROR "GLM not found. Please install GLM:\n"
                        "  macOS: brew install glm\n"
                        "  Ubuntu/Debian: sudo apt-get install libglm-dev\n"
                        "  Fedora: sudo dnf install glm-devel")
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/SDK)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/External)

# Collect source files (only .cpp files that aren't templates)
set(SOURCES
    SDK/Renderer/Object/CMentalFBXModel.cpp
    SDK/Renderer/Object/CMentalOBJModel.cpp
    SDK/WindowManager/CMentalWindowManager.cpp
    SDK/UI/CMentalImGui.cpp
)

# For header-only library, we still want to track headers
file(GLOB_RECURSE HEADERS
    "SDK/*.hpp"
)

# Create the SDK library as STATIC (with implementation files)
add_library(MentalSDK STATIC ${SOURCES})

# Set target properties for static library
target_include_directories(MentalSDK PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/SDK
    ${OPENGL_INCLUDE_DIRS}
)

# Link libraries based on what was found
if(glfw3_FOUND)
    target_link_libraries(MentalSDK PUBLIC
        glfw
        ${OPENGL_LIBRARIES}
    )
else()
    target_include_directories(MentalSDK PUBLIC ${GLFW_INCLUDE_DIRS})
    target_link_libraries(MentalSDK PUBLIC
        ${GLFW_LIBRARIES}
        ${OPENGL_LIBRARIES}
    )
endif()

# Link GLEW
if(GLEW_FOUND AND TARGET GLEW::GLEW)
    target_link_libraries(MentalSDK PUBLIC GLEW::GLEW)
elseif(GLEW_FOUND)
    target_include_directories(MentalSDK PUBLIC ${GLEW_INCLUDE_DIRS})
    target_link_libraries(MentalSDK PUBLIC ${GLEW_LIBRARIES})
endif()

# Link GLM
if(glm_FOUND AND TARGET glm::glm)
    target_link_libraries(MentalSDK PUBLIC glm::glm)
elseif(GLM_FOUND)
    target_include_directories(MentalSDK PUBLIC ${GLM_INCLUDE_DIRS})
endif()

# Link ufbx and imgui
target_link_libraries(MentalSDK PUBLIC ufbx_lib imgui_lib)

# Create the example executable
add_executable(mental_engine GameEngine/mental.cpp)

target_link_libraries(mental_engine PRIVATE
    MentalSDK
)

# Create MentalEngine directory structure and copy common files
add_custom_command(TARGET mental_engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/MentalEngine/bin
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/MentalEngine/common
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/GameEngine/Common ${CMAKE_BINARY_DIR}/MentalEngine/common
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:mental_engine> ${CMAKE_BINARY_DIR}/MentalEngine/bin/
    COMMAND ${CMAKE_COMMAND} -E create_symlink bin/mental_engine ${CMAKE_BINARY_DIR}/MentalEngine/mental_engine
    COMMENT "Setting up MentalEngine directory structure and copying files"
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