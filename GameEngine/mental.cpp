/**
 * @file mental.cpp
 * @brief Example application demonstrating Mental SDK usage
 * @author Mental SDK Team
 * @version 1.0.0
 * 
 * This example creates a simple window using the Mental SDK and runs
 * the main rendering loop. It demonstrates basic usage of the window
 * manager and renderer components.
 */
#include "../SDK/sdk.hpp"

/**
 * @brief Main entry point for the example application
 * @return Exit code (0 for success)
 * 
 * Creates a window with default settings and runs the main loop.
 * The window will display until the user closes it.
 */
int main() {
    // Create window configuration with default values
    mentalsdk::WindowManagerInfo info{};
    
    // Create window manager with GLFW window and render pipeline
    mentalsdk::CMentalWindowManager<mentalsdk::GLFWwindow, mentalsdk::CMentalRenderer<mentalsdk::CMentalRenderPipeline<>>> game{info};
    
    // Start the main rendering loop
    game.run();
    
    return 0;
}