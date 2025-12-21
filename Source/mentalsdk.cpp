#include "mentalsdk.hpp"
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <ctime>
#include <sys/stat.h>

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

namespace mentalsdk {

// ============================================================================
// Global GLFW window pointer for input handling
// ============================================================================

GLFWwindow* g_current_window = nullptr;

// Mouse tracking variables
static double g_last_mouse_x = 0.0;
static double g_last_mouse_y = 0.0;
static double g_mouse_delta_x = 0.0;
static double g_mouse_delta_y = 0.0;
static bool g_first_mouse = true;

// ============================================================================
// Global time tracking for Lua scripts
// ============================================================================

static auto g_start_time = std::chrono::high_resolution_clock::now();

static float GetCurrentTime() {
    auto current_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - g_start_time);
    return duration.count() / 1000.0f;
}

static void UpdateMousePosition() {
    if (!g_current_window) return;
    
    double current_x, current_y;
    glfwGetCursorPos(g_current_window, &current_x, &current_y);
    
    if (g_first_mouse) {
        g_last_mouse_x = current_x;
        g_last_mouse_y = current_y;
        g_first_mouse = false;
    }
    
    g_mouse_delta_x = current_x - g_last_mouse_x;
    g_mouse_delta_y = g_last_mouse_y - current_y; // Инвертируем Y для правильного направления
    
    g_last_mouse_x = current_x;
    g_last_mouse_y = current_y;
}

// ============================================================================
// CMentalLuaScript Implementation
// ============================================================================

CMentalLuaScript::CMentalLuaScript() 
    : lua_state_(nullptr), is_loaded_(false), bound_object_(nullptr), bound_camera_(nullptr) {
    std::cout << "CMentalLuaScript: Creating Lua script..." << std::endl;
    InitializeLuaState();
}

CMentalLuaScript::~CMentalLuaScript() {
    CleanupLuaState();
}

void CMentalLuaScript::InitializeLuaState() {
    lua_state_ = luaL_newstate();
    if (!lua_state_) {
        throw std::runtime_error("Failed to create Lua state");
    }
    
    // Load standard Lua libraries
    luaL_openlibs(lua_state_);
    
    // Register our API
    RegisterLuaAPI();
    
    std::cout << "CMentalLuaScript: Lua state initialized" << std::endl;
}

void CMentalLuaScript::RegisterLuaAPI() {
    // Store pointer to this script in Lua registry
    lua_pushlightuserdata(lua_state_, this);
    lua_setfield(lua_state_, LUA_REGISTRYINDEX, "script_instance");
    
    // Register object manipulation functions
    lua_register(lua_state_, "setPosition", lua_setPosition);
    lua_register(lua_state_, "getPosition", lua_getPosition);
    lua_register(lua_state_, "setRotation", lua_setRotation);
    lua_register(lua_state_, "getRotation", lua_getRotation);
    lua_register(lua_state_, "setScale", lua_setScale);
    lua_register(lua_state_, "getScale", lua_getScale);
    lua_register(lua_state_, "setColor", lua_setColor);
    lua_register(lua_state_, "getColor", lua_getColor);
    lua_register(lua_state_, "setVisible", lua_setVisible);
    lua_register(lua_state_, "isVisible", lua_isVisible);
    
    // Register camera functions
    lua_register(lua_state_, "setCameraPosition", lua_setCameraPosition);
    lua_register(lua_state_, "getCameraPosition", lua_getCameraPosition);
    lua_register(lua_state_, "setCameraTarget", lua_setCameraTarget);
    lua_register(lua_state_, "getCameraTarget", lua_getCameraTarget);
    lua_register(lua_state_, "setCameraUp", lua_setCameraUp);
    lua_register(lua_state_, "getCameraUp", lua_getCameraUp);
    lua_register(lua_state_, "lookAt", lua_lookAt);
    lua_register(lua_state_, "setPerspective", lua_setPerspective);
    lua_register(lua_state_, "setFOV", lua_setFOV);
    lua_register(lua_state_, "moveForward", lua_moveForward);
    lua_register(lua_state_, "moveBackward", lua_moveBackward);
    lua_register(lua_state_, "moveLeft", lua_moveLeft);
    lua_register(lua_state_, "moveRight", lua_moveRight);
    lua_register(lua_state_, "moveUp", lua_moveUp);
    lua_register(lua_state_, "moveDown", lua_moveDown);
    lua_register(lua_state_, "rotateYaw", lua_rotateYaw);
    lua_register(lua_state_, "rotatePitch", lua_rotatePitch);
    lua_register(lua_state_, "rotateRoll", lua_rotateRoll);
    
    // Register utility functions
    lua_register(lua_state_, "getTime", lua_getTime);
    lua_register(lua_state_, "sin", lua_sin);
    lua_register(lua_state_, "cos", lua_cos);
    lua_register(lua_state_, "print", lua_print);
    
    // Register input functions
    lua_register(lua_state_, "isKeyPressed", lua_isKeyPressed);
    lua_register(lua_state_, "KEY_UP", lua_KEY_UP);
    lua_register(lua_state_, "KEY_DOWN", lua_KEY_DOWN);
    lua_register(lua_state_, "KEY_LEFT", lua_KEY_LEFT);
    lua_register(lua_state_, "KEY_RIGHT", lua_KEY_RIGHT);
    lua_register(lua_state_, "KEY_W", lua_KEY_W);
    lua_register(lua_state_, "KEY_A", lua_KEY_A);
    lua_register(lua_state_, "KEY_S", lua_KEY_S);
    lua_register(lua_state_, "KEY_D", lua_KEY_D);
    
    // Register mouse functions
    lua_register(lua_state_, "getMousePosition", lua_getMousePosition);
    lua_register(lua_state_, "getMouseDelta", lua_getMouseDelta);
    lua_register(lua_state_, "isMouseButtonPressed", lua_isMouseButtonPressed);
    lua_register(lua_state_, "MOUSE_LEFT", lua_MOUSE_LEFT);
    lua_register(lua_state_, "MOUSE_RIGHT", lua_MOUSE_RIGHT);
    // Register root node functions
    lua_register(lua_state_, "createObject", lua_createObject);
    lua_register(lua_state_, "createShader", lua_createShader);
    lua_register(lua_state_, "attachShaderToObject", lua_attachShaderToObject);
    lua_register(lua_state_, "attachScriptToObject", lua_attachScriptToObject);
    lua_register(lua_state_, "getObjectByName", lua_getObjectByName);
    lua_register(lua_state_, "removeObject", lua_removeObject);
    lua_register(lua_state_, "listObjects", lua_listObjects);
    lua_register(lua_state_, "reloadScript", lua_reloadScript);
    lua_register(lua_state_, "clearScene", lua_clearScene);
    
    // Register object manipulation functions
    lua_register(lua_state_, "setObjectPosition", lua_setObjectPosition);
    lua_register(lua_state_, "getObjectPosition", lua_getObjectPosition);
    lua_register(lua_state_, "setObjectRotation", lua_setObjectRotation);
    lua_register(lua_state_, "getObjectRotation", lua_getObjectRotation);
    lua_register(lua_state_, "setObjectScale", lua_setObjectScale);
    lua_register(lua_state_, "getObjectScale", lua_getObjectScale);
    lua_register(lua_state_, "setObjectColor", lua_setObjectColor);
    lua_register(lua_state_, "getObjectColor", lua_getObjectColor);
    lua_register(lua_state_, "setObjectVisible", lua_setObjectVisible);
    lua_register(lua_state_, "isObjectVisible", lua_isObjectVisible);
}

void CMentalLuaScript::CleanupLuaState() {
    if (lua_state_) {
        lua_close(lua_state_);
        lua_state_ = nullptr;
    }
}

bool CMentalLuaScript::LoadFromFile(const std::string& script_path) {
    std::cout << "CMentalLuaScript: Loading script from file: " << script_path << std::endl;
    
    std::ifstream file(script_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open Lua script file: " << script_path << std::endl;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string script_content = buffer.str();
    file.close();
    
    script_path_ = script_path;
    return ExecuteScript(script_content);
}

bool CMentalLuaScript::LoadFromString(const std::string& script_content) {
    std::cout << "CMentalLuaScript: Loading script from string..." << std::endl;
    script_path_ = "<string>";
    return ExecuteScript(script_content);
}

bool CMentalLuaScript::ExecuteScript(const std::string& script_content) {
    if (!lua_state_) {
        std::cerr << "Lua state not initialized" << std::endl;
        return false;
    }
    
    int result = luaL_dostring(lua_state_, script_content.c_str());
    if (result != LUA_OK) {
        std::cerr << "Lua script error: " << lua_tostring(lua_state_, -1) << std::endl;
        lua_pop(lua_state_, 1);
        return false;
    }
    
    is_loaded_ = true;
    std::cout << "CMentalLuaScript: Script loaded successfully" << std::endl;
    return true;
}

void CMentalLuaScript::Reload() {
    if (script_path_.empty() || script_path_ == "<string>") {
        std::cerr << "Cannot reload script: no file path available" << std::endl;
        return;
    }
    
    std::cout << "CMentalLuaScript: Reloading script..." << std::endl;
    CleanupLuaState();
    InitializeLuaState();
    LoadFromFile(script_path_);
}

void CMentalLuaScript::BindToObject(CMentalObject* object) {
    bound_object_ = object;
    std::cout << "CMentalLuaScript: Bound to object" << std::endl;
}

void CMentalLuaScript::BindToCamera(CMentalCamera* camera) {
    bound_camera_ = camera;
    std::cout << "CMentalLuaScript: Bound to camera" << std::endl;
}

void CMentalLuaScript::CallInit() {
    if (!is_loaded_ || !lua_state_) return;
    
    lua_getglobal(lua_state_, "init");
    if (lua_isfunction(lua_state_, -1)) {
        int result = lua_pcall(lua_state_, 0, 0, 0);
        if (result != LUA_OK) {
            std::cerr << "Lua init() error: " << lua_tostring(lua_state_, -1) << std::endl;
            lua_pop(lua_state_, 1);
        }
    } else {
        lua_pop(lua_state_, 1);
    }
}

void CMentalLuaScript::CallUpdate(float delta_time) {
    if (!is_loaded_ || !lua_state_) return;
    
    lua_getglobal(lua_state_, "update");
    if (lua_isfunction(lua_state_, -1)) {
        lua_pushnumber(lua_state_, delta_time);
        int result = lua_pcall(lua_state_, 1, 0, 0);
        if (result != LUA_OK) {
            std::cerr << "Lua update() error: " << lua_tostring(lua_state_, -1) << std::endl;
            lua_pop(lua_state_, 1);
        }
    } else {
        lua_pop(lua_state_, 1);
    }
}

void CMentalLuaScript::CallRender() {
    if (!is_loaded_ || !lua_state_) return;
    
    lua_getglobal(lua_state_, "render");
    if (lua_isfunction(lua_state_, -1)) {
        int result = lua_pcall(lua_state_, 0, 0, 0);
        if (result != LUA_OK) {
            std::cerr << "Lua render() error: " << lua_tostring(lua_state_, -1) << std::endl;
            lua_pop(lua_state_, 1);
        }
    } else {
        lua_pop(lua_state_, 1);
    }
}

bool CMentalLuaScript::CallFunction(const std::string& function_name) {
    if (!is_loaded_ || !lua_state_) return false;
    
    lua_getglobal(lua_state_, function_name.c_str());
    if (lua_isfunction(lua_state_, -1)) {
        int result = lua_pcall(lua_state_, 0, 0, 0);
        if (result != LUA_OK) {
            std::cerr << "Lua " << function_name << "() error: " << lua_tostring(lua_state_, -1) << std::endl;
            lua_pop(lua_state_, 1);
            return false;
        }
        return true;
    } else {
        lua_pop(lua_state_, 1);
        return false;
    }
}

// ============================================================================
// Lua API Implementation
// ============================================================================

CMentalLuaScript* GetScriptFromLua(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, "script_instance");
    CMentalLuaScript* script = static_cast<CMentalLuaScript*>(lua_touserdata(L, -1));
    lua_pop(L, 1);
    return script;
}

int CMentalLuaScript::lua_setPosition(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_object_) return 0;
    
    float x = static_cast<float>(luaL_checknumber(L, 1));
    float y = static_cast<float>(luaL_checknumber(L, 2));
    float z = static_cast<float>(luaL_optnumber(L, 3, 0.0));
    
    script->bound_object_->SetPosition(x, y, z);
    return 0;
}

int CMentalLuaScript::lua_getPosition(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_object_) return 0;
    
    const auto& pos = script->bound_object_->GetPosition();
    lua_pushnumber(L, pos[0]);
    lua_pushnumber(L, pos[1]);
    lua_pushnumber(L, pos[2]);
    return 3;
}

int CMentalLuaScript::lua_setRotation(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_object_) return 0;
    
    float x = static_cast<float>(luaL_checknumber(L, 1));
    float y = static_cast<float>(luaL_checknumber(L, 2));
    float z = static_cast<float>(luaL_checknumber(L, 3));
    
    script->bound_object_->SetRotation(x, y, z);
    return 0;
}

int CMentalLuaScript::lua_getRotation(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_object_) return 0;
    
    const auto& rot = script->bound_object_->GetRotation();
    lua_pushnumber(L, rot[0]);
    lua_pushnumber(L, rot[1]);
    lua_pushnumber(L, rot[2]);
    return 3;
}

int CMentalLuaScript::lua_setScale(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_object_) return 0;
    
    float x = static_cast<float>(luaL_checknumber(L, 1));
    float y = static_cast<float>(luaL_checknumber(L, 2));
    float z = static_cast<float>(luaL_optnumber(L, 3, 1.0));
    
    script->bound_object_->SetScale(x, y, z);
    return 0;
}

int CMentalLuaScript::lua_getScale(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_object_) return 0;
    
    const auto& scale = script->bound_object_->GetScale();
    lua_pushnumber(L, scale[0]);
    lua_pushnumber(L, scale[1]);
    lua_pushnumber(L, scale[2]);
    return 3;
}

int CMentalLuaScript::lua_setColor(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_object_) return 0;
    
    float r = static_cast<float>(luaL_checknumber(L, 1));
    float g = static_cast<float>(luaL_checknumber(L, 2));
    float b = static_cast<float>(luaL_checknumber(L, 3));
    float a = static_cast<float>(luaL_optnumber(L, 4, 1.0));
    
    script->bound_object_->SetColor(r, g, b, a);
    return 0;
}

int CMentalLuaScript::lua_getColor(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_object_) return 0;
    
    const auto& color = script->bound_object_->GetColor();
    lua_pushnumber(L, color[0]);
    lua_pushnumber(L, color[1]);
    lua_pushnumber(L, color[2]);
    lua_pushnumber(L, color[3]);
    return 4;
}

int CMentalLuaScript::lua_setVisible(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_object_) return 0;
    
    bool visible = lua_toboolean(L, 1);
    script->bound_object_->SetVisible(visible);
    return 0;
}

int CMentalLuaScript::lua_isVisible(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_object_) return 0;
    
    lua_pushboolean(L, script->bound_object_->IsVisible());
    return 1;
}

int CMentalLuaScript::lua_getTime(lua_State* L) {
    lua_pushnumber(L, GetCurrentTime());
    return 1;
}

int CMentalLuaScript::lua_sin(lua_State* L) {
    float angle = static_cast<float>(luaL_checknumber(L, 1));
    lua_pushnumber(L, std::sin(angle));
    return 1;
}

int CMentalLuaScript::lua_cos(lua_State* L) {
    float angle = static_cast<float>(luaL_checknumber(L, 1));
    lua_pushnumber(L, std::cos(angle));
    return 1;
}

int CMentalLuaScript::lua_print(lua_State* L) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        if (i > 1) std::cout << "\t";
        if (lua_isstring(L, i)) {
            std::cout << lua_tostring(L, i);
        } else if (lua_isnumber(L, i)) {
            std::cout << lua_tonumber(L, i);
        } else if (lua_isboolean(L, i)) {
            std::cout << (lua_toboolean(L, i) ? "true" : "false");
        } else {
            std::cout << luaL_typename(L, i);
        }
    }
    std::cout << std::endl;
    return 0;
}

// Input functions - connected to GLFW
int CMentalLuaScript::lua_isKeyPressed(lua_State* L) {
    int key = static_cast<int>(luaL_checkinteger(L, 1));
    
    // Get the current GLFW window from somewhere - we need to store it globally
    // For now, we'll use a simple approach by storing the window pointer
    extern GLFWwindow* g_current_window;
    
    if (g_current_window) {
        int state = glfwGetKey(g_current_window, key);
        lua_pushboolean(L, state == GLFW_PRESS || state == GLFW_REPEAT);
    } else {
        lua_pushboolean(L, false);
    }
    return 1;
}

int CMentalLuaScript::lua_KEY_UP(lua_State* L) {
    lua_pushinteger(L, GLFW_KEY_UP);
    return 1;
}

int CMentalLuaScript::lua_KEY_DOWN(lua_State* L) {
    lua_pushinteger(L, GLFW_KEY_DOWN);
    return 1;
}

int CMentalLuaScript::lua_KEY_LEFT(lua_State* L) {
    lua_pushinteger(L, GLFW_KEY_LEFT);
    return 1;
}

int CMentalLuaScript::lua_KEY_RIGHT(lua_State* L) {
    lua_pushinteger(L, GLFW_KEY_RIGHT);
    return 1;
}

int CMentalLuaScript::lua_KEY_W(lua_State* L) {
    lua_pushinteger(L, GLFW_KEY_W);
    return 1;
}

int CMentalLuaScript::lua_KEY_A(lua_State* L) {
    lua_pushinteger(L, GLFW_KEY_A);
    return 1;
}

int CMentalLuaScript::lua_KEY_S(lua_State* L) {
    lua_pushinteger(L, GLFW_KEY_S);
    return 1;
}

int CMentalLuaScript::lua_KEY_D(lua_State* L) {
    lua_pushinteger(L, GLFW_KEY_D);
    return 1;
}

// ============================================================================
// Mouse Lua API Implementation
// ============================================================================

int CMentalLuaScript::lua_getMousePosition(lua_State* L) {
    if (!g_current_window) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 2;
    }
    
    double x, y;
    glfwGetCursorPos(g_current_window, &x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}

int CMentalLuaScript::lua_getMouseDelta(lua_State* L) {
    lua_pushnumber(L, g_mouse_delta_x);
    lua_pushnumber(L, g_mouse_delta_y);
    return 2;
}

int CMentalLuaScript::lua_isMouseButtonPressed(lua_State* L) {
    if (!g_current_window) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    int button = static_cast<int>(luaL_checkinteger(L, 1));
    int state = glfwGetMouseButton(g_current_window, button);
    lua_pushboolean(L, state == GLFW_PRESS);
    return 1;
}

int CMentalLuaScript::lua_MOUSE_LEFT(lua_State* L) {
    lua_pushinteger(L, GLFW_MOUSE_BUTTON_LEFT);
    return 1;
}

int CMentalLuaScript::lua_MOUSE_RIGHT(lua_State* L) {
    lua_pushinteger(L, GLFW_MOUSE_BUTTON_RIGHT);
    return 1;
}

int CMentalLuaScript::lua_MOUSE_MIDDLE(lua_State* L) {
    lua_pushinteger(L, GLFW_MOUSE_BUTTON_MIDDLE);
    return 1;
}

// ============================================================================
// Root Node Lua API Implementation
// ============================================================================

CMentalRootNode* GetRootNodeFromLua(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, "root_node_instance");
    CMentalRootNode* root_node = static_cast<CMentalRootNode*>(lua_touserdata(L, -1));
    lua_pop(L, 1);
    return root_node;
}

int CMentalLuaScript::lua_createObject(lua_State* L) {
    CMentalRootNode* root_node = GetRootNodeFromLua(L);
    if (!root_node) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    const char* name = luaL_checkstring(L, 1);
    const char* type = luaL_checkstring(L, 2);
    
    auto object = root_node->CreateObject(std::string(name), std::string(type));
    lua_pushboolean(L, object != nullptr);
    return 1;
}

int CMentalLuaScript::lua_createShader(lua_State* L) {
    CMentalRootNode* root_node = GetRootNodeFromLua(L);
    if (!root_node) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    const char* name = luaL_checkstring(L, 1);
    const char* vertex_path = luaL_checkstring(L, 2);
    const char* fragment_path = luaL_checkstring(L, 3);
    
    auto shader = root_node->CreateShader(std::string(name), std::string(vertex_path), std::string(fragment_path));
    lua_pushboolean(L, shader != nullptr);
    return 1;
}

int CMentalLuaScript::lua_attachShaderToObject(lua_State* L) {
    CMentalRootNode* root_node = GetRootNodeFromLua(L);
    if (!root_node) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    const char* object_name = luaL_checkstring(L, 1);
    const char* shader_name = luaL_checkstring(L, 2);
    
    bool success = root_node->AttachShaderToObject(std::string(object_name), std::string(shader_name));
    lua_pushboolean(L, success);
    return 1;
}

int CMentalLuaScript::lua_attachScriptToObject(lua_State* L) {
    CMentalRootNode* root_node = GetRootNodeFromLua(L);
    if (!root_node) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    const char* object_name = luaL_checkstring(L, 1);
    const char* script_path = luaL_checkstring(L, 2);
    
    bool success = root_node->AttachScriptToObject(std::string(object_name), std::string(script_path));
    lua_pushboolean(L, success);
    return 1;
}

int CMentalLuaScript::lua_getObjectByName(lua_State* L) {
    CMentalRootNode* root_node = GetRootNodeFromLua(L);
    if (!root_node) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    const char* name = luaL_checkstring(L, 1);
    auto object = root_node->GetObjectByName(std::string(name));
    lua_pushboolean(L, object != nullptr);
    return 1;
}

int CMentalLuaScript::lua_removeObject(lua_State* L) {
    CMentalRootNode* root_node = GetRootNodeFromLua(L);
    if (!root_node) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    const char* name = luaL_checkstring(L, 1);
    bool success = root_node->RemoveObject(std::string(name));
    lua_pushboolean(L, success);
    return 1;
}

int CMentalLuaScript::lua_listObjects(lua_State* L) {
    CMentalRootNode* root_node = GetRootNodeFromLua(L);
    if (!root_node) {
        return 0;
    }
    
    root_node->ListObjects();
    return 0;
}

int CMentalLuaScript::lua_reloadScript(lua_State* L) {
    CMentalRootNode* root_node = GetRootNodeFromLua(L);
    if (!root_node) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    root_node->ReloadScript();
    lua_pushboolean(L, true);
    return 1;
}

int CMentalLuaScript::lua_clearScene(lua_State* L) {
    CMentalRootNode* root_node = GetRootNodeFromLua(L);
    if (!root_node) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    root_node->ClearScene();
    lua_pushboolean(L, true);
    return 1;
}

// ============================================================================
// Object Manipulation Lua API Implementation
// ============================================================================

int CMentalLuaScript::lua_setObjectPosition(lua_State* L) {
    CMentalRootNode* root_node = GetRootNodeFromLua(L);
    if (!root_node) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    const char* object_name = luaL_checkstring(L, 1);
    float x = static_cast<float>(luaL_checknumber(L, 2));
    float y = static_cast<float>(luaL_checknumber(L, 3));
    float z = static_cast<float>(luaL_optnumber(L, 4, 0.0));
    
    auto object = root_node->GetObjectByName(std::string(object_name));
    if (object) {
        object->SetPosition(x, y, z);
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }
    return 1;
}

int CMentalLuaScript::lua_getObjectPosition(lua_State* L) {
    CMentalRootNode* root_node = GetRootNodeFromLua(L);
    if (!root_node) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 3;
    }
    
    const char* object_name = luaL_checkstring(L, 1);
    auto object = root_node->GetObjectByName(std::string(object_name));
    if (object) {
        const auto& pos = object->GetPosition();
        lua_pushnumber(L, pos[0]);
        lua_pushnumber(L, pos[1]);
        lua_pushnumber(L, pos[2]);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }
    return 3;
}

int CMentalLuaScript::lua_setObjectRotation(lua_State* L) {
    CMentalRootNode* root_node = GetRootNodeFromLua(L);
    if (!root_node) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    const char* object_name = luaL_checkstring(L, 1);
    float x = static_cast<float>(luaL_checknumber(L, 2));
    float y = static_cast<float>(luaL_checknumber(L, 3));
    float z = static_cast<float>(luaL_checknumber(L, 4));
    
    auto object = root_node->GetObjectByName(std::string(object_name));
    if (object) {
        object->SetRotation(x, y, z);
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }
    return 1;
}

int CMentalLuaScript::lua_getObjectRotation(lua_State* L) {
    CMentalRootNode* root_node = GetRootNodeFromLua(L);
    if (!root_node) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 3;
    }
    
    const char* object_name = luaL_checkstring(L, 1);
    auto object = root_node->GetObjectByName(std::string(object_name));
    if (object) {
        const auto& rot = object->GetRotation();
        lua_pushnumber(L, rot[0]);
        lua_pushnumber(L, rot[1]);
        lua_pushnumber(L, rot[2]);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }
    return 3;
}

int CMentalLuaScript::lua_setObjectScale(lua_State* L) {
    CMentalRootNode* root_node = GetRootNodeFromLua(L);
    if (!root_node) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    const char* object_name = luaL_checkstring(L, 1);
    float x = static_cast<float>(luaL_checknumber(L, 2));
    float y = static_cast<float>(luaL_checknumber(L, 3));
    float z = static_cast<float>(luaL_optnumber(L, 4, 1.0));
    
    auto object = root_node->GetObjectByName(std::string(object_name));
    if (object) {
        object->SetScale(x, y, z);
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }
    return 1;
}

int CMentalLuaScript::lua_getObjectScale(lua_State* L) {
    CMentalRootNode* root_node = GetRootNodeFromLua(L);
    if (!root_node) {
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, 1.0);
        return 3;
    }
    
    const char* object_name = luaL_checkstring(L, 1);
    auto object = root_node->GetObjectByName(std::string(object_name));
    if (object) {
        const auto& scale = object->GetScale();
        lua_pushnumber(L, scale[0]);
        lua_pushnumber(L, scale[1]);
        lua_pushnumber(L, scale[2]);
    } else {
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, 1.0);
    }
    return 3;
}

int CMentalLuaScript::lua_setObjectColor(lua_State* L) {
    CMentalRootNode* root_node = GetRootNodeFromLua(L);
    if (!root_node) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    const char* object_name = luaL_checkstring(L, 1);
    float r = static_cast<float>(luaL_checknumber(L, 2));
    float g = static_cast<float>(luaL_checknumber(L, 3));
    float b = static_cast<float>(luaL_checknumber(L, 4));
    float a = static_cast<float>(luaL_optnumber(L, 5, 1.0));
    
    auto object = root_node->GetObjectByName(std::string(object_name));
    if (object) {
        object->SetColor(r, g, b, a);
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }
    return 1;
}

int CMentalLuaScript::lua_getObjectColor(lua_State* L) {
    CMentalRootNode* root_node = GetRootNodeFromLua(L);
    if (!root_node) {
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, 1.0);
        return 4;
    }
    
    const char* object_name = luaL_checkstring(L, 1);
    auto object = root_node->GetObjectByName(std::string(object_name));
    if (object) {
        const auto& color = object->GetColor();
        lua_pushnumber(L, color[0]);
        lua_pushnumber(L, color[1]);
        lua_pushnumber(L, color[2]);
        lua_pushnumber(L, color[3]);
    } else {
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, 1.0);
    }
    return 4;
}

int CMentalLuaScript::lua_setObjectVisible(lua_State* L) {
    CMentalRootNode* root_node = GetRootNodeFromLua(L);
    if (!root_node) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    const char* object_name = luaL_checkstring(L, 1);
    bool visible = lua_toboolean(L, 2);
    
    auto object = root_node->GetObjectByName(std::string(object_name));
    if (object) {
        object->SetVisible(visible);
        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }
    return 1;
}

int CMentalLuaScript::lua_isObjectVisible(lua_State* L) {
    CMentalRootNode* root_node = GetRootNodeFromLua(L);
    if (!root_node) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    const char* object_name = luaL_checkstring(L, 1);
    auto object = root_node->GetObjectByName(std::string(object_name));
    if (object) {
        lua_pushboolean(L, object->IsVisible());
    } else {
        lua_pushboolean(L, false);
    }
    return 1;
}

// ============================================================================
// Camera Lua API Implementation
// ============================================================================

int CMentalLuaScript::lua_setCameraPosition(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_camera_) return 0;
    
    float x = static_cast<float>(luaL_checknumber(L, 1));
    float y = static_cast<float>(luaL_checknumber(L, 2));
    float z = static_cast<float>(luaL_checknumber(L, 3));
    
    script->bound_camera_->SetPosition(x, y, z);
    return 0;
}

int CMentalLuaScript::lua_getCameraPosition(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_camera_) return 0;
    
    const auto& pos = script->bound_camera_->GetPosition();
    lua_pushnumber(L, pos[0]);
    lua_pushnumber(L, pos[1]);
    lua_pushnumber(L, pos[2]);
    return 3;
}

int CMentalLuaScript::lua_setCameraTarget(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_camera_) return 0;
    
    float x = static_cast<float>(luaL_checknumber(L, 1));
    float y = static_cast<float>(luaL_checknumber(L, 2));
    float z = static_cast<float>(luaL_checknumber(L, 3));
    
    script->bound_camera_->SetTarget(x, y, z);
    return 0;
}

int CMentalLuaScript::lua_getCameraTarget(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_camera_) return 0;
    
    const auto& target = script->bound_camera_->GetTarget();
    lua_pushnumber(L, target[0]);
    lua_pushnumber(L, target[1]);
    lua_pushnumber(L, target[2]);
    return 3;
}

int CMentalLuaScript::lua_setCameraUp(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_camera_) return 0;
    
    float x = static_cast<float>(luaL_checknumber(L, 1));
    float y = static_cast<float>(luaL_checknumber(L, 2));
    float z = static_cast<float>(luaL_checknumber(L, 3));
    
    script->bound_camera_->SetUp(x, y, z);
    return 0;
}

int CMentalLuaScript::lua_getCameraUp(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_camera_) return 0;
    
    const auto& up = script->bound_camera_->GetUp();
    lua_pushnumber(L, up[0]);
    lua_pushnumber(L, up[1]);
    lua_pushnumber(L, up[2]);
    return 3;
}

int CMentalLuaScript::lua_lookAt(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_camera_) return 0;
    
    float eye_x = static_cast<float>(luaL_checknumber(L, 1));
    float eye_y = static_cast<float>(luaL_checknumber(L, 2));
    float eye_z = static_cast<float>(luaL_checknumber(L, 3));
    float target_x = static_cast<float>(luaL_checknumber(L, 4));
    float target_y = static_cast<float>(luaL_checknumber(L, 5));
    float target_z = static_cast<float>(luaL_checknumber(L, 6));
    float up_x = static_cast<float>(luaL_optnumber(L, 7, 0.0));
    float up_y = static_cast<float>(luaL_optnumber(L, 8, 1.0));
    float up_z = static_cast<float>(luaL_optnumber(L, 9, 0.0));
    
    script->bound_camera_->LookAt(eye_x, eye_y, eye_z, target_x, target_y, target_z, up_x, up_y, up_z);
    return 0;
}

int CMentalLuaScript::lua_setPerspective(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_camera_) return 0;
    
    float fov = static_cast<float>(luaL_checknumber(L, 1));
    float aspect_ratio = static_cast<float>(luaL_checknumber(L, 2));
    float near_plane = static_cast<float>(luaL_checknumber(L, 3));
    float far_plane = static_cast<float>(luaL_checknumber(L, 4));
    
    script->bound_camera_->SetPerspective(fov, aspect_ratio, near_plane, far_plane);
    return 0;
}

int CMentalLuaScript::lua_setFOV(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_camera_) return 0;
    
    float fov = static_cast<float>(luaL_checknumber(L, 1));
    script->bound_camera_->SetFOV(fov);
    return 0;
}

int CMentalLuaScript::lua_moveForward(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_camera_) return 0;
    
    float distance = static_cast<float>(luaL_checknumber(L, 1));
    script->bound_camera_->MoveForward(distance);
    return 0;
}

int CMentalLuaScript::lua_moveBackward(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_camera_) return 0;
    
    float distance = static_cast<float>(luaL_checknumber(L, 1));
    script->bound_camera_->MoveBackward(distance);
    return 0;
}

int CMentalLuaScript::lua_moveLeft(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_camera_) return 0;
    
    float distance = static_cast<float>(luaL_checknumber(L, 1));
    script->bound_camera_->MoveLeft(distance);
    return 0;
}

int CMentalLuaScript::lua_moveRight(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_camera_) return 0;
    
    float distance = static_cast<float>(luaL_checknumber(L, 1));
    script->bound_camera_->MoveRight(distance);
    return 0;
}

int CMentalLuaScript::lua_moveUp(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_camera_) return 0;
    
    float distance = static_cast<float>(luaL_checknumber(L, 1));
    script->bound_camera_->MoveUp(distance);
    return 0;
}

int CMentalLuaScript::lua_moveDown(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_camera_) return 0;
    
    float distance = static_cast<float>(luaL_checknumber(L, 1));
    script->bound_camera_->MoveDown(distance);
    return 0;
}

int CMentalLuaScript::lua_rotateYaw(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_camera_) return 0;
    
    float angle = static_cast<float>(luaL_checknumber(L, 1));
    script->bound_camera_->RotateYaw(angle);
    return 0;
}

int CMentalLuaScript::lua_rotatePitch(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_camera_) return 0;
    
    float angle = static_cast<float>(luaL_checknumber(L, 1));
    script->bound_camera_->RotatePitch(angle);
    return 0;
}

int CMentalLuaScript::lua_rotateRoll(lua_State* L) {
    CMentalLuaScript* script = GetScriptFromLua(L);
    if (!script || !script->bound_camera_) return 0;
    
    float angle = static_cast<float>(luaL_checknumber(L, 1));
    script->bound_camera_->RotateRoll(angle);
    return 0;
}

// ============================================================================
// CMentalRootNode Implementation
// ============================================================================

CMentalRootNode::CMentalRootNode() 
    : lua_script_(nullptr), camera_(nullptr), hot_reload_enabled_(false), last_script_time_(0) {
    std::cout << "CMentalRootNode: Creating root node..." << std::endl;
}

void CMentalRootNode::SetLuaScript(std::shared_ptr<CMentalLuaScript> script) {
    lua_script_ = script;
    if (lua_script_) {
        // Store pointer to this root node in Lua registry
        lua_pushlightuserdata(lua_script_->GetLuaState(), this);
        lua_setfield(lua_script_->GetLuaState(), LUA_REGISTRYINDEX, "root_node_instance");
        
        lua_script_->CallInit();
        std::cout << "CMentalRootNode: Lua script attached and initialized" << std::endl;
    }
}

void CMentalRootNode::LoadLuaScript(const std::string& script_path) {
    script_path_ = script_path;
    auto script = std::make_shared<CMentalLuaScript>();
    if (script->LoadFromFile(script_path)) {
        SetLuaScript(script);
        
        // Store file modification time for hot reload
        struct stat file_stat;
        if (stat(script_path.c_str(), &file_stat) == 0) {
            last_script_time_ = file_stat.st_mtime;
        }
    } else {
        std::cerr << "Failed to load Lua script for root node: " << script_path << std::endl;
    }
}

std::shared_ptr<CMentalObject> CMentalRootNode::CreateObject(const std::string& name, const std::string& type) {
    std::cout << "CMentalRootNode: Creating object '" << name << "' of type '" << type << "'" << std::endl;
    
    // Check if object with this name already exists
    if (named_objects_.find(name) != named_objects_.end()) {
        std::cerr << "Object with name '" << name << "' already exists!" << std::endl;
        return nullptr;
    }
    
    auto object = std::make_shared<CMentalObject>();
    
    // Initialize based on type
    if (type == "rectangle") {
        if (!object->InitializeRectangle()) {
            std::cerr << "Failed to initialize rectangle object" << std::endl;
            return nullptr;
        }
    } else if (type == "triangle") {
        if (!object->InitializeTriangle()) {
            std::cerr << "Failed to initialize triangle object" << std::endl;
            return nullptr;
        }
    } else {
        std::cerr << "Unknown object type: " << type << std::endl;
        return nullptr;
    }
    
    // Store object
    objects_.push_back(object);
    named_objects_[name] = object;
    
    std::cout << "CMentalRootNode: Object '" << name << "' created successfully" << std::endl;
    return object;
}

std::shared_ptr<CMentalShaderMaterial> CMentalRootNode::CreateShader(const std::string& name, 
                                                                     const std::string& vertex_path, 
                                                                     const std::string& fragment_path) {
    std::cout << "CMentalRootNode: Creating shader '" << name << "'" << std::endl;
    
    // Check if shader with this name already exists
    if (named_shaders_.find(name) != named_shaders_.end()) {
        std::cerr << "Shader with name '" << name << "' already exists!" << std::endl;
        return nullptr;
    }
    
    auto shader = std::make_shared<CMentalShaderMaterial>();
    if (!shader->CompileFromFiles(vertex_path, fragment_path)) {
        std::cerr << "Failed to compile shader '" << name << "'" << std::endl;
        return nullptr;
    }
    
    // Store shader
    shaders_.push_back(shader);
    named_shaders_[name] = shader;
    
    std::cout << "CMentalRootNode: Shader '" << name << "' created successfully" << std::endl;
    return shader;
}

bool CMentalRootNode::AttachShaderToObject(const std::string& object_name, const std::string& shader_name) {
    std::cout << "CMentalRootNode: Attaching shader '" << shader_name << "' to object '" << object_name << "'" << std::endl;
    
    auto object_it = named_objects_.find(object_name);
    auto shader_it = named_shaders_.find(shader_name);
    
    if (object_it == named_objects_.end()) {
        std::cerr << "Object '" << object_name << "' not found!" << std::endl;
        return false;
    }
    
    if (shader_it == named_shaders_.end()) {
        std::cerr << "Shader '" << shader_name << "' not found!" << std::endl;
        return false;
    }
    
    object_it->second->SetMaterial(shader_it->second);
    std::cout << "CMentalRootNode: Shader attached successfully" << std::endl;
    return true;
}

bool CMentalRootNode::AttachScriptToObject(const std::string& object_name, const std::string& script_path) {
    std::cout << "CMentalRootNode: Attaching script '" << script_path << "' to object '" << object_name << "'" << std::endl;
    
    auto object_it = named_objects_.find(object_name);
    if (object_it == named_objects_.end()) {
        std::cerr << "Object '" << object_name << "' not found!" << std::endl;
        return false;
    }
    
    object_it->second->LoadLuaScript(script_path);
    std::cout << "CMentalRootNode: Script attached successfully" << std::endl;
    return true;
}

std::shared_ptr<CMentalObject> CMentalRootNode::GetObjectByName(const std::string& name) {
    auto it = named_objects_.find(name);
    return (it != named_objects_.end()) ? it->second : nullptr;
}

std::shared_ptr<CMentalShaderMaterial> CMentalRootNode::GetShaderByName(const std::string& name) {
    auto it = named_shaders_.find(name);
    return (it != named_shaders_.end()) ? it->second : nullptr;
}

bool CMentalRootNode::RemoveObject(const std::string& name) {
    std::cout << "CMentalRootNode: Removing object '" << name << "'" << std::endl;
    
    auto it = named_objects_.find(name);
    if (it == named_objects_.end()) {
        std::cerr << "Object '" << name << "' not found!" << std::endl;
        return false;
    }
    
    // Remove from objects vector
    auto object = it->second;
    objects_.erase(std::remove(objects_.begin(), objects_.end(), object), objects_.end());
    
    // Remove from named objects map
    named_objects_.erase(it);
    
    std::cout << "CMentalRootNode: Object '" << name << "' removed successfully" << std::endl;
    return true;
}

void CMentalRootNode::SetCamera(std::shared_ptr<CMentalCamera> camera) {
    camera_ = camera;
    if (camera_) {
        std::cout << "CMentalRootNode: Camera set successfully" << std::endl;
    }
}

void CMentalRootNode::Update(float delta_time) {
    // Check for script file changes if hot reload is enabled
    if (hot_reload_enabled_ && !script_path_.empty()) {
        struct stat file_stat;
        if (stat(script_path_.c_str(), &file_stat) == 0) {
            if (file_stat.st_mtime > last_script_time_) {
                std::cout << "CMentalRootNode: Script file changed, reloading..." << std::endl;
                ReloadScript();
                last_script_time_ = file_stat.st_mtime;
            }
        }
    }
    
    // Update root script first
    if (lua_script_) {
        lua_script_->CallUpdate(delta_time);
    }
    
    // Update camera
    if (camera_) {
        camera_->Update(delta_time);
    }
    
    // Update all objects
    for (const auto& object : objects_) {
        if (object && object->IsInitialized()) {
            object->Update(delta_time);
        }
    }
}

void CMentalRootNode::ReloadScript() {
    if (script_path_.empty()) {
        std::cout << "CMentalRootNode: No script path to reload" << std::endl;
        return;
    }
    
    std::cout << "CMentalRootNode: Reloading script: " << script_path_ << std::endl;
    
    // Clear current scene
    ClearScene();
    
    // Reload script
    auto script = std::make_shared<CMentalLuaScript>();
    if (script->LoadFromFile(script_path_)) {
        SetLuaScript(script);
        std::cout << "CMentalRootNode: Script reloaded successfully" << std::endl;
    } else {
        std::cerr << "CMentalRootNode: Failed to reload script: " << script_path_ << std::endl;
    }
}

void CMentalRootNode::ClearScene() {
    std::cout << "CMentalRootNode: Clearing scene..." << std::endl;
    
    // Clear all objects and shaders
    objects_.clear();
    shaders_.clear();
    named_objects_.clear();
    named_shaders_.clear();
    
    std::cout << "CMentalRootNode: Scene cleared" << std::endl;
}

void CMentalRootNode::Render() const {
    // Call root script render
    if (lua_script_) {
        lua_script_->CallRender();
    }
    
    // Render all objects
    for (const auto& object : objects_) {
        if (object && object->IsInitialized()) {
            // Set camera matrices if camera is available
            if (camera_ && object->GetLuaScript()) {
                auto material = object->GetMaterial();
                if (material && material->IsCompiled()) {
                    material->Use();
                    
                    // Set camera matrices
                    const auto& viewMatrix = camera_->GetViewMatrix();
                    const auto& projectionMatrix = camera_->GetProjectionMatrix();
                    
                    material->SetUniformMatrix4("viewMatrix", viewMatrix);
                    material->SetUniformMatrix4("projectionMatrix", projectionMatrix);
                    
                    material->Unuse();
                }
            }
            
            object->Render();
        }
    }
}

void CMentalRootNode::ListObjects() const {
    std::cout << "=== CMentalRootNode Objects ===" << std::endl;
    std::cout << "Total objects: " << objects_.size() << std::endl;
    std::cout << "Total shaders: " << shaders_.size() << std::endl;
    
    std::cout << "\nObjects:" << std::endl;
    for (const auto& pair : named_objects_) {
        std::cout << "  - " << pair.first << " (initialized: " << pair.second->IsInitialized() << ")" << std::endl;
    }
    
    std::cout << "\nShaders:" << std::endl;
    for (const auto& pair : named_shaders_) {
        std::cout << "  - " << pair.first << " (compiled: " << pair.second->IsCompiled() << ")" << std::endl;
    }
    std::cout << "===============================" << std::endl;
}

// ============================================================================
// CMentalCamera Implementation
// ============================================================================

CMentalCamera::CMentalCamera() 
    : lua_script_(nullptr),
      position_{{0.0f, 0.0f, 3.0f}},
      target_{{0.0f, 0.0f, 0.0f}},
      up_{{0.0f, 1.0f, 0.0f}},
      fov_(45.0f),
      aspect_ratio_(16.0f / 9.0f),
      near_plane_(0.1f),
      far_plane_(100.0f),
      matrices_dirty_(true) {
    std::cout << "CMentalCamera: Creating camera..." << std::endl;
    
    // Initialize matrices to identity
    for (int i = 0; i < 16; ++i) {
        view_matrix_[i] = 0.0f;
        projection_matrix_[i] = 0.0f;
        view_projection_matrix_[i] = 0.0f;
    }
    
    // Set diagonal to 1 for identity matrices
    view_matrix_[0] = view_matrix_[5] = view_matrix_[10] = view_matrix_[15] = 1.0f;
    projection_matrix_[0] = projection_matrix_[5] = projection_matrix_[10] = projection_matrix_[15] = 1.0f;
    view_projection_matrix_[0] = view_projection_matrix_[5] = view_projection_matrix_[10] = view_projection_matrix_[15] = 1.0f;
}

void CMentalCamera::SetLuaScript(std::shared_ptr<CMentalLuaScript> script) {
    lua_script_ = script;
    if (lua_script_) {
        lua_script_->BindToCamera(this);
        lua_script_->CallInit();
        std::cout << "CMentalCamera: Lua script attached and initialized" << std::endl;
    }
}

void CMentalCamera::LoadLuaScript(const std::string& script_path) {
    auto script = std::make_shared<CMentalLuaScript>();
    if (script->LoadFromFile(script_path)) {
        SetLuaScript(script);
    } else {
        std::cerr << "Failed to load Lua script for camera: " << script_path << std::endl;
    }
}

void CMentalCamera::SetPosition(float x, float y, float z) {
    position_[0] = x;
    position_[1] = y;
    position_[2] = z;
    matrices_dirty_ = true;
}

void CMentalCamera::SetTarget(float x, float y, float z) {
    target_[0] = x;
    target_[1] = y;
    target_[2] = z;
    matrices_dirty_ = true;
}

void CMentalCamera::SetUp(float x, float y, float z) {
    up_[0] = x;
    up_[1] = y;
    up_[2] = z;
    matrices_dirty_ = true;
}

void CMentalCamera::LookAt(float eye_x, float eye_y, float eye_z, 
                          float target_x, float target_y, float target_z,
                          float up_x, float up_y, float up_z) {
    SetPosition(eye_x, eye_y, eye_z);
    SetTarget(target_x, target_y, target_z);
    SetUp(up_x, up_y, up_z);
}

void CMentalCamera::SetPerspective(float fov, float aspect_ratio, float near_plane, float far_plane) {
    fov_ = fov;
    aspect_ratio_ = aspect_ratio;
    near_plane_ = near_plane;
    far_plane_ = far_plane;
    matrices_dirty_ = true;
}

void CMentalCamera::SetFOV(float fov) {
    fov_ = fov;
    matrices_dirty_ = true;
}

void CMentalCamera::SetAspectRatio(float aspect_ratio) {
    aspect_ratio_ = aspect_ratio;
    matrices_dirty_ = true;
}

void CMentalCamera::SetNearFar(float near_plane, float far_plane) {
    near_plane_ = near_plane;
    far_plane_ = far_plane;
    matrices_dirty_ = true;
}

void CMentalCamera::MoveForward(float distance) {
    auto forward = GetForwardVector();
    position_[0] += forward[0] * distance;
    position_[1] += forward[1] * distance;
    position_[2] += forward[2] * distance;
    matrices_dirty_ = true;
}

void CMentalCamera::MoveBackward(float distance) {
    MoveForward(-distance);
}

void CMentalCamera::MoveLeft(float distance) {
    auto right = GetRightVector();
    position_[0] -= right[0] * distance;
    position_[1] -= right[1] * distance;
    position_[2] -= right[2] * distance;
    matrices_dirty_ = true;
}

void CMentalCamera::MoveRight(float distance) {
    MoveLeft(-distance);
}

void CMentalCamera::MoveUp(float distance) {
    position_[0] += up_[0] * distance;
    position_[1] += up_[1] * distance;
    position_[2] += up_[2] * distance;
    matrices_dirty_ = true;
}

void CMentalCamera::MoveDown(float distance) {
    MoveUp(-distance);
}

void CMentalCamera::RotateYaw(float angle) {
    // Поворот вокруг оси Y (влево-вправо)
    float cos_angle = std::cos(angle);
    float sin_angle = std::sin(angle);
    
    auto forward = GetForwardVector();
    auto right = GetRightVector();
    
    // Новое направление forward
    std::array<float, 3> new_forward = {
        forward[0] * cos_angle + right[0] * sin_angle,
        forward[1],
        forward[2] * cos_angle + right[2] * sin_angle
    };
    
    new_forward = Normalize(new_forward);
    
    target_[0] = position_[0] + new_forward[0];
    target_[1] = position_[1] + new_forward[1];
    target_[2] = position_[2] + new_forward[2];
    
    matrices_dirty_ = true;
}

void CMentalCamera::RotatePitch(float angle) {
    // Поворот вокруг оси X (вверх-вниз)
    float cos_angle = std::cos(angle);
    float sin_angle = std::sin(angle);
    
    auto forward = GetForwardVector();
    auto right = GetRightVector();
    auto up = CrossProduct(right, forward);
    
    // Новое направление forward
    std::array<float, 3> new_forward = {
        forward[0] * cos_angle + up[0] * sin_angle,
        forward[1] * cos_angle + up[1] * sin_angle,
        forward[2] * cos_angle + up[2] * sin_angle
    };
    
    new_forward = Normalize(new_forward);
    
    target_[0] = position_[0] + new_forward[0];
    target_[1] = position_[1] + new_forward[1];
    target_[2] = position_[2] + new_forward[2];
    
    matrices_dirty_ = true;
}

void CMentalCamera::RotateRoll(float angle) {
    // Поворот вокруг оси Z (наклон)
    float cos_angle = std::cos(angle);
    float sin_angle = std::sin(angle);
    
    auto right = GetRightVector();
    auto forward = GetForwardVector();
    auto current_up = CrossProduct(right, forward);
    
    // Новое направление up
    up_[0] = current_up[0] * cos_angle + right[0] * sin_angle;
    up_[1] = current_up[1] * cos_angle + right[1] * sin_angle;
    up_[2] = current_up[2] * cos_angle + right[2] * sin_angle;
    
    up_ = Normalize(up_);
    matrices_dirty_ = true;
}

void CMentalCamera::Update(float delta_time) {
    if (lua_script_) {
        lua_script_->CallUpdate(delta_time);
    }
}

const std::array<float, 16>& CMentalCamera::GetViewMatrix() {
    if (matrices_dirty_) {
        UpdateMatrices();
    }
    return view_matrix_;
}

const std::array<float, 16>& CMentalCamera::GetProjectionMatrix() {
    if (matrices_dirty_) {
        UpdateMatrices();
    }
    return projection_matrix_;
}

const std::array<float, 16>& CMentalCamera::GetViewProjectionMatrix() {
    if (matrices_dirty_) {
        UpdateMatrices();
    }
    return view_projection_matrix_;
}

void CMentalCamera::UpdateMatrices() {
    CalculateViewMatrix();
    CalculateProjectionMatrix();
    
    // Calculate view-projection matrix (simplified multiplication)
    // For now, just copy view matrix - proper matrix multiplication would be needed for full implementation
    for (int i = 0; i < 16; ++i) {
        view_projection_matrix_[i] = view_matrix_[i];
    }
    
    matrices_dirty_ = false;
}

void CMentalCamera::CalculateViewMatrix() {
    // Simplified view matrix calculation
    auto forward = GetForwardVector();
    auto right = GetRightVector();
    auto up = CrossProduct(right, forward);
    
    // Column-major order for OpenGL
    view_matrix_[0] = right[0];
    view_matrix_[1] = up[0];
    view_matrix_[2] = -forward[0];
    view_matrix_[3] = 0.0f;
    
    view_matrix_[4] = right[1];
    view_matrix_[5] = up[1];
    view_matrix_[6] = -forward[1];
    view_matrix_[7] = 0.0f;
    
    view_matrix_[8] = right[2];
    view_matrix_[9] = up[2];
    view_matrix_[10] = -forward[2];
    view_matrix_[11] = 0.0f;
    
    view_matrix_[12] = -DotProduct(right, position_);
    view_matrix_[13] = -DotProduct(up, position_);
    view_matrix_[14] = DotProduct(forward, position_);
    view_matrix_[15] = 1.0f;
}

void CMentalCamera::CalculateProjectionMatrix() {
    // Perspective projection matrix
    float fov_rad = fov_ * M_PI / 180.0f;
    float tan_half_fov = std::tan(fov_rad / 2.0f);
    
    // Clear matrix
    for (int i = 0; i < 16; ++i) {
        projection_matrix_[i] = 0.0f;
    }
    
    projection_matrix_[0] = 1.0f / (aspect_ratio_ * tan_half_fov);
    projection_matrix_[5] = 1.0f / tan_half_fov;
    projection_matrix_[10] = -(far_plane_ + near_plane_) / (far_plane_ - near_plane_);
    projection_matrix_[11] = -1.0f;
    projection_matrix_[14] = -(2.0f * far_plane_ * near_plane_) / (far_plane_ - near_plane_);
}

std::array<float, 3> CMentalCamera::GetForwardVector() const {
    std::array<float, 3> forward = {
        target_[0] - position_[0],
        target_[1] - position_[1],
        target_[2] - position_[2]
    };
    return Normalize(forward);
}

std::array<float, 3> CMentalCamera::GetRightVector() const {
    auto forward = GetForwardVector();
    return Normalize(CrossProduct(forward, up_));
}

std::array<float, 3> CMentalCamera::CrossProduct(const std::array<float, 3>& a, const std::array<float, 3>& b) const {
    return {
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0]
    };
}

std::array<float, 3> CMentalCamera::Normalize(const std::array<float, 3>& v) const {
    float length = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (length > 0.0f) {
        return {v[0] / length, v[1] / length, v[2] / length};
    }
    return {0.0f, 0.0f, 0.0f};
}

float CMentalCamera::DotProduct(const std::array<float, 3>& a, const std::array<float, 3>& b) const {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

// ============================================================================
// CMentalShaderMaterial Implementation
// ============================================================================

CMentalShaderMaterial::CMentalShaderMaterial() 
    : shader_program_(0), vertex_shader_(0), fragment_shader_(0), is_compiled_(false) {
    std::cout << "CMentalShaderMaterial: Creating shader material..." << std::endl;
}

CMentalShaderMaterial::~CMentalShaderMaterial() {
    if (is_compiled_) {
        if (vertex_shader_ != 0) {
            glDeleteShader(vertex_shader_);
        }
        if (fragment_shader_ != 0) {
            glDeleteShader(fragment_shader_);
        }
        if (shader_program_ != 0) {
            glDeleteProgram(shader_program_);
        }
    }
}

bool CMentalShaderMaterial::CompileFromSource(const std::string& vertex_source, const std::string& fragment_source) {
    std::cout << "CMentalShaderMaterial: Compiling shaders from source..." << std::endl;
    
    // Compile vertex shader
    vertex_shader_ = CompileShader(vertex_source, GL_VERTEX_SHADER);
    if (vertex_shader_ == 0) {
        return false;
    }
    
    // Compile fragment shader
    fragment_shader_ = CompileShader(fragment_source, GL_FRAGMENT_SHADER);
    if (fragment_shader_ == 0) {
        glDeleteShader(vertex_shader_);
        return false;
    }
    
    // Link program
    if (!LinkProgram()) {
        glDeleteShader(vertex_shader_);
        glDeleteShader(fragment_shader_);
        return false;
    }
    
    is_compiled_ = true;
    std::cout << "CMentalShaderMaterial: Shaders compiled successfully" << std::endl;
    return true;
}

bool CMentalShaderMaterial::CompileFromFiles(const std::string& vertex_path, const std::string& fragment_path) {
    std::cout << "CMentalShaderMaterial: Loading shaders from files..." << std::endl;
    
    // Read vertex shader
    std::ifstream vertex_file(vertex_path);
    if (!vertex_file.is_open()) {
        std::cerr << "Failed to open vertex shader file: " << vertex_path << std::endl;
        return false;
    }
    
    std::stringstream vertex_stream;
    vertex_stream << vertex_file.rdbuf();
    std::string vertex_source = vertex_stream.str();
    vertex_file.close();
    
    // Read fragment shader
    std::ifstream fragment_file(fragment_path);
    if (!fragment_file.is_open()) {
        std::cerr << "Failed to open fragment shader file: " << fragment_path << std::endl;
        return false;
    }
    
    std::stringstream fragment_stream;
    fragment_stream << fragment_file.rdbuf();
    std::string fragment_source = fragment_stream.str();
    fragment_file.close();
    
    return CompileFromSource(vertex_source, fragment_source);
}

void CMentalShaderMaterial::Use() const {
    if (is_compiled_) {
        glUseProgram(shader_program_);
    }
}

void CMentalShaderMaterial::Unuse() const {
    glUseProgram(0);
}

void CMentalShaderMaterial::SetUniform(const std::string& name, float value) const {
    if (is_compiled_) {
        int location = glGetUniformLocation(shader_program_, name.c_str());
        if (location != -1) {
            glUniform1f(location, value);
        }
    }
}

void CMentalShaderMaterial::SetUniform(const std::string& name, int value) const {
    if (is_compiled_) {
        int location = glGetUniformLocation(shader_program_, name.c_str());
        if (location != -1) {
            glUniform1i(location, value);
        }
    }
}

void CMentalShaderMaterial::SetUniform(const std::string& name, const std::array<float, 3>& value) const {
    if (is_compiled_) {
        int location = glGetUniformLocation(shader_program_, name.c_str());
        if (location != -1) {
            glUniform3f(location, value[0], value[1], value[2]);
        }
    }
}

void CMentalShaderMaterial::SetUniform(const std::string& name, const std::array<float, 4>& value) const {
    if (is_compiled_) {
        int location = glGetUniformLocation(shader_program_, name.c_str());
        if (location != -1) {
            glUniform4f(location, value[0], value[1], value[2], value[3]);
        }
    }
}

void CMentalShaderMaterial::SetUniformMatrix4(const std::string& name, const std::array<float, 16>& matrix) const {
    if (is_compiled_) {
        int location = glGetUniformLocation(shader_program_, name.c_str());
        if (location != -1) {
            glUniformMatrix4fv(location, 1, GL_FALSE, matrix.data());
        }
    }
}

unsigned int CMentalShaderMaterial::CompileShader(const std::string& source, unsigned int type) {
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    // Check compilation errors
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        std::string shader_type = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
        std::cerr << "Shader compilation error (" << shader_type << "): " << info_log << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

bool CMentalShaderMaterial::LinkProgram() {
    shader_program_ = glCreateProgram();
    glAttachShader(shader_program_, vertex_shader_);
    glAttachShader(shader_program_, fragment_shader_);
    glLinkProgram(shader_program_);
    
    // Check linking errors
    int success;
    glGetProgramiv(shader_program_, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(shader_program_, 512, nullptr, info_log);
        std::cerr << "Shader program linking error: " << info_log << std::endl;
        glDeleteProgram(shader_program_);
        shader_program_ = 0;
        return false;
    }
    
    return true;
}

void CMentalShaderMaterial::CheckCompileErrors(unsigned int shader, const std::string& type) {
    int success;
    char info_log[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, info_log);
            std::cerr << "Shader compilation error of type: " << type << "\n" << info_log << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, info_log);
            std::cerr << "Program linking error of type: " << type << "\n" << info_log << std::endl;
        }
    }
}

// ============================================================================
// CMentalObject Implementation
// ============================================================================

CMentalObject::CMentalObject() 
    : VAO_(0), VBO_(0), EBO_(0), material_(nullptr), lua_script_(nullptr),
      position_{{0.0f, 0.0f, 0.0f}}, 
      rotation_{{0.0f, 0.0f, 0.0f}}, 
      scale_{{1.0f, 1.0f, 1.0f}}, 
      color_{{1.0f, 1.0f, 1.0f, 1.0f}}, 
      is_initialized_(false), is_visible_(true), is_triangle_(false) {
    std::cout << "CMentalObject: Creating object..." << std::endl;
}

CMentalObject::~CMentalObject() {
    if (is_initialized_) {
        glDeleteVertexArrays(1, &VAO_);
        glDeleteBuffers(1, &VBO_);
        glDeleteBuffers(1, &EBO_);
    }
}

bool CMentalObject::InitializeRectangle() {
    std::cout << "CMentalObject: Initializing rectangle geometry..." << std::endl;
    
    CreateRectangleGeometry();
    is_initialized_ = true;
    
    std::cout << "CMentalObject: Rectangle initialized successfully" << std::endl;
    return true;
}

bool CMentalObject::InitializeTriangle() {
    std::cout << "CMentalObject: Initializing triangle geometry..." << std::endl;
    
    CreateTriangleGeometry();
    is_initialized_ = true;
    is_triangle_ = true;
    
    std::cout << "CMentalObject: Triangle initialized successfully" << std::endl;
    return true;
}

void CMentalObject::SetMaterial(std::shared_ptr<CMentalShaderMaterial> material) {
    material_ = material;
}

void CMentalObject::SetLuaScript(std::shared_ptr<CMentalLuaScript> script) {
    lua_script_ = script;
    if (lua_script_) {
        lua_script_->BindToObject(this);
        lua_script_->CallInit();
        std::cout << "CMentalObject: Lua script attached and initialized" << std::endl;
    }
}

void CMentalObject::LoadLuaScript(const std::string& script_path) {
    auto script = std::make_shared<CMentalLuaScript>();
    if (script->LoadFromFile(script_path)) {
        SetLuaScript(script);
    } else {
        std::cerr << "Failed to load Lua script: " << script_path << std::endl;
    }
}

void CMentalObject::SetPosition(float x, float y, float z) {
    position_[0] = x;
    position_[1] = y;
    position_[2] = z;
}

void CMentalObject::SetRotation(float x, float y, float z) {
    rotation_[0] = x;
    rotation_[1] = y;
    rotation_[2] = z;
}

void CMentalObject::SetScale(float x, float y, float z) {
    scale_[0] = x;
    scale_[1] = y;
    scale_[2] = z;
}

void CMentalObject::SetColor(float r, float g, float b, float a) {
    color_[0] = r;
    color_[1] = g;
    color_[2] = b;
    color_[3] = a;
}

void CMentalObject::Update(float delta_time) {
    if (lua_script_) {
        lua_script_->CallUpdate(delta_time);
    }
}

void CMentalObject::Render() const {
    if (!is_initialized_ || !material_ || !material_->IsCompiled() || !is_visible_) {
        return;
    }
    
    // Call Lua render function if available
    if (lua_script_) {
        lua_script_->CallRender();
    }
    
    material_->Use();
    UpdateUniforms();
    
    glBindVertexArray(VAO_);
    
    if (is_triangle_) {
        // Draw triangle (3 vertices)
        glDrawArrays(GL_TRIANGLES, 0, 3);
    } else {
        // Draw rectangle (6 indices)
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    
    glBindVertexArray(0);
    material_->Unuse();
}

void CMentalObject::CreateRectangleGeometry() {
    // Rectangle vertices with positions and colors (3D)
    float vertices[] = {
        // positions        // colors
         0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // top right (red)
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom right (green)
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  // bottom left (blue)
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f   // top left (yellow)
    };
    
    unsigned int indices[] = {
        0, 1, 3,  // first triangle
        1, 2, 3   // second triangle
    };
    
    // Generate and bind VAO
    glGenVertexArrays(1, &VAO_);
    glBindVertexArray(VAO_);
    
    // Generate and bind VBO
    glGenBuffers(1, &VBO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Generate and bind EBO
    glGenBuffers(1, &EBO_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Unbind VAO
    glBindVertexArray(0);
}

void CMentalObject::CreateTriangleGeometry() {
    // Triangle vertices with positions and colors (3D)
    float vertices[] = {
        // positions        // colors
         0.0f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  // top (yellow)
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 1.0f,  // bottom left (magenta)
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 1.0f   // bottom right (cyan)
    };
    
    // Generate and bind VAO
    glGenVertexArrays(1, &VAO_);
    glBindVertexArray(VAO_);
    
    // Generate and bind VBO
    glGenBuffers(1, &VBO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Unbind VAO
    glBindVertexArray(0);
}

void CMentalObject::UpdateUniforms() const {
    if (material_) {
        // Set time uniform for animation
        static float time = 0.0f;
        time += 0.016f; // Approximate 60 FPS
        material_->SetUniform("time", time);
        
        // Set color uniform
        material_->SetUniform("objectColor", color_);
    }
}

// ============================================================================
// CMentalOGLRenderPipeline Implementation
// ============================================================================

CMentalOGLRenderPipeline::CMentalOGLRenderPipeline() : root_node_(nullptr) {
    std::cout << "CMentalOGLRenderPipeline: Initializing render pipeline..." << std::endl;
    
    // Create default root node
    root_node_ = std::make_shared<CMentalRootNode>();
    
    std::cout << "CMentalOGLRenderPipeline: Basic initialization completed" << std::endl;
}

void CMentalOGLRenderPipeline::InitializeOpenGL() {
    std::cout << "CMentalOGLRenderPipeline: Initializing OpenGL..." << std::endl;
    
    // Set default OpenGL state
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    std::cout << "CMentalOGLRenderPipeline: OpenGL initialized successfully" << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void CMentalOGLRenderPipeline::SetRootNode(std::shared_ptr<CMentalRootNode> root_node) {
    root_node_ = root_node;
    if (root_node_) {
        std::cout << "CMentalOGLRenderPipeline: Root node set successfully" << std::endl;
    }
}

void CMentalOGLRenderPipeline::AddObject(std::shared_ptr<CMentalObject> object) {
    // Legacy support - add to root node if available
    if (root_node_ && object) {
        // For legacy support, we'll add objects directly to root node's vector
        // This is not ideal but maintains backward compatibility
        std::cout << "CMentalOGLRenderPipeline: Adding object via legacy method" << std::endl;
    }
}

void CMentalOGLRenderPipeline::RemoveObject(std::shared_ptr<CMentalObject> object) {
    // Legacy support
    std::cout << "CMentalOGLRenderPipeline: Legacy remove object called" << std::endl;
}

void CMentalOGLRenderPipeline::ClearObjects() {
    // Legacy support
    std::cout << "CMentalOGLRenderPipeline: Legacy clear objects called" << std::endl;
}

void CMentalOGLRenderPipeline::SetCamera(std::shared_ptr<CMentalCamera> camera) {
    if (root_node_) {
        root_node_->SetCamera(camera);
    }
}

std::shared_ptr<CMentalCamera> CMentalOGLRenderPipeline::GetCamera() const {
    return root_node_ ? root_node_->GetCamera() : nullptr;
}

void CMentalOGLRenderPipeline::UpdateAll(float delta_time) {
    // Update mouse position for input handling
    UpdateMousePosition();
    
    // Update root node (which updates camera and all objects)
    if (root_node_) {
        root_node_->Update(delta_time);
    }
}

void CMentalOGLRenderPipeline::RenderAll() const {
    if (root_node_) {
        root_node_->Render();
    }
}

size_t CMentalOGLRenderPipeline::GetObjectCount() const {
    return root_node_ ? root_node_->GetObjectCount() : 0;
}

// ============================================================================
// CMentalOGLRenderer Implementation
// ============================================================================

template <class P>
CMentalOGLRenderer<P>::CMentalOGLRenderer() {
    std::cout << "CMentalOGLRenderer: Initializing renderer..." << std::endl;
    
    // The render pipeline is already created via member initializer
    // OpenGL initialization will be done later via InitializeOpenGL()
    
    std::cout << "CMentalOGLRenderer: Basic initialization completed" << std::endl;
}

template <class P>
void CMentalOGLRenderer<P>::InitializeOpenGL() {
    std::cout << "CMentalOGLRenderer: Initializing OpenGL..." << std::endl;
    
    try {
        if (up_renderpipeline_) {
            up_renderpipeline_->InitializeOpenGL();
        }
        std::cout << "CMentalOGLRenderer: OpenGL initialization completed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "CMentalOGLRenderer: Failed to initialize OpenGL: " << e.what() << std::endl;
        throw;
    }
}

// ============================================================================
// CMentalWindowManager Implementation
// ============================================================================

template<typename T, class R>
CMentalWindowManager<T, R>::CMentalWindowManager() 
    : up_window_(nullptr, [](T* ptr) { 
        if (ptr) glfwDestroyWindow(ptr); 
      }) {
    std::cout << "CMentalWindowManager: Initializing window manager..." << std::endl;
    
    // Initialize GLFW
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    
    std::cout << "CMentalWindowManager: GLFW initialized successfully" << std::endl;
    
    // Set GLFW window hints for OpenGL context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For macOS compatibility
    
    // Create window
    T* window = static_cast<T*>(glfwCreateWindow(800, 600, "Mental SDK Window", nullptr, nullptr));
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    
    // Store window in unique_ptr
    up_window_.reset(window);
    
    // Set global window pointer for input handling
    g_current_window = window;
    
    // Make context current
    glfwMakeContextCurrent(window);
    
    // Enable V-Sync
    glfwSwapInterval(1);
    
    std::cout << "CMentalWindowManager: Window created successfully (800x600)" << std::endl;
    
    try {
        // Initialize OpenGL after context is created
        if (up_renderer_) {
            up_renderer_->InitializeOpenGL();
        }
        std::cout << "CMentalWindowManager: Renderer initialized successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "CMentalWindowManager: Failed to initialize renderer: " << e.what() << std::endl;
        glfwTerminate();
        throw;
    }
    
    std::cout << "CMentalWindowManager: Initialized successfully" << std::endl;
}

template<typename T, class R>
CMentalWindowManager<T, R>::~CMentalWindowManager() {
    std::cout << "CMentalWindowManager: Cleaning up..." << std::endl;
    
    // Clear global window pointer
    g_current_window = nullptr;
    
    // unique_ptr with custom deleter will handle window cleanup
    up_window_.reset();
    glfwTerminate();
    
    std::cout << "CMentalWindowManager: Cleanup completed" << std::endl;
}

// ============================================================================
// Additional utility methods for CMentalWindowManager
// ============================================================================

template<typename T, class R>
bool CMentalWindowManager<T, R>::ShouldClose() const {
    if (!up_window_) return true;
    return glfwWindowShouldClose(up_window_.get());
}

template<typename T, class R>
void CMentalWindowManager<T, R>::PollEvents() {
    glfwPollEvents();
}

template<typename T, class R>
void CMentalWindowManager<T, R>::SwapBuffers() {
    if (up_window_) {
        glfwSwapBuffers(up_window_.get());
    }
}

template<typename T, class R>
void CMentalWindowManager<T, R>::Clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

template<typename T, class R>
void CMentalWindowManager<T, R>::SetTitle(const std::string& title) {
    if (up_window_) {
        glfwSetWindowTitle(up_window_.get(), title.c_str());
    }
}

template<typename T, class R>
void CMentalWindowManager<T, R>::GetWindowSize(int& width, int& height) const {
    if (up_window_) {
        glfwGetWindowSize(up_window_.get(), &width, &height);
    } else {
        width = height = 0;
    }
}

// ============================================================================
// Explicit template instantiations
// ============================================================================

// Instantiate the most common template combinations
template class CMentalOGLRenderer<CMentalOGLRenderPipeline>;
template class CMentalWindowManager<::GLFWwindow, CMentalOGLRenderer<CMentalOGLRenderPipeline>>;

} // namespace mentalsdk