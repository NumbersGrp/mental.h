#pragma once
#include <map>
#include <memory>
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
        
        // Then render all objects
        for (const auto& [name, object]: *hierarchy_) {
            if (object) { object->render(); }
        }
    }
};

} // mentalsdk
