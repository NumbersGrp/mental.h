#include <memory>
#include <vector>
#include <string>
#include <array>
#include <functional>
#include <map>

// Forward declaration для GLFW
struct GLFWwindow;

// Forward declaration для Lua
extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

namespace mentalsdk {

// ============================================================================
// Global GLFW window pointer for input handling
// ============================================================================

extern GLFWwindow* g_current_window;

// ============================================================================
// Forward declarations
// ============================================================================

class CMentalShaderMaterial;
class CMentalObject;
class CMentalLuaScript;
class CMentalCamera;
class CMentalRootNode;

// ============================================================================
// Lua Script Class
// ============================================================================

class CMentalLuaScript
{
private:
    lua_State* lua_state_;
    std::string script_path_;
    bool is_loaded_;
    CMentalObject* bound_object_;
    CMentalCamera* bound_camera_;
    
    // Callback functions
    std::function<void()> init_callback_;
    std::function<void(float)> update_callback_;
    std::function<void()> render_callback_;
    
public:
    CMentalLuaScript();
    ~CMentalLuaScript();
    
    CMentalLuaScript(const CMentalLuaScript&) = delete;
    CMentalLuaScript& operator=(const CMentalLuaScript&) = delete;
    CMentalLuaScript(CMentalLuaScript&&) = delete;
    CMentalLuaScript& operator=(CMentalLuaScript&&) = delete;
    
    // Script loading
    bool LoadFromFile(const std::string& script_path);
    bool LoadFromString(const std::string& script_content);
    void Reload();
    
    // Object binding
    void BindToObject(CMentalObject* object);
    CMentalObject* GetBoundObject() const { return bound_object_; }
    
    // Camera binding
    void BindToCamera(CMentalCamera* camera);
    CMentalCamera* GetBoundCamera() const { return bound_camera_; }
    
    // Script execution
    void CallInit();
    void CallUpdate(float delta_time);
    void CallRender();
    bool CallFunction(const std::string& function_name);
    
    // State
    bool IsLoaded() const { return is_loaded_; }
    const std::string& GetScriptPath() const { return script_path_; }
    lua_State* GetLuaState() const { return lua_state_; }
    
private:
    void InitializeLuaState();
    void RegisterLuaAPI();
    void CleanupLuaState();
    bool ExecuteScript(const std::string& script_content);
    
    // Static Lua API functions
    static int lua_setPosition(lua_State* L);
    static int lua_getPosition(lua_State* L);
    static int lua_setRotation(lua_State* L);
    static int lua_getRotation(lua_State* L);
    static int lua_setScale(lua_State* L);
    static int lua_getScale(lua_State* L);
    static int lua_setColor(lua_State* L);
    static int lua_getColor(lua_State* L);
    static int lua_setVisible(lua_State* L);
    static int lua_isVisible(lua_State* L);
    static int lua_getTime(lua_State* L);
    static int lua_sin(lua_State* L);
    static int lua_cos(lua_State* L);
    static int lua_print(lua_State* L);
    
    // Camera API functions
    static int lua_setCameraPosition(lua_State* L);
    static int lua_getCameraPosition(lua_State* L);
    static int lua_setCameraTarget(lua_State* L);
    static int lua_getCameraTarget(lua_State* L);
    static int lua_setCameraUp(lua_State* L);
    static int lua_getCameraUp(lua_State* L);
    static int lua_lookAt(lua_State* L);
    static int lua_setPerspective(lua_State* L);
    static int lua_setFOV(lua_State* L);
    static int lua_moveForward(lua_State* L);
    static int lua_moveBackward(lua_State* L);
    static int lua_moveLeft(lua_State* L);
    static int lua_moveRight(lua_State* L);
    static int lua_moveUp(lua_State* L);
    static int lua_moveDown(lua_State* L);
    static int lua_rotateYaw(lua_State* L);
    static int lua_rotatePitch(lua_State* L);
    static int lua_rotateRoll(lua_State* L);
    
    // Input functions
    static int lua_isKeyPressed(lua_State* L);
    static int lua_KEY_UP(lua_State* L);
    static int lua_KEY_DOWN(lua_State* L);
    static int lua_KEY_LEFT(lua_State* L);
    static int lua_KEY_RIGHT(lua_State* L);
    static int lua_KEY_W(lua_State* L);
    static int lua_KEY_A(lua_State* L);
    static int lua_KEY_S(lua_State* L);
    static int lua_KEY_D(lua_State* L);
    
    // Mouse functions
    static int lua_getMousePosition(lua_State* L);
    static int lua_getMouseDelta(lua_State* L);
    static int lua_isMouseButtonPressed(lua_State* L);
    static int lua_MOUSE_LEFT(lua_State* L);
    static int lua_MOUSE_RIGHT(lua_State* L);
    static int lua_MOUSE_MIDDLE(lua_State* L);
    // Root node functions
    static int lua_createObject(lua_State* L);
    static int lua_createShader(lua_State* L);
    static int lua_attachShaderToObject(lua_State* L);
    static int lua_attachScriptToObject(lua_State* L);
    static int lua_getObjectByName(lua_State* L);
    static int lua_removeObject(lua_State* L);
    static int lua_listObjects(lua_State* L);
    static int lua_reloadScript(lua_State* L);
    static int lua_clearScene(lua_State* L);
    
    // Object manipulation functions
    static int lua_setObjectPosition(lua_State* L);
    static int lua_getObjectPosition(lua_State* L);
    static int lua_setObjectRotation(lua_State* L);
    static int lua_getObjectRotation(lua_State* L);
    static int lua_setObjectScale(lua_State* L);
    static int lua_getObjectScale(lua_State* L);
    static int lua_setObjectColor(lua_State* L);
    static int lua_getObjectColor(lua_State* L);
    static int lua_setObjectVisible(lua_State* L);
    static int lua_isObjectVisible(lua_State* L);
};

// ============================================================================
// Root Node Class - Scene Management
// ============================================================================

class CMentalRootNode
{
private:
    std::shared_ptr<CMentalLuaScript> lua_script_;
    std::vector<std::shared_ptr<CMentalObject>> objects_;
    std::vector<std::shared_ptr<CMentalShaderMaterial>> shaders_;
    std::shared_ptr<CMentalCamera> camera_;
    std::map<std::string, std::shared_ptr<CMentalObject>> named_objects_;
    std::map<std::string, std::shared_ptr<CMentalShaderMaterial>> named_shaders_;
    
    // Hot reload
    bool hot_reload_enabled_;
    std::string script_path_;
    time_t last_script_time_;
    
public:
    CMentalRootNode();
    ~CMentalRootNode() = default;
    
    CMentalRootNode(const CMentalRootNode&) = delete;
    CMentalRootNode& operator=(const CMentalRootNode&) = delete;
    CMentalRootNode(CMentalRootNode&&) = delete;
    CMentalRootNode& operator=(CMentalRootNode&&) = delete;
    
    // Lua script management
    void SetLuaScript(std::shared_ptr<CMentalLuaScript> script);
    void LoadLuaScript(const std::string& script_path);
    std::shared_ptr<CMentalLuaScript> GetLuaScript() const { return lua_script_; }
    
    // Object management
    std::shared_ptr<CMentalObject> CreateObject(const std::string& name, const std::string& type);
    std::shared_ptr<CMentalShaderMaterial> CreateShader(const std::string& name, 
                                                        const std::string& vertex_path, 
                                                        const std::string& fragment_path);
    bool AttachShaderToObject(const std::string& object_name, const std::string& shader_name);
    bool AttachScriptToObject(const std::string& object_name, const std::string& script_path);
    
    // Object access
    std::shared_ptr<CMentalObject> GetObjectByName(const std::string& name);
    std::shared_ptr<CMentalShaderMaterial> GetShaderByName(const std::string& name);
    bool RemoveObject(const std::string& name);
    
    // Camera management
    void SetCamera(std::shared_ptr<CMentalCamera> camera);
    std::shared_ptr<CMentalCamera> GetCamera() const { return camera_; }
    
    // Scene management
    void Update(float delta_time);
    void Render() const;
    void ReloadScript();
    void ClearScene();
    
    // Hot reload
    void EnableHotReload(bool enable = true) { hot_reload_enabled_ = enable; }
    bool IsHotReloadEnabled() const { return hot_reload_enabled_; }
    
    // Getters
    const std::vector<std::shared_ptr<CMentalObject>>& GetObjects() const { return objects_; }
    const std::vector<std::shared_ptr<CMentalShaderMaterial>>& GetShaders() const { return shaders_; }
    size_t GetObjectCount() const { return objects_.size(); }
    size_t GetShaderCount() const { return shaders_.size(); }
    
    // Debug
    void ListObjects() const;
};

// ============================================================================
// Camera Class
// ============================================================================

class CMentalCamera
{
private:
    std::shared_ptr<CMentalLuaScript> lua_script_;
    
    // Camera transform
    std::array<float, 3> position_;
    std::array<float, 3> target_;
    std::array<float, 3> up_;
    
    // Camera parameters
    float fov_;
    float aspect_ratio_;
    float near_plane_;
    float far_plane_;
    
    // View and projection matrices (stored as arrays for simplicity)
    std::array<float, 16> view_matrix_;
    std::array<float, 16> projection_matrix_;
    std::array<float, 16> view_projection_matrix_;
    
    bool matrices_dirty_;
    
public:
    CMentalCamera();
    ~CMentalCamera() = default;
    
    CMentalCamera(const CMentalCamera&) = delete;
    CMentalCamera& operator=(const CMentalCamera&) = delete;
    CMentalCamera(CMentalCamera&&) = delete;
    CMentalCamera& operator=(CMentalCamera&&) = delete;
    
    // Lua script management
    void SetLuaScript(std::shared_ptr<CMentalLuaScript> script);
    void LoadLuaScript(const std::string& script_path);
    std::shared_ptr<CMentalLuaScript> GetLuaScript() const { return lua_script_; }
    
    // Camera transform
    void SetPosition(float x, float y, float z);
    void SetTarget(float x, float y, float z);
    void SetUp(float x, float y, float z);
    void LookAt(float eye_x, float eye_y, float eye_z, 
                float target_x, float target_y, float target_z,
                float up_x = 0.0f, float up_y = 1.0f, float up_z = 0.0f);
    
    // Camera parameters
    void SetPerspective(float fov, float aspect_ratio, float near_plane, float far_plane);
    void SetFOV(float fov);
    void SetAspectRatio(float aspect_ratio);
    void SetNearFar(float near_plane, float far_plane);
    
    // Movement
    void MoveForward(float distance);
    void MoveBackward(float distance);
    void MoveLeft(float distance);
    void MoveRight(float distance);
    void MoveUp(float distance);
    void MoveDown(float distance);
    
    // Rotation
    void RotateYaw(float angle);   // Поворот вокруг Y (влево-вправо)
    void RotatePitch(float angle); // Поворот вокруг X (вверх-вниз)
    void RotateRoll(float angle);  // Поворот вокруг Z (наклон)
    
    // Getters
    const std::array<float, 3>& GetPosition() const { return position_; }
    const std::array<float, 3>& GetTarget() const { return target_; }
    const std::array<float, 3>& GetUp() const { return up_; }
    float GetFOV() const { return fov_; }
    float GetAspectRatio() const { return aspect_ratio_; }
    float GetNearPlane() const { return near_plane_; }
    float GetFarPlane() const { return far_plane_; }
    
    // Matrix access
    const std::array<float, 16>& GetViewMatrix();
    const std::array<float, 16>& GetProjectionMatrix();
    const std::array<float, 16>& GetViewProjectionMatrix();
    
    // Update
    void Update(float delta_time);
    
private:
    void UpdateMatrices();
    void CalculateViewMatrix();
    void CalculateProjectionMatrix();
    std::array<float, 3> GetForwardVector() const;
    std::array<float, 3> GetRightVector() const;
    std::array<float, 3> CrossProduct(const std::array<float, 3>& a, const std::array<float, 3>& b) const;
    std::array<float, 3> Normalize(const std::array<float, 3>& v) const;
    float DotProduct(const std::array<float, 3>& a, const std::array<float, 3>& b) const;
};

// ============================================================================
// Shader Material Class
// ============================================================================

class CMentalShaderMaterial
{
private:
    unsigned int shader_program_;
    unsigned int vertex_shader_;
    unsigned int fragment_shader_;
    bool is_compiled_;
    
public:
    CMentalShaderMaterial();
    ~CMentalShaderMaterial();
    
    CMentalShaderMaterial(const CMentalShaderMaterial&) = delete;
    CMentalShaderMaterial& operator=(const CMentalShaderMaterial&) = delete;
    CMentalShaderMaterial(CMentalShaderMaterial&&) = delete;
    CMentalShaderMaterial& operator=(CMentalShaderMaterial&&) = delete;
    
    // Shader compilation
    bool CompileFromSource(const std::string& vertex_source, const std::string& fragment_source);
    bool CompileFromFiles(const std::string& vertex_path, const std::string& fragment_path);
    
    // Shader usage
    void Use() const;
    void Unuse() const;
    
    // Uniform setters
    void SetUniform(const std::string& name, float value) const;
    void SetUniform(const std::string& name, int value) const;
    void SetUniform(const std::string& name, const std::array<float, 3>& value) const;
    void SetUniform(const std::string& name, const std::array<float, 4>& value) const;
    
    // Matrix uniforms
    void SetUniformMatrix4(const std::string& name, const std::array<float, 16>& matrix) const;
    
    // Getters
    unsigned int GetProgramId() const { return shader_program_; }
    bool IsCompiled() const { return is_compiled_; }
    
private:
    unsigned int CompileShader(const std::string& source, unsigned int type);
    bool LinkProgram();
    void CheckCompileErrors(unsigned int shader, const std::string& type);
};

// ============================================================================
// Object Class
// ============================================================================

class CMentalObject
{
private:
    unsigned int VAO_;
    unsigned int VBO_;
    unsigned int EBO_;
    std::shared_ptr<CMentalShaderMaterial> material_;
    std::shared_ptr<CMentalLuaScript> lua_script_;
    
    // Transform data
    std::array<float, 3> position_;
    std::array<float, 3> rotation_;
    std::array<float, 3> scale_;
    std::array<float, 4> color_;
    
    bool is_initialized_;
    bool is_visible_;
    bool is_triangle_; // Flag to distinguish between triangle and rectangle
    
public:
    CMentalObject();
    ~CMentalObject();
    
    CMentalObject(const CMentalObject&) = delete;
    CMentalObject& operator=(const CMentalObject&) = delete;
    CMentalObject(CMentalObject&&) = delete;
    CMentalObject& operator=(CMentalObject&&) = delete;
    
    // Initialization
    bool InitializeRectangle();
    bool InitializeTriangle();
    void SetMaterial(std::shared_ptr<CMentalShaderMaterial> material);
    
    // Lua script management
    void SetLuaScript(std::shared_ptr<CMentalLuaScript> script);
    void LoadLuaScript(const std::string& script_path);
    std::shared_ptr<CMentalLuaScript> GetLuaScript() const { return lua_script_; }
    
    // Transform methods
    void SetPosition(float x, float y, float z = 0.0f);
    void SetRotation(float x, float y, float z);
    void SetScale(float x, float y, float z = 1.0f);
    void SetColor(float r, float g, float b, float a = 1.0f);
    void SetVisible(bool visible) { is_visible_ = visible; }
    
    // Getters
    const std::array<float, 3>& GetPosition() const { return position_; }
    const std::array<float, 3>& GetRotation() const { return rotation_; }
    const std::array<float, 3>& GetScale() const { return scale_; }
    const std::array<float, 4>& GetColor() const { return color_; }
    bool IsVisible() const { return is_visible_; }
    std::shared_ptr<CMentalShaderMaterial> GetMaterial() const { return material_; }
    
    // Lifecycle methods
    void Update(float delta_time);
    void Render() const;
    
    // State
    bool IsInitialized() const { return is_initialized_; }
    
private:
    void CreateRectangleGeometry();
    void CreateTriangleGeometry();
    void UpdateUniforms() const;
};

class CMentalOGLRenderPipeline
{
private:
    std::shared_ptr<CMentalRootNode> root_node_;
    
public:
    CMentalOGLRenderPipeline();
    ~CMentalOGLRenderPipeline() = default;

    CMentalOGLRenderPipeline(const CMentalOGLRenderPipeline&) = delete;
    CMentalOGLRenderPipeline& operator=(const CMentalOGLRenderPipeline&) = delete;
    CMentalOGLRenderPipeline(CMentalOGLRenderPipeline&&) = delete;
    CMentalOGLRenderPipeline& operator=(CMentalOGLRenderPipeline&&) = delete;
    
    // Initialize OpenGL after context creation
    void InitializeOpenGL();
    
    // Root node management
    void SetRootNode(std::shared_ptr<CMentalRootNode> root_node);
    std::shared_ptr<CMentalRootNode> GetRootNode() const { return root_node_; }
    
    // Legacy object management (for backward compatibility)
    void AddObject(std::shared_ptr<CMentalObject> object);
    void RemoveObject(std::shared_ptr<CMentalObject> object);
    void ClearObjects();
    
    // Camera management
    void SetCamera(std::shared_ptr<CMentalCamera> camera);
    std::shared_ptr<CMentalCamera> GetCamera() const;
    
    // Update and rendering
    void UpdateAll(float delta_time);
    void RenderAll() const;
    
    // Getters
    size_t GetObjectCount() const;
};

template <class P = CMentalOGLRenderPipeline>
class CMentalOGLRenderer
{
private:
    std::unique_ptr<P> up_renderpipeline_ = std::make_unique<P>();
public:
    CMentalOGLRenderer();
    ~CMentalOGLRenderer() = default;

    CMentalOGLRenderer(const CMentalOGLRenderer&) = delete;
    CMentalOGLRenderer& operator=(const CMentalOGLRenderer&) = delete;
    CMentalOGLRenderer(CMentalOGLRenderer&&) = delete;
    CMentalOGLRenderer& operator=(CMentalOGLRenderer&&) = delete;
    
    // Initialize OpenGL after context creation
    void InitializeOpenGL();
    
    // Get render pipeline
    P* GetRenderPipeline() { return up_renderpipeline_.get(); }
    const P* GetRenderPipeline() const { return up_renderpipeline_.get(); }
};


template<typename T = ::GLFWwindow, class R = CMentalOGLRenderer<>>
class CMentalWindowManager
{
private:
    std::unique_ptr<T, void(*)(T*)> up_window_;
    std::unique_ptr<R> up_renderer_ = std::make_unique<R>();
public:
    CMentalWindowManager();
    ~CMentalWindowManager();
    
    CMentalWindowManager(const CMentalWindowManager&) = delete;
    CMentalWindowManager& operator=(const CMentalWindowManager&) = delete;
    CMentalWindowManager(CMentalWindowManager&&) = delete;
    CMentalWindowManager& operator=(CMentalWindowManager&&) = delete;
    
    // Utility methods
    bool ShouldClose() const;
    void PollEvents();
    void SwapBuffers();
    void Clear(float r = 0.2f, float g = 0.3f, float b = 0.3f, float a = 1.0f);
    void SetTitle(const std::string& title);
    void GetWindowSize(int& width, int& height) const;
    
    // Get renderer
    R* GetRenderer() { return up_renderer_.get(); }
    const R* GetRenderer() const { return up_renderer_.get(); }
};
}