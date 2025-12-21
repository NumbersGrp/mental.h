#include "mentalsdk.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <memory>

int main() {
    try {
        std::cout << "=== Mental SDK - Multiple Glowing Triangles Example ===" << std::endl;
        
        // Create window manager
        mentalsdk::CMentalWindowManager<> windowManager;
        
        windowManager.SetTitle("Mental SDK - Multiple Glowing Triangles");
        
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
        
        // Load glow shaders for triangles
        if (!glowShader->CompileFromFiles("Common/Shaders/Glow/vertex_shader.glsl", 
                                         "Common/Shaders/Glow/fragment_shader.glsl")) {
            std::cerr << "Failed to compile glow shaders!" << std::endl;
            return -1;
        }
        
        // Create rectangle object (центр)
        auto rectangle = std::make_shared<mentalsdk::CMentalObject>();
        rectangle->InitializeRectangle();
        rectangle->SetMaterial(rainbowShader);
        rectangle->SetPosition(0.0F, 0.0F, 0.0F);  // В центре
        rectangle->SetScale(0.6F, 0.6F, 1.0F);
        rectangle->SetColor(1.0F, 1.0F, 1.0F, 1.0F);
        
        // Load Lua script for the rectangle
        rectangle->LoadLuaScript("Common/Scripts/simple_rectangle.lua");
        
        // Create first triangle (верх-право, дальше от центра)
        auto triangle1 = std::make_shared<mentalsdk::CMentalObject>();
        triangle1->InitializeTriangle();
        triangle1->SetMaterial(glowShader);
        triangle1->SetPosition(2.0F, 1.5F, 0.0F);   // Дальше вправо и выше
        triangle1->SetScale(0.8F, 0.8F, 1.0F);
        triangle1->SetColor(1.0F, 0.8F, 0.2F, 1.0F); // Золотистый
        
        // Load Lua script for the first triangle
        triangle1->LoadLuaScript("Common/Scripts/glowing_triangle.lua");
        
        // Create second triangle (низ-лево, дальше от центра)
        auto triangle2 = std::make_shared<mentalsdk::CMentalObject>();
        triangle2->InitializeTriangle();
        triangle2->SetMaterial(glowShader);
        triangle2->SetPosition(-2.0F, -1.5F, 0.0F);  // Дальше влево и ниже
        triangle2->SetScale(0.7F, 0.7F, 1.0F);
        triangle2->SetColor(0.2F, 0.8F, 1.0F, 1.0F); // Голубоватый
        
        // Load Lua script for the second triangle
        triangle2->LoadLuaScript("Common/Scripts/glowing_triangle_bottom.lua");
        
        // Create third triangle (право, дальше от центра)
        auto triangle3 = std::make_shared<mentalsdk::CMentalObject>();
        triangle3->InitializeTriangle();
        triangle3->SetMaterial(glowShader);
        triangle3->SetPosition(2.5F, -0.5F, 0.0F);   // Дальше вправо
        triangle3->SetScale(0.9F, 0.9F, 1.0F);
        triangle3->SetColor(1.0F, 0.2F, 0.8F, 1.0F); // Розоватый
        
        // Load Lua script for the third triangle (reuse the first one)
        triangle3->LoadLuaScript("Common/Scripts/glowing_triangle.lua");
        
        // Create camera with mouse control
        auto camera = std::make_shared<mentalsdk::CMentalCamera>();
        camera->LoadLuaScript("Common/Scripts/camera_mouse.lua");
        
        // Add objects to render pipeline
        auto* renderer = windowManager.GetRenderer();
        if (renderer) {
            auto* pipeline = renderer->GetRenderPipeline();
            if (pipeline) {
                pipeline->AddObject(rectangle);
                pipeline->AddObject(triangle1);
                pipeline->AddObject(triangle2);
                pipeline->AddObject(triangle3);
                pipeline->SetCamera(camera);
                std::cout << "Rectangle and 3 glowing triangles added to render pipeline" << std::endl;
            }
        }
        
        // Main loop
        std::cout << "Starting main loop with multiple glowing triangles..." << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  WASD - Move camera" << std::endl;
        std::cout << "  Left mouse button + drag - Rotate camera" << std::endl;
        std::cout << "  Watch the glowing triangles around the rectangle!" << std::endl;
        
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        while (!windowManager.ShouldClose()) {
            // Calculate delta time
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;
            
            // Clear screen with darker background to show glow effect better
            windowManager.Clear(0.02F, 0.02F, 0.08F, 1.0F);
            
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