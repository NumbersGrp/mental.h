#pragma once
#include <memory>
#include <vector>
#include <functional>
#include <GL/glew.h>
#include <iostream>

namespace mentalsdk
{
using RenderCommand = std::function<void()>;

class CMentalRenderer
{
private:
    std::shared_ptr<std::vector<RenderCommand>> command_pool_;
public:
    CMentalRenderer() : command_pool_(std::make_shared<std::vector<RenderCommand>>()) {
        this->initializeGL();
    }
    
    static bool initializeGL() {
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) { 
            std::cerr << "Failed to initialize GLEW library.\n";
            return false;
        }
        return true;
    }
    ~CMentalRenderer() = default;

    CMentalRenderer(const CMentalRenderer&) = delete;
    CMentalRenderer& operator=(const CMentalRenderer&) = delete;
    CMentalRenderer(CMentalRenderer&&) = delete;
    CMentalRenderer& operator=(CMentalRenderer&&) = delete;

    std::shared_ptr<std::vector<RenderCommand>> getCommandPool() { return command_pool_; }
    
    void addCommandToPool(const RenderCommand& command) { 
        if (command_pool_) {
            command_pool_->push_back(command); 
        }
    }
    
    void addCommandToPool(RenderCommand&& command) { 
        if (command_pool_) {
            command_pool_->emplace_back(std::move(command)); 
        }
    }
    
    void executeCommands() {
        if (command_pool_) {
            for (const auto& command : *command_pool_) {
                if (command) {
                    command();
                }
            }
        }
    }
    
    void clearCommandPool() {
        if (command_pool_) {
            command_pool_->clear();
        }
    }
    
    [[nodiscard]] size_t getCommandPoolSize() const {
        return command_pool_ ? command_pool_->size() : 0;
    }

    void render() {
        for (const RenderCommand& command : *this->command_pool_) {
            command();
        }
    }
};

} // namespace mentalsdk
