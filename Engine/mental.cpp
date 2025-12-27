#include "../SDK/SDK.hpp"
#include <iostream>
#include <memory>


int main() {
    std::cout << "Hello, World!\n";
    
    try {
        mentalsdk::CMentalWindow<mentalsdk::CMentalRenderer> window;
        
        auto renderer = std::make_shared<mentalsdk::CMentalRenderer>();
        auto world = std::make_shared<mentalsdk::CMentalWorld>();
        
        auto environment = std::make_shared<mentalsdk::CMentalEnvironment>();
        environment->setColor(0.3F, 0.2F, 0.7F, 1.0F);
        
        // Create a triangle using the new factory method
        auto triangle = mentalsdk::CMentalObject::createTriangle("MyTriangle");
        triangle->connectShader("common/Shaders/default_vertex.glsl", "common/Shaders/default_fragment.glsl");
        triangle->connectScript("common/Scripts/rotate_script.lua");
        
        auto camera = std::make_shared<mentalsdk::CMentalObject>("Camera", mentalsdk::CMentalObjectType::Camera);
        auto moonObject = std::make_shared<mentalsdk::CMentalObject>("Moon", mentalsdk::CMentalObjectType::ObjModel);
        
        world->setEnvironment(environment);
        world->setNode("Triangle", triangle);
        // world->setNode("Camera", camera);  // Camera doesn't need rendering yet

        renderer->addCommandToPool([&world]() { world->render(); });
        window.setRenderPool(renderer);
        window.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return -1;
    }
    
    return 0;
}