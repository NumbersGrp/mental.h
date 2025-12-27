#pragma once
#include <string>
#include <utility>
#include <iostream>

extern "C" {
    #include <lua/lua.h>
    #include <lua/lauxlib.h>
    #include <lua/lualib.h>
}

namespace mentalsdk
{

class CMentalScript
{
private:
    bool scriptExists_ = false;
    std::string scriptFile_;
    lua_State* L_ = nullptr;
public:
    explicit CMentalScript(std::string scriptFile) : scriptFile_(std::move(scriptFile)) {
        // Initialize Lua state
        L_ = luaL_newstate();
        luaL_openlibs(L_);
        this->loadScript(this->scriptFile_);
        this->registerLuaFunctions();
    }
    
    ~CMentalScript() {
        if (L_) {
            lua_close(L_);
        }
    }

    CMentalScript(const CMentalScript&) = delete;
    CMentalScript& operator=(const CMentalScript&) = delete;
    CMentalScript(CMentalScript&&) = delete;
    CMentalScript& operator=(CMentalScript&&) = delete;

    void loadScript(const std::string& scriptFile) {
        if (!L_) {
            std::cerr << "Lua state not initialized\n";
            return;
        }
        
        int result = luaL_loadfile(L_, scriptFile.c_str());
        if (result != LUA_OK) {
            std::cerr << "Error loading Lua script: " << lua_tostring(L_, -1) << "\n";
            lua_pop(L_, 1);
            scriptExists_ = false;
            return;
        }
        
        // Execute the script once to define functions
        result = lua_pcall(L_, 0, 0, 0);
        if (result != LUA_OK) {
            std::cerr << "Error executing Lua script: " << lua_tostring(L_, -1) << "\n";
            lua_pop(L_, 1);
            scriptExists_ = false;
            return;
        }
        
        scriptExists_ = true;
        std::cout << "Lua script loaded and executed: " << scriptFile << "\n";
    }
    
    void executeScript() {
        if (!L_ || !scriptExists_) {
            std::cerr << "No valid Lua script to execute\n";
            return;
        }
        
        int result = lua_pcall(L_, 0, 0, 0);
        if (result != LUA_OK) {
            std::cerr << "Error executing Lua script: " << lua_tostring(L_, -1) << "\n";
            lua_pop(L_, 1);
        }
    }

    void registerLuaFunctions() {
        // Register C++ functions that can be called from Lua if needed
        // For now, we'll just call Lua functions from C++
    }

    void callInit() {
        if (!L_ || !scriptExists_) {
            return;
        }
        
        lua_getglobal(L_, "init");
        if (lua_isfunction(L_, -1)) {
            int result = lua_pcall(L_, 0, 0, 0);
            if (result != LUA_OK) {
                std::cerr << "Error calling init function: " << lua_tostring(L_, -1) << "\n";
                lua_pop(L_, 1);
            }
        } else {
            lua_pop(L_, 1); // Remove non-function from stack
        }
    }
    
    void callUpdate() {
        if (!L_ || !scriptExists_) {
            return;
        }
        
        lua_getglobal(L_, "update");
        if (lua_isfunction(L_, -1)) {
            int result = lua_pcall(L_, 0, 0, 0);
            if (result != LUA_OK) {
                std::cerr << "Error calling update function: " << lua_tostring(L_, -1) << "\n";
                lua_pop(L_, 1);
            }
        } else {
            lua_pop(L_, 1); // Remove non-function from stack
        }
    }
    
    // Advanced version with parameters
    void callUpdateWithDeltaTime(float deltaTime) {
        if (!L_ || !scriptExists_) {
            return;
        }
        
        lua_getglobal(L_, "update");
        if (lua_isfunction(L_, -1)) {
            lua_pushnumber(L_, deltaTime); // Push deltaTime as parameter
            int result = lua_pcall(L_, 1, 0, 0); // 1 parameter, 0 return values
            if (result != LUA_OK) {
                std::cerr << "Error calling update function with deltaTime: " << lua_tostring(L_, -1) << "\n";
                lua_pop(L_, 1);
            }
        } else {
            lua_pop(L_, 1); // Remove non-function from stack
        }
    }
    
    // Get rotation value from Lua script
    float getRotationFromScript() {
        if (!L_ || !scriptExists_) {
            return 0.0f;
        }
        
        lua_getglobal(L_, "getRotation");
        if (lua_isfunction(L_, -1)) {
            int result = lua_pcall(L_, 0, 1, 0); // 0 parameters, 1 return value
            if (result == LUA_OK) {
                if (lua_isnumber(L_, -1)) {
                    float rotation = static_cast<float>(lua_tonumber(L_, -1));
                    lua_pop(L_, 1); // Remove return value from stack
                    return rotation;
                }
                lua_pop(L_, 1); // Remove non-number from stack
            } else {
                std::cerr << "Error calling getRotation function: " << lua_tostring(L_, -1) << "\n";
                lua_pop(L_, 1);
            }
        } else {
            lua_pop(L_, 1); // Remove non-function from stack
        }
        
        return 0.0f;
    }
    
    // Get position from Lua script
    glm::vec3 getPositionFromScript() {
        if (!L_ || !scriptExists_) {
            return glm::vec3(0.0f);
        }
        
        lua_getglobal(L_, "getPosition");
        if (lua_isfunction(L_, -1)) {
            int result = lua_pcall(L_, 0, 3, 0); // 0 parameters, 3 return values (x, y, z)
            if (result == LUA_OK) {
                glm::vec3 position(0.0f);
                
                // Get z (top of stack)
                if (lua_isnumber(L_, -1)) {
                    position.z = static_cast<float>(lua_tonumber(L_, -1));
                }
                lua_pop(L_, 1);
                
                // Get y
                if (lua_isnumber(L_, -1)) {
                    position.y = static_cast<float>(lua_tonumber(L_, -1));
                }
                lua_pop(L_, 1);
                
                // Get x
                if (lua_isnumber(L_, -1)) {
                    position.x = static_cast<float>(lua_tonumber(L_, -1));
                }
                lua_pop(L_, 1);
                
                return position;
            } else {
                std::cerr << "Error calling getPosition function: " << lua_tostring(L_, -1) << "\n";
                lua_pop(L_, 1);
            }
        } else {
            lua_pop(L_, 1); // Remove non-function from stack
        }
        
        return glm::vec3(0.0f);
    }
    
    // Get scale from Lua script
    glm::vec3 getScaleFromScript() {
        if (!L_ || !scriptExists_) {
            return glm::vec3(1.0f);
        }
        
        lua_getglobal(L_, "getScale");
        if (lua_isfunction(L_, -1)) {
            int result = lua_pcall(L_, 0, 3, 0); // 0 parameters, 3 return values (x, y, z)
            if (result == LUA_OK) {
                glm::vec3 scale(1.0f);
                
                // Get z (top of stack)
                if (lua_isnumber(L_, -1)) {
                    scale.z = static_cast<float>(lua_tonumber(L_, -1));
                }
                lua_pop(L_, 1);
                
                // Get y
                if (lua_isnumber(L_, -1)) {
                    scale.y = static_cast<float>(lua_tonumber(L_, -1));
                }
                lua_pop(L_, 1);
                
                // Get x
                if (lua_isnumber(L_, -1)) {
                    scale.x = static_cast<float>(lua_tonumber(L_, -1));
                }
                lua_pop(L_, 1);
                
                return scale;
            } else {
                std::cerr << "Error calling getScale function: " << lua_tostring(L_, -1) << "\n";
                lua_pop(L_, 1);
            }
        } else {
            lua_pop(L_, 1); // Remove non-function from stack
        }
        
        return glm::vec3(1.0f);
    }
    
    // Set object position to Lua script
    void setObjectTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
        if (!L_ || !scriptExists_) {
            return;
        }
        
        // Set position
        lua_newtable(L_);
        lua_pushnumber(L_, position.x); lua_setfield(L_, -2, "x");
        lua_pushnumber(L_, position.y); lua_setfield(L_, -2, "y");
        lua_pushnumber(L_, position.z); lua_setfield(L_, -2, "z");
        lua_setglobal(L_, "position");
        
        // Set rotation
        lua_newtable(L_);
        lua_pushnumber(L_, rotation.x); lua_setfield(L_, -2, "x");
        lua_pushnumber(L_, rotation.y); lua_setfield(L_, -2, "y");
        lua_pushnumber(L_, rotation.z); lua_setfield(L_, -2, "z");
        lua_setglobal(L_, "rotation");
        
        // Set scale
        lua_newtable(L_);
        lua_pushnumber(L_, scale.x); lua_setfield(L_, -2, "x");
        lua_pushnumber(L_, scale.y); lua_setfield(L_, -2, "y");
        lua_pushnumber(L_, scale.z); lua_setfield(L_, -2, "z");
        lua_setglobal(L_, "scale");
    }

    [[nodiscard]] bool hasScript() const { return this->scriptExists_; }
};

} // mentalsdk
