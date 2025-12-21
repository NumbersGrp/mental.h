#include "mentalsdk.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <memory>

int main() {
    try {
        std::cout << "=== Mental SDK - Root Node Scene Example ===" << std::endl;
        
        // Create window manager
        mentalsdk::CMentalWindowManager<> windowManager;
        
        windowManager.SetTitle("Mental SDK - Root Node Scene");
        
        int width = 0;
        int height = 0;
        windowManager.GetWindowSize(width, height);
        std::cout << "Window size: " << width << "x" << height << std::endl;
        
        // Get renderer and pipeline
        auto* renderer = windowManager.GetRenderer();
        if (!renderer) {
            std::cerr << "Failed to get renderer!" << std::endl;
            return -1;
        }
        
        auto* pipeline = renderer->GetRenderPipeline();
        if (!pipeline) {
            std::cerr << "Failed to get render pipeline!" << std::endl;
            return -1;
        }
        
        // Get root node from pipeline
        auto rootNode = pipeline->GetRootNode();
        if (!rootNode) {
            std::cerr << "Failed to get root node!" << std::endl;
            return -1;
        }
        
        // Load root scene script - this will create all objects and shaders
        std::cout << "Loading root scene script..." << std::endl;
        rootNode->LoadLuaScript("Common/Scripts/root_scene.lua");
        
        // Enable hot reload for the script
        rootNode->EnableHotReload(true);
        std::cout << "Hot reload enabled - you can edit root_scene.lua and see changes immediately!" << std::endl;
        
        // Create and set up camera
        auto camera = std::make_shared<mentalsdk::CMentalCamera>();
        camera->LoadLuaScript("Common/Scripts/camera_mouse.lua");
        
        // Set camera to both root node and pipeline
        rootNode->SetCamera(camera);
        pipeline->SetCamera(camera);
        
        std::cout << "Scene setup completed!" << std::endl;
        
        // Main loop
        std::cout << "Starting main loop with root node scene..." << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  WASD - Move camera" << std::endl;
        std::cout << "  Left mouse button + drag - Rotate camera" << std::endl;
        std::cout << "  Edit Common/Scripts/root_scene.lua to see changes in real-time!" << std::endl;
        std::cout << "  All objects are created and managed by Lua script!" << std::endl;
        
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        while (!windowManager.ShouldClose()) {
            // Calculate delta time
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;
            
            // Clear screen with darker background to show glow effect better
            windowManager.Clear(0.02F, 0.02F, 0.08F, 1.0F);
            
            // Update and render all objects through pipeline
            pipeline->UpdateAll(deltaTime);
            pipeline->RenderAll();
            
            // Swap buffers and poll events
            windowManager.SwapBuffers();
            windowManager.PollEvents();
            
            // Limit frame rate to ~60 FPS
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
        
        std::cout << "Main loop ended" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    
    std::cout << "Application finished successfully" << std::endl;
    return 0;
}