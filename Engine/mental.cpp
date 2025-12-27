#include "../SDK/Objects/Object.hpp"
#include "../SDK/Objects/World.hpp"
#include "../SDK/Objects/Environment.hpp"
#include "../SDK/Window/Window.hpp"
#include "../SDK/Renderer/Renderer.hpp"
#include <iostream>
#include <memory>


int main() {
    std::cout << "Hello, World!\n";
    
    try {
        mentalsdk::CMentalWindow<mentalsdk::CMentalRenderer> window;
        
        auto renderer = std::make_shared<mentalsdk::CMentalRenderer>();
        auto world = std::make_shared<mentalsdk::CMentalWorld>();
        
        auto environment = std::make_shared<mentalsdk::CMentalEnvironment>();
        environment->setColor(0.3F, 0.2F, 0.7F, 1.0F );
        
        auto camera = std::make_shared<mentalsdk::CMentalObject>("Camera", mentalsdk::CMentalObjectType::Camera);
        auto moonObject = std::make_shared<mentalsdk::CMentalObject>("Moon", mentalsdk::CMentalObjectType::ObjModel);
        
        world->setEnvironment(environment);
        world->setNode("Camera", camera);
        world->setNode("Moon Object", moonObject);

        renderer->addCommandToPool([&world]() { world->render(); });
        window.setRenderPool(renderer);
        window.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return -1;
    }
    
    return 0;
}