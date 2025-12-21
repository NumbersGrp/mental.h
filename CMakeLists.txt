cmake_minimum_required(VERSION 3.14)
project(MentalProject LANGUAGES C CXX)

# Set C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform-specific settings
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")
endif()

# Add external dependencies
if(APPLE)
    # On macOS, use Homebrew spdlog
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SPDLOG REQUIRED spdlog)
    set(SPDLOG_LIBRARIES ${SPDLOG_LIBRARIES})
    set(SPDLOG_INCLUDE_DIRS ${SPDLOG_INCLUDE_DIRS})
else()
    # On other platforms, try submodule first
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/External/spdlog/CMakeLists.txt")
        add_subdirectory(External/spdlog)
    else()
        find_package(spdlog REQUIRED)
    endif()
endif()

# Find packages based on platform
if(APPLE)
    # On macOS, use Homebrew-installed packages
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW REQUIRED glfw3)
    pkg_check_modules(GLEW REQUIRED glew)
    pkg_check_modules(LUA REQUIRED lua5.4)
    
    # Find MoltenVK via Homebrew
    set(VULKAN_SDK "$ENV{VULKAN_SDK}")
    if(NOT VULKAN_SDK)
        # Try to find MoltenVK from Homebrew
        execute_process(
            COMMAND brew --prefix molten-vk
            OUTPUT_VARIABLE MOLTENVK_PREFIX
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(MOLTENVK_PREFIX)
            set(Vulkan_INCLUDE_DIRS "${MOLTENVK_PREFIX}/include")
            set(Vulkan_LIBRARIES "${MOLTENVK_PREFIX}/lib/libMoltenVK.dylib")
            set(Vulkan_FOUND TRUE)
            message(STATUS "Found MoltenVK via Homebrew: ${MOLTENVK_PREFIX}")
        else()
            message(FATAL_ERROR "MoltenVK not found. Install with: brew install molten-vk")
        endif()
    else()
        find_package(Vulkan REQUIRED)
    endif()
    
    # Set GLFW variables for consistency
    set(GLFW_LIBRARIES ${GLFW_LIBRARIES})
    set(GLFW_INCLUDE_DIRS ${GLFW_INCLUDE_DIRS})
    set(GLEW_LIBRARIES ${GLEW_LIBRARIES})
    set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIRS})
    set(LUA_LIBRARIES ${LUA_LIBRARIES})
    set(LUA_INCLUDE_DIRS ${LUA_INCLUDE_DIRS})
else()
    # On other platforms, use submodules if available
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/External/glfw/CMakeLists.txt")
        set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        add_subdirectory(External/glfw)
        set(GLFW_LIBRARIES glfw)
    else()
        find_package(glfw3 REQUIRED)
        set(GLFW_LIBRARIES glfw)
    endif()
    
    # Find GLEW
    find_package(GLEW REQUIRED)
    set(GLEW_LIBRARIES GLEW::GLEW)
    
    find_package(Vulkan REQUIRED)
endif()

# Add vk-bootstrap if available
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/External/vk-bootstrap/CMakeLists.txt")
    add_subdirectory(External/vk-bootstrap)
    set(VK_BOOTSTRAP_AVAILABLE TRUE)
else()
    set(VK_BOOTSTRAP_AVAILABLE FALSE)
    message(WARNING "vk-bootstrap submodule not found. Vulkan bootstrap functionality will be limited.")
endif()

# Collect source files
file(GLOB_RECURSE CPP_SOURCES "Source/*.cpp")
file(GLOB_RECURSE C_SOURCES "Source/*.c")

# Remove example files from main build
list(REMOVE_ITEM CPP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Source/example_mentalsdk.cpp")
list(REMOVE_ITEM CPP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Source/example_smooth_camera.cpp")
list(REMOVE_ITEM CPP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Source/example_triangle.cpp")
list(REMOVE_ITEM CPP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Source/example_multiple_triangles.cpp")
list(REMOVE_ITEM CPP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Source/example_root_node.cpp")

set(ALL_SOURCES ${CPP_SOURCES} ${C_SOURCES})

# Create main executable
add_executable(${PROJECT_NAME} ${ALL_SOURCES})

# Create example executable
add_executable(${PROJECT_NAME}_Example Source/example_mentalsdk.cpp Source/mentalsdk.cpp)

# Create smooth camera example executable
add_executable(${PROJECT_NAME}_SmoothCamera Source/example_smooth_camera.cpp Source/mentalsdk.cpp)

# Create triangle example executable
add_executable(${PROJECT_NAME}_Triangle Source/example_triangle.cpp Source/mentalsdk.cpp)

# Create multiple triangles example executable
add_executable(${PROJECT_NAME}_MultiTriangles Source/example_multiple_triangles.cpp Source/mentalsdk.cpp)

# Create root node scene example executable
add_executable(${PROJECT_NAME}_RootNode Source/example_root_node.cpp Source/mentalsdk.cpp)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${Vulkan_INCLUDE_DIRS}
)

# Include directories for example
target_include_directories(${PROJECT_NAME}_Example PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${Vulkan_INCLUDE_DIRS}
)

# Include directories for smooth camera example
target_include_directories(${PROJECT_NAME}_SmoothCamera PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${Vulkan_INCLUDE_DIRS}
)

# Include directories for triangle example
target_include_directories(${PROJECT_NAME}_Triangle PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${Vulkan_INCLUDE_DIRS}
)

# Include directories for multiple triangles example
target_include_directories(${PROJECT_NAME}_MultiTriangles PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${Vulkan_INCLUDE_DIRS}
)

# Include directories for root node example
target_include_directories(${PROJECT_NAME}_RootNode PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${Vulkan_INCLUDE_DIRS}
)

# Add GLFW and spdlog include directories on macOS
if(APPLE)
    # Add vulkan-headers from Homebrew
    execute_process(
        COMMAND brew --prefix vulkan-headers
        OUTPUT_VARIABLE VULKAN_HEADERS_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    target_include_directories(${PROJECT_NAME} PRIVATE 
        ${GLFW_INCLUDE_DIRS}
        ${SPDLOG_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${LUA_INCLUDE_DIRS}
        "${VULKAN_HEADERS_PREFIX}/include"
    )
    
    target_include_directories(${PROJECT_NAME}_Example PRIVATE 
        ${GLFW_INCLUDE_DIRS}
        ${SPDLOG_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${LUA_INCLUDE_DIRS}
        "${VULKAN_HEADERS_PREFIX}/include"
    )
    
    target_include_directories(${PROJECT_NAME}_SmoothCamera PRIVATE 
        ${GLFW_INCLUDE_DIRS}
        ${SPDLOG_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${LUA_INCLUDE_DIRS}
        "${VULKAN_HEADERS_PREFIX}/include"
    )
    
    target_include_directories(${PROJECT_NAME}_Triangle PRIVATE 
        ${GLFW_INCLUDE_DIRS}
        ${SPDLOG_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${LUA_INCLUDE_DIRS}
        "${VULKAN_HEADERS_PREFIX}/include"
    )
    
    target_include_directories(${PROJECT_NAME}_MultiTriangles PRIVATE 
        ${GLFW_INCLUDE_DIRS}
        ${SPDLOG_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${LUA_INCLUDE_DIRS}
        "${VULKAN_HEADERS_PREFIX}/include"
    )
    
    target_include_directories(${PROJECT_NAME}_RootNode PRIVATE 
        ${GLFW_INCLUDE_DIRS}
        ${SPDLOG_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${LUA_INCLUDE_DIRS}
        "${VULKAN_HEADERS_PREFIX}/include"
    )
endif()

# Compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<PLATFORM_ID:Darwin>:VK_USE_PLATFORM_MACOS_MVK>
    $<$<PLATFORM_ID:Linux>:VK_USE_PLATFORM_XLIB_KHR>
    $<$<PLATFORM_ID:Windows>:VK_USE_PLATFORM_WIN32_KHR>
)

# Link libraries
if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        ${GLFW_LIBRARIES}
        ${SPDLOG_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${LUA_LIBRARIES}
        ${Vulkan_LIBRARIES}
    )
    
    target_link_libraries(${PROJECT_NAME}_Example PRIVATE 
        ${GLFW_LIBRARIES}
        ${SPDLOG_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${LUA_LIBRARIES}
        ${Vulkan_LIBRARIES}
    )
    
    target_link_libraries(${PROJECT_NAME}_SmoothCamera PRIVATE 
        ${GLFW_LIBRARIES}
        ${SPDLOG_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${LUA_LIBRARIES}
        ${Vulkan_LIBRARIES}
    )
    
    target_link_libraries(${PROJECT_NAME}_Triangle PRIVATE 
        ${GLFW_LIBRARIES}
        ${SPDLOG_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${LUA_LIBRARIES}
        ${Vulkan_LIBRARIES}
    )
    
    target_link_libraries(${PROJECT_NAME}_MultiTriangles PRIVATE 
        ${GLFW_LIBRARIES}
        ${SPDLOG_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${LUA_LIBRARIES}
        ${Vulkan_LIBRARIES}
    )
    
    target_link_libraries(${PROJECT_NAME}_RootNode PRIVATE 
        ${GLFW_LIBRARIES}
        ${SPDLOG_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${LUA_LIBRARIES}
        ${Vulkan_LIBRARIES}
    )
    
    # Add library directories for pkg-config libraries
    target_link_directories(${PROJECT_NAME} PRIVATE 
        ${GLFW_LIBRARY_DIRS}
        ${SPDLOG_LIBRARY_DIRS}
        ${GLEW_LIBRARY_DIRS}
        ${LUA_LIBRARY_DIRS}
    )
    
    target_link_directories(${PROJECT_NAME}_Example PRIVATE 
        ${GLFW_LIBRARY_DIRS}
        ${SPDLOG_LIBRARY_DIRS}
        ${GLEW_LIBRARY_DIRS}
        ${LUA_LIBRARY_DIRS}
    )
    
    target_link_directories(${PROJECT_NAME}_SmoothCamera PRIVATE 
        ${GLFW_LIBRARY_DIRS}
        ${SPDLOG_LIBRARY_DIRS}
        ${GLEW_LIBRARY_DIRS}
        ${LUA_LIBRARY_DIRS}
    )
    
    target_link_directories(${PROJECT_NAME}_Triangle PRIVATE 
        ${GLFW_LIBRARY_DIRS}
        ${SPDLOG_LIBRARY_DIRS}
        ${GLEW_LIBRARY_DIRS}
        ${LUA_LIBRARY_DIRS}
    )
    
    target_link_directories(${PROJECT_NAME}_MultiTriangles PRIVATE 
        ${GLFW_LIBRARY_DIRS}
        ${SPDLOG_LIBRARY_DIRS}
        ${GLEW_LIBRARY_DIRS}
        ${LUA_LIBRARY_DIRS}
    )
    
    target_link_directories(${PROJECT_NAME}_RootNode PRIVATE 
        ${GLFW_LIBRARY_DIRS}
        ${SPDLOG_LIBRARY_DIRS}
        ${GLEW_LIBRARY_DIRS}
        ${LUA_LIBRARY_DIRS}
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        ${GLFW_LIBRARIES}
        spdlog::spdlog
        ${GLEW_LIBRARIES}
        ${Vulkan_LIBRARIES}
    )
    
    target_link_libraries(${PROJECT_NAME}_Example PRIVATE 
        ${GLFW_LIBRARIES}
        spdlog::spdlog
        ${GLEW_LIBRARIES}
        ${Vulkan_LIBRARIES}
    )
    
    target_link_libraries(${PROJECT_NAME}_SmoothCamera PRIVATE 
        ${GLFW_LIBRARIES}
        spdlog::spdlog
        ${GLEW_LIBRARIES}
        ${Vulkan_LIBRARIES}
    )
    
    target_link_libraries(${PROJECT_NAME}_Triangle PRIVATE 
        ${GLFW_LIBRARIES}
        spdlog::spdlog
        ${GLEW_LIBRARIES}
        ${Vulkan_LIBRARIES}
    )
    
    target_link_libraries(${PROJECT_NAME}_MultiTriangles PRIVATE 
        ${GLFW_LIBRARIES}
        spdlog::spdlog
        ${GLEW_LIBRARIES}
        ${Vulkan_LIBRARIES}
    )
    
    target_link_libraries(${PROJECT_NAME}_RootNode PRIVATE 
        ${GLFW_LIBRARIES}
        spdlog::spdlog
        ${GLEW_LIBRARIES}
        ${Vulkan_LIBRARIES}
    )
endif()

# Add vk-bootstrap if available
if(VK_BOOTSTRAP_AVAILABLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE vk-bootstrap::vk-bootstrap)
    target_compile_definitions(${PROJECT_NAME} PRIVATE VK_BOOTSTRAP_AVAILABLE)
endif()

# Platform-specific linking
if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    find_library(OPENGL_LIBRARY OpenGL)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        ${COCOA_LIBRARY} 
        ${IOKIT_LIBRARY} 
        ${COREVIDEO_LIBRARY}
        ${OPENGL_LIBRARY}
    )
    target_link_libraries(${PROJECT_NAME}_Example PRIVATE 
        ${COCOA_LIBRARY} 
        ${IOKIT_LIBRARY} 
        ${COREVIDEO_LIBRARY}
        ${OPENGL_LIBRARY}
    )
    target_link_libraries(${PROJECT_NAME}_SmoothCamera PRIVATE 
        ${COCOA_LIBRARY} 
        ${IOKIT_LIBRARY} 
        ${COREVIDEO_LIBRARY}
        ${OPENGL_LIBRARY}
    )
    target_link_libraries(${PROJECT_NAME}_Triangle PRIVATE 
        ${COCOA_LIBRARY} 
        ${IOKIT_LIBRARY} 
        ${COREVIDEO_LIBRARY}
        ${OPENGL_LIBRARY}
    )
    target_link_libraries(${PROJECT_NAME}_MultiTriangles PRIVATE 
        ${COCOA_LIBRARY} 
        ${IOKIT_LIBRARY} 
        ${COREVIDEO_LIBRARY}
        ${OPENGL_LIBRARY}
    )
    target_link_libraries(${PROJECT_NAME}_RootNode PRIVATE 
        ${COCOA_LIBRARY} 
        ${IOKIT_LIBRARY} 
        ${COREVIDEO_LIBRARY}
        ${OPENGL_LIBRARY}
    )
elseif(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(X11 REQUIRED x11)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${X11_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PRIVATE ${X11_INCLUDE_DIRS})
endif()

# Compiler-specific flags
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Wpedantic
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4
        $<$<CONFIG:Debug>:/Od /Zi>
        $<$<CONFIG:Release>:/O2 /DNDEBUG>
    )
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    DEBUG_POSTFIX "_d"
)

# Copy Common directory to bin on build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Common
    ${CMAKE_BINARY_DIR}/bin/Common
    COMMENT "Copying Common directory to bin"
)

# Print configuration summary
message(STATUS "Mental Project Configuration:")
message(STATUS "  C Standard: ${CMAKE_C_STANDARD}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Vulkan Found: ${Vulkan_FOUND}")
message(STATUS "  Vulkan Version: ${Vulkan_VERSION}")
message(STATUS "  VK-Bootstrap Available: ${VK_BOOTSTRAP_AVAILABLE}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
if(APPLE)
    message(STATUS "  macOS Deployment Target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")
    message(STATUS "  Using Homebrew GLFW: ${GLFW_FOUND}")
endif()