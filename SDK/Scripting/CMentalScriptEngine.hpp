/**
 * @file CMentalScriptEngine.hpp
 * @brief Simple script interface for the Mental SDK
 * @author Mental SDK Team
 * @version 1.0.0
 */

#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <glm/glm.hpp>

namespace mentalsdk
{

/**
 * @struct ScriptData
 * @brief Data structure for script communication
 */
struct ScriptData {
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};
    glm::vec3 color_tint{1.0f};
    float time = 0.0f;
};

/**
 * @class CMentalScriptEngine
 * @brief Simple script engine for basic scripting support
 * 
 * This is a simplified script engine that simulates Lua-like behavior
 * without requiring external dependencies.
 */
class CMentalScriptEngine
{
private:
    std::unordered_map<std::string, std::function<ScriptData(float)>> scripts_;
    float total_time_ = 0.0f;

public:
    /**
     * @brief Constructor
     */
    CMentalScriptEngine() = default;
    
    /**
     * @brief Destructor
     */
    ~CMentalScriptEngine() = default;

    /**
     * @brief Load a script (simplified version)
     * @param name Script name
     * @param scriptPath Path to script file
     * @return true if loaded successfully
     */
    bool loadScript(const std::string& name, const std::string& scriptPath);
    
    /**
     * @brief Execute a script
     * @param name Script name
     * @param deltaTime Time since last update
     * @return Script data
     */
    ScriptData executeScript(const std::string& name, float deltaTime);
    
    /**
     * @brief Check if script exists
     * @param name Script name
     * @return true if script exists
     */
    bool hasScript(const std::string& name) const;

private:
    /**
     * @brief Create triangle controller script (hardcoded for simplicity)
     * @return Script function
     */
    std::function<ScriptData(float)> createTriangleController();
};

// Implementation
inline bool CMentalScriptEngine::loadScript(const std::string& name, const std::string& scriptPath) {
    // For simplicity, we'll hardcode the triangle controller
    // In a real implementation, you'd parse the Lua file
    if (name == "triangle_controller") {
        scripts_[name] = createTriangleController();
        std::cout << "Loaded script: " << name << " from " << scriptPath << std::endl;
        return true;
    }
    
    std::cout << "Unknown script: " << name << std::endl;
    return false;
}

inline ScriptData CMentalScriptEngine::executeScript(const std::string& name, float deltaTime) {
    total_time_ += deltaTime;
    
    auto it = scripts_.find(name);
    if (it != scripts_.end()) {
        return it->second(deltaTime);
    }
    
    return ScriptData{}; // Return default data
}

inline bool CMentalScriptEngine::hasScript(const std::string& name) const {
    return scripts_.find(name) != scripts_.end();
}

inline std::function<ScriptData(float)> CMentalScriptEngine::createTriangleController() {
    return [this](float deltaTime) -> ScriptData {
        ScriptData data;
        
        // Rotate the triangle
        data.rotation.y = total_time_ * 1.0f; // rotation_speed = 1.0
        
        // Pulse the scale
        float pulse = 1.0f + 0.2f * std::sin(total_time_ * 2.0f); // pulse_speed = 2.0
        data.scale = glm::vec3(pulse);
        
        // Cycle through colors
        data.color_tint.r = 0.5f + 0.5f * std::sin(total_time_ * 0.5f);
        data.color_tint.g = 0.5f + 0.5f * std::sin(total_time_ * 0.7f);
        data.color_tint.b = 0.5f + 0.5f * std::sin(total_time_ * 0.9f);
        
        data.time = total_time_;
        
        return data;
    };
}

} // namespace mentalsdk