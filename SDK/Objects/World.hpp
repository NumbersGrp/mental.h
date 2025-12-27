#pragma once
#include <map>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Objects/Object.hpp"
#include "Environment.hpp"

namespace mentalsdk
{


class CMentalWorld
{
private:
    std::shared_ptr<std::map<std::string, std::shared_ptr<CMentalObject>>> hierarchy_ = std::make_shared<std::map<std::string, std::shared_ptr<CMentalObject>>>();
    std::shared_ptr<CMentalEnvironment> environment_ = nullptr;
public:
    CMentalWorld() = default;
    ~CMentalWorld() = default;

    CMentalWorld(const CMentalWorld&) = delete;
    CMentalWorld& operator=(const CMentalWorld&) = delete;
    CMentalWorld(CMentalWorld&&) = delete;
    CMentalWorld& operator=(CMentalWorld&&) = delete;


    [[nodiscard]] std::shared_ptr<std::map<std::string, std::shared_ptr<CMentalObject>>> getHierarchy() const { return this->hierarchy_; }
    void setNode(const std::string& name, const std::shared_ptr<CMentalObject>& object) {  if (hierarchy_) { (*hierarchy_)[name] = object; } }
    std::shared_ptr<CMentalObject> getNode(const std::string& name) { return hierarchy_ ? (*hierarchy_)[name] : nullptr; }
    
    void setEnvironment(const std::shared_ptr<CMentalEnvironment>& environment) { environment_ = environment; }
    std::shared_ptr<CMentalEnvironment> getEnvironment() const { return environment_; }
    
    void render() {
        // Render environment first (clear color, etc.)
        if (environment_) {
            environment_->renderClearColor();
        }
        
        // Set up basic matrices (you'll want to make these configurable later)
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 3.0f),   // Camera position
            glm::vec3(0.0f, 0.0f, 0.0f),   // Look at origin
            glm::vec3(0.0f, 1.0f, 0.0f)    // Up vector
        );
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),           // FOV
            800.0f / 600.0f,               // Aspect ratio (you'll want to get this from window)
            0.1f,                          // Near plane
            100.0f                         // Far plane
        );
        
        // Render all objects with proper matrices
        for (const auto& [name, object]: *hierarchy_) {
            if (object) { 
                object->render(model, view, projection);
            }
        }
    }
};

} // mentalsdk
