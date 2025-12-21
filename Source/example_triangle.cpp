#include "mentalsdk.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <memory>

int main() {
    try {
        std::cout << "=== Mental SDK - Glowing Triangle Example ===" << std::endl;
        
        // Create window manager
        mentalsdk::CMentalWindowManager<> windowManager;
        
        windowManager.SetTitle("Mental SDK - Glowing Triangle");
        
        int width = 0;
        int height = 0;
        windowManager.GetWindowSize(width, height);
        std::cout << "Window size: " << width << "x" << height << std::endl;
        
        // Create shader materials
        auto rainbowShader = std::make_shared<mentalsdk::CMentalShaderMaterial>();
        auto glowShader = std::make_shared<mentalsdk::CMentalShaderMaterial>();
        
        // Load rainbow shaders for rectangle
        if (!rainbowShader->CompileFromFiles("Common/Shaders/Rainbow/vertex_shader.glsl", 
                                            "Common/Shaders/Rainbow/fragment_shader.glsl")) {
            std::cerr << "Failed to compile rainbow shaders!" << std::endl;
            return -1;
        }
        
        // Load glow shaders for triangle
        if (!glowShader->CompileFromFiles("Common/Shaders/Glow/vertex_shader.glsl", 
                                         "Common/Shaders/Glow/fragment_shader.glsl")) {
            std::cerr << "Failed to compile glow shaders!" << std::endl;
            return -1;
        }
        
        // Create rectangle object
        auto rectangle = std::make_shared<mentalsdk::CMentalObject>();
        rectangle->InitializeRectangle();
        rectangle->SetMaterial(rainbowShader);
        rectangle->SetPosition(-1.5F, 0.0F, 0.0F);  // Смещаем влево
        rectangle->SetScale(0.8F, 0.8F, 1.0F);
        rectangle->SetColor(1.0F, 1.0F, 1.0F, 1.0F);
        
        // Load Lua script for the rectangle
        rectangle->LoadLuaScript("Common/Scripts/simple_rectangle.lua");
        
        // Create triangle object with glow shader
        auto triangle = std::make_shared<mentalsdk::CMentalObject>();
        triangle->InitializeTriangle();
        triangle->SetMaterial(glowShader);
        triangle->SetPosition(2.0F, 1.0F, 0.0F);   // Дальше вправо и выше
        triangle->SetScale(1.0F, 1.0F, 1.0F);
        triangle->SetColor(1.0F, 0.8F, 0.2F, 1.0F); // Золотистый цвет
        
        // Load Lua script for the glowing triangle
        triangle->LoadLuaScript("Common/Scripts/glowing_triangle_single.lua");
        
        // Create camera with mouse control
        auto camera = std::make_shared<mentalsdk::CMentalCamera>();
        camera->LoadLuaScript("Common/Scripts/camera_mouse.lua");
        
        // Add objects to render pipeline
        auto* renderer = windowManager.GetRenderer();
        if (renderer) {
            auto* pipeline = renderer->GetRenderPipeline();
            if (pipeline) {
                pipeline->AddObject(rectangle);
                pipeline->AddObject(triangle);
                pipeline->SetCamera(camera);
                std::cout << "Rectangle and glowing triangle added to render pipeline" << std::endl;
            }
        }
        
        // Main loop
        std::cout << "Starting main loop with glowing triangle..." << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  WASD - Move camera" << std::endl;
        std::cout << "  Left mouse button + drag - Rotate camera" << std::endl;
        std::cout << "  Watch the glowing triangle on the right!" << std::endl;
        
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        while (!windowManager.ShouldClose()) {
            // Calculate delta time
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;
            
            // Clear screen with darker background to show glow effect better
            windowManager.Clear(0.05F, 0.05F, 0.1F, 1.0F);
            
            // Update and render all objects
            if (renderer) {
                auto* pipeline = renderer->GetRenderPipeline();
                if (pipeline) {
                    pipeline->UpdateAll(deltaTime);
                    pipeline->RenderAll();
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