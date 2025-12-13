cmake_minimum_required(VERSION 3.14)
project(MentalProject)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add spdlog submodule
add_subdirectory(External/spdlog)

# Add vk-bootstrap submodule
add_subdirectory(External/vk-bootstrap)

# Add GLFW submodule
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(External/glfw)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Collect source files
file(GLOB_RECURSE SOURCES "Source/*.cpp")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries to the target
target_link_libraries(${PROJECT_NAME} PRIVATE 
    spdlog::spdlog
    Vulkan::Vulkan
    vk-bootstrap::vk-bootstrap
    glfw
)

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)