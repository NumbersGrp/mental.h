#include "mentalsdk.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <memory>

int main() {
    try {
        std::cout << "=== Mental SDK Example with Lua ===" << std::endl;
        
        // Create window manager (this will initialize GLFW, create window, and setup renderer)
        mentalsdk::CMentalWindowManager<> windowManager;
        
        windowManager.SetTitle("Mental SDK - Camera Mouse Control");
        
        int width = 0;
        int height = 0;
        windowManager.GetWindowSize(width, height);
        std::cout << "Window size: " << width << "x" << height << std::endl;
        
        // Create shader material
        auto shaderMaterial = std::make_shared<mentalsdk::CMentalShaderMaterial>();
        
        // Load shaders from files
        if (!shaderMaterial->CompileFromFiles("Common/Shaders/Rainbow/vertex_shader.glsl", 
                                             "Common/Shaders/Rainbow/fragment_shader.glsl")) {
            std::cerr << "Failed to compile shaders!" << std::endl;
            return -1;
        }
        
        // Create rectangle object
        auto rectangle = std::make_shared<mentalsdk::CMentalObject>();
        rectangle->InitializeRectangle();
        rectangle->SetMaterial(shaderMaterial);
        rectangle->SetPosition(0.0f, 0.0f, 0.0f);
        rectangle->SetScale(0.8f, 0.8f, 1.0f);
        rectangle->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
        
        // Load Lua script for the rectangle
        rectangle->LoadLuaScript("Common/Scripts/simple_rectangle.lua");
        
        // Create camera with mouse control Lua script
        auto camera = std::make_shared<mentalsdk::CMentalCamera>();
        camera->LoadLuaScript("Common/Scripts/camera_mouse.lua");
        
        // Add objects to render pipeline
        auto* renderer = windowManager.GetRenderer();
        if (renderer) {
            auto* pipeline = renderer->GetRenderPipeline();
            if (pipeline) {
                pipeline->AddObject(rectangle);
                pipeline->SetCamera(camera);
                std::cout << "Rectangle and camera with Lua scripts added to render pipeline" << std::endl;
            }
        }
        
        // Main loop
        std::cout << "Starting main loop..." << std::endl;
        
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        while (!windowManager.ShouldClose()) {
            // Calculate delta time
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;
            
            // Clear screen
            windowManager.Clear(0.1f, 0.1f, 0.1f, 1.0f);
            
            // Update and render all objects
            if (renderer) {
                auto* pipeline = renderer->GetRenderPipeline();
                if (pipeline) {
                    pipeline->UpdateAll(deltaTime);  // This will call Lua update functions
                    pipeline->RenderAll();           // This will call Lua render functions
                }
            }
            
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