/**
 * @file mental.cpp
 * @brief Mental Game Engine with FBX Model Support
 * @author Mental SDK Team
 * @version 1.0.0
 */
#include "../SDK/sdk.hpp"
#include <iostream>
#include <memory>
#include <chrono>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

/**
 * @brief Configuration loader for game parameters
 */
class ConfigLoader {
private:
    std::unordered_map<std::string, float> floatParams_;
    std::unordered_map<std::string, std::string> stringParams_;

public:
    bool loadConfig(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open config file: " << filename << '\n';
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Parse key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                // Try to parse as float first
                try {
                    float floatValue = std::stof(value);
                    floatParams_[key] = floatValue;
                    std::cout << "Loaded float parameter: " << key << " = " << floatValue << '\n';
                } catch (const std::exception&) {
                    // If not a float, store as string
                    stringParams_[key] = value;
                    std::cout << "Loaded string parameter: " << key << " = " << value << '\n';
                }
            }
        }
        
        return true;
    }
    
    float getFloat(const std::string& key, float defaultValue = 0.0F) const {
        auto it = floatParams_.find(key);
        return (it != floatParams_.end()) ? it->second : defaultValue;
    }
    
    std::string getString(const std::string& key, const std::string& defaultValue = "") const {
        auto it = stringParams_.find(key);
        return (it != stringParams_.end()) ? it->second : defaultValue;
    }
};

/**
 * @brief Texture manager for different model types
 */
class TextureManager {
public:
    /**
     * @brief Get texture path for a specific material and model
     * @param modelPath Path to the model file
     * @param materialName Name of the material
     * @return Path to texture file or empty string if not found
     */
    static std::string getTexturePathForMaterial(const std::string& modelPath, const std::string& materialName) {
        // Mario OBJ model
        if (modelPath.find("mario-obj") != std::string::npos || modelPath.find("Mario.obj") != std::string::npos) {
            std::string basePath = "common/FBX Assets/mario-obj/textures/";
            
            // Mario OBJ has submesh materials, map by material name
            if (materialName == "submesh_0") {
                return basePath + "0x39b36443.png"; // First texture
            } else if (materialName == "submesh_1") {
                return basePath + "0x3b7d69bc.png"; // Second texture
            } else if (materialName == "submesh_2") {
                return basePath + "0x4c506d2f.png"; // Third texture
            } else if (materialName == "submesh_3") {
                return basePath + "0x54faa61.png"; // Fourth texture
            } else if (materialName == "submesh_4") {
                return basePath + "0x73e72eb1.png"; // Fifth texture
            } else if (materialName == "submesh_5") {
                return basePath + "0x95852189.png"; // Sixth texture
            } else if (materialName == "submesh_6") {
                return basePath + "0x96b71089.png"; // Seventh texture
            } else if (materialName == "submesh_7") {
                return basePath + "0xbc4e9ae5.png"; // Eighth texture
            } else {
                // Default texture for unknown materials
                return basePath + "0x39b36443.png"; // Default to first texture
            }
        }
        
        // Cube model (simple cube)
        if (modelPath.find("cube") != std::string::npos && modelPath.find("rubiks") == std::string::npos) {
            return "common/FBX Assets/cube/texture.png";
        }
        
        // Rubik's Cube model
        if (modelPath.find("rubiks-cube") != std::string::npos || modelPath.find("rubiks") != std::string::npos) {
            return "common/FBX Assets/rubiks-cube/textures/diffuse.png";
        }
        
        // Steve-rig model
        if (modelPath.find("steve") != std::string::npos) {
            std::string basePath = "common/FBX Assets/steve-rig/textures/";
            
            if (materialName.find("skin") != std::string::npos || 
                materialName.find("Skin") != std::string::npos ||
                materialName.empty()) {
                return basePath + "diffuse.jpeg";
            }
            
            return basePath + "skin.jpg"; // fallback
        }
        
        // Mococo-abyssgard model
        if (modelPath.find("mococo") != std::string::npos) {
            std::string basePath = "common/FBX Assets/mococo-abyssgard/textures/";
            
            if (materialName.find("Skin") != std::string::npos) {
                return basePath + "Skin_Base_color.png";
            } else if (materialName.find("Hair") != std::string::npos) {
                return basePath + "Hair_Base_color.png";
            } else if (materialName.find("jacket") != std::string::npos) {
                return basePath + "jacket_Base_color.png";
            } else if (materialName.find("accessories") != std::string::npos) {
                return basePath + "accessories_Base_color.png";
            } else if (materialName.find("Shoes") != std::string::npos) {
                return basePath + "Shoes_Base_color.png";
            } else if (materialName.find("inner") != std::string::npos) {
                return basePath + "inner_Base_color.png";
            } else if (materialName.find("Common_acc") != std::string::npos || 
                       materialName.find("Common_Acc") != std::string::npos) {
                return basePath + "Common_Acc_Base_color.png";
            }
            
            return basePath + "Skin_Base_color.png"; // fallback
        }
        
        // Unknown model - no texture
        return "";
    }
};

/**
 * @brief Enhanced Game Engine with FBX support and Camera Control
 */
class MentalGameEngine {
private:
    std::unique_ptr<mentalsdk::CMentalWindowManager<mentalsdk::GLFWwindow, mentalsdk::CMentalRenderer<mentalsdk::CMentalRenderPipeline<>>>> windowManager_;
    std::unique_ptr<mentalsdk::CMentalTriangle> triangle_;
    std::unique_ptr<mentalsdk::CMentalOBJModel> objModel_;
    std::unique_ptr<mentalsdk::CMentalFBXModel> fbxModel_;
    std::unique_ptr<mentalsdk::CMentalShader> triangleShader_;
    std::unique_ptr<mentalsdk::CMentalShader> fbxShader_;
    std::unique_ptr<mentalsdk::CMentalShader> rubiksCubeShader_;
    std::unique_ptr<mentalsdk::CMentalCamera> camera_;
    std::unique_ptr<mentalsdk::CMentalInput> input_;
    std::unique_ptr<mentalsdk::CMentalImGui> imgui_;
    std::unique_ptr<mentalsdk::CMentalCameraController> cameraController_;
    
    ConfigLoader config_;
    std::chrono::high_resolution_clock::time_point lastTime_;
    float totalTime_ = 0.0f;
    bool showTriangle_ = true;
    bool showOBJModel_ = false;
    bool showFBXModel_ = false;
    
    // UI state
    bool showImGuiDemo_ = false;
    bool showDebugWindow_ = false;
    bool showUnityInterface_ = true;
    
    // Unity-style panels
    bool showHierarchy_ = true;
    bool showInspector_ = true;
    bool showScene_ = true;
    bool showGame_ = true;
    bool showProject_ = true;
    bool showConsole_ = true;

public:
    bool initialize() {
        try {
            // Load configuration
            config_.loadConfig("common/Scripts/fbx_demo.txt");
            
            // Initialize window
            mentalsdk::WindowManagerInfo info{};
            info.aSizes[0] = 1200;
            info.aSizes[1] = 800;
            info.title = "Mental Game Engine - FBX Model Viewer with Camera Control";
            
            windowManager_ = std::make_unique<mentalsdk::CMentalWindowManager<mentalsdk::GLFWwindow, mentalsdk::CMentalRenderer<mentalsdk::CMentalRenderPipeline<>>>>(info);
            
            // Initialize camera
            glm::vec3 cameraPos(0.0f, 0.0f, 3.0f); // Closer to cube
            camera_ = std::make_unique<mentalsdk::CMentalCamera>(cameraPos);
            camera_->setMovementSpeed(config_.getFloat("camera_speed", 3.0f));
            camera_->setMouseSensitivity(config_.getFloat("mouse_sensitivity", 0.15f));
            
            // Initialize input system
            input_ = std::make_unique<mentalsdk::CMentalInput>(windowManager_->getWindow());
            input_->initialize();
            
            // Initialize ImGui
            imgui_ = std::make_unique<mentalsdk::CMentalImGui>();
            if (!imgui_->initialize(windowManager_->getWindow())) {
                std::cerr << "Failed to initialize ImGui!" << std::endl;
                return false;
            }
            
            // Initialize camera controller
            cameraController_ = std::make_unique<mentalsdk::CMentalCameraController>(camera_.get(), input_.get());
            cameraController_->init();
            
            // Create triangle (fallback)
            triangle_ = std::make_unique<mentalsdk::CMentalTriangle>("FallbackTriangle");
            triangle_->initialize();
            
            // Create OBJ model
            objModel_ = std::make_unique<mentalsdk::CMentalOBJModel>("TestOBJModel");
            
            // Create FBX model
            fbxModel_ = std::make_unique<mentalsdk::CMentalFBXModel>("TestFBXModel");
            
            // Try to load model
            std::string modelPath = config_.getString("model_path", "common/FBX Assets/test_cube.fbx");
            
            // Check if it's an OBJ file
            if (modelPath.find(".obj") != std::string::npos) {
                if (objModel_->loadFromFile(modelPath)) {
                    showOBJModel_ = true;
                    showTriangle_ = false;
                    std::cout << "OBJ model loaded successfully!" << std::endl;
                    
                    // Load textures for the model
                    loadTexturesForOBJModel();
                } else {
                    std::cout << "Failed to load OBJ model, trying FBX fallback." << std::endl;
                }
            }
            
            // If OBJ failed or it's an FBX file, try FBX
            if (!showOBJModel_) {
                if (fbxModel_->loadFromFile(modelPath)) {
                    showFBXModel_ = true;
                    showTriangle_ = false;
                    std::cout << "FBX model loaded successfully!" << std::endl;
                    
                    // Load textures for the model
                    loadTexturesForFBXModel();
                } else {
                    std::cout << "Failed to load model, using triangle fallback." << std::endl;
                    showTriangle_ = true;
                }
            }
            
            // Load shaders
            triangleShader_ = std::make_unique<mentalsdk::CMentalShader>(
                "common/Shaders/triangle.vert",
                "common/Shaders/triangle.frag"
            );
            
            fbxShader_ = std::make_unique<mentalsdk::CMentalShader>(
                "common/Shaders/fbx_model.vert",
                "common/Shaders/fbx_model.frag"
            );
            
            // Load Rubik's cube shader
            rubiksCubeShader_ = std::make_unique<mentalsdk::CMentalShader>(
                "common/Shaders/rubiks_cube.vert",
                "common/Shaders/rubiks_cube.frag"
            );
            
            // Setup OpenGL state
            glEnable(GL_DEPTH_TEST);
            // Disable face culling completely to show all triangles
            glDisable(GL_CULL_FACE);
            
            lastTime_ = std::chrono::high_resolution_clock::now();
            
            std::cout << "Mental Game Engine initialized successfully!" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Failed to initialize game engine: " << e.what() << std::endl;
            return false;
        }
    }
    
    void run() {
        if (!windowManager_) {
            std::cerr << "Game engine not initialized!" << std::endl;
            return;
        }
        
        std::cout << "Starting Mental Game Engine with Unity-style interface..." << std::endl;
        std::cout << "Camera Controls:" << std::endl;
        std::cout << "  WASD - Move camera" << std::endl;
        std::cout << "  Hold Left Mouse + Move - Look around" << std::endl;
        std::cout << "  Space - Move up" << std::endl;
        std::cout << "  Left Shift - Move down" << std::endl;
        std::cout << "Interface Controls:" << std::endl;
        std::cout << "  I - Toggle Unity-style interface" << std::endl;
        std::cout << "  G - Toggle debug window" << std::endl;
        std::cout << "  H - Toggle ImGui demo window" << std::endl;
        std::cout << "Rendering Controls:" << std::endl;
        std::cout << "  T - Toggle wireframe mode" << std::endl;
        std::cout << "  U - Toggle textures on/off" << std::endl;
        std::cout << "  F - Toggle texture flip (restart required)" << std::endl;
        std::cout << "  V - Toggle UV debug mode (restart required)" << std::endl;
        std::cout << "  ESC or close window to exit" << std::endl;
        
        while (!windowManager_->shouldClose()) {
            updateTime();
            update();
            render();
            windowManager_->pollEvents();
            windowManager_->swapBuffers();
        }
        
        std::cout << "FBX viewer ended." << std::endl;
    }

private:
    void loadTexturesForOBJModel() {
        if (!objModel_ || !objModel_->isLoaded()) {
            return;
        }
        
        // Get model configuration
        std::string modelPath = config_.getString("model_path", "");
        bool flipTextures = config_.getFloat("texture_flip", 0.0f) > 0.5f; // Convert float to bool
        
        // Load textures based on model type and materials
        for (size_t i = 0; i < objModel_->getMeshCount(); i++) {
            std::string materialName = objModel_->getMaterialName(i);
            std::string texturePath = TextureManager::getTexturePathForMaterial(modelPath, materialName);
            
            if (!texturePath.empty()) {
                if (objModel_->loadTextureForMesh(i, texturePath, flipTextures)) {
                    std::cout << "Loaded texture for OBJ mesh " << i << " (material: " << materialName << "): " << texturePath 
                              << " (flip: " << (flipTextures ? "yes" : "no") << ")" << std::endl;
                } else {
                    std::cout << "Failed to load texture for OBJ mesh " << i << ": " << texturePath << std::endl;
                }
            }
        }
    }
    
    void loadTexturesForFBXModel() {
        if (!fbxModel_ || !fbxModel_->isLoaded()) {
            return;
        }
        
        // Get model configuration
        std::string modelPath = config_.getString("model_path", "");
        bool flipTextures = config_.getFloat("texture_flip", 0.0f) > 0.5f; // Convert float to bool
        
        // Load textures based on model type and materials
        for (size_t i = 0; i < fbxModel_->getMeshCount(); i++) {
            std::string materialName = fbxModel_->getMaterialName(i);
            std::string texturePath = TextureManager::getTexturePathForMaterial(modelPath, materialName);
            
            if (!texturePath.empty()) {
                if (fbxModel_->loadTextureForMesh(i, texturePath, flipTextures)) {
                    std::cout << "Loaded texture for FBX mesh " << i << " (material: " << materialName << "): " << texturePath 
                              << " (flip: " << (flipTextures ? "yes" : "no") << ")" << std::endl;
                } else {
                    std::cout << "Failed to load texture for FBX mesh " << i << ": " << texturePath << std::endl;
                }
            }
        }
    }
    
    void updateTime() {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime_).count();
        lastTime_ = currentTime;
        totalTime_ += deltaTime;
        
        // Update input and camera
        if (input_) {
            input_->update();
        }
        if (cameraController_) {
            cameraController_->update(deltaTime);
        }
    }
    
    void update() {
        // Handle ESC key to exit
        if (input_ && input_->isKeyPressed(GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(windowManager_->getWindow(), GLFW_TRUE);
        }
        
        // Toggle wireframe mode with 'T' key
        static bool tKeyPressed = false;
        static bool wireframeMode = false; // Start in filled mode
        
        if (input_ && input_->isKeyPressed(GLFW_KEY_T)) {
            if (!tKeyPressed) {
                wireframeMode = !wireframeMode;
                if (wireframeMode) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    std::cout << "Wireframe mode ON" << std::endl;
                } else {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    std::cout << "Wireframe mode OFF" << std::endl;
                }
                tKeyPressed = true;
            }
        } else {
            tKeyPressed = false;
        }
        
        // Toggle texture mode with 'U' key
        static bool uKeyPressed = false;
        static bool useTextures = true;
        
        if (input_ && input_->isKeyPressed(GLFW_KEY_U)) {
            if (!uKeyPressed) {
                useTextures = !useTextures;
                std::cout << "Textures " << (useTextures ? "ON" : "OFF") << std::endl;
                uKeyPressed = true;
            }
        } else {
            uKeyPressed = false;
        }
        
        // Toggle texture flip with 'F' key
        static bool fKeyPressed = false;
        static bool flipTextures = false;
        
        if (input_ && input_->isKeyPressed(GLFW_KEY_F)) {
            if (!fKeyPressed) {
                flipTextures = !flipTextures;
                std::cout << "Texture flip " << (flipTextures ? "ON" : "OFF") << std::endl;
                std::cout << "Note: Restart to apply texture flip changes" << std::endl;
                fKeyPressed = true;
            }
        } else {
            fKeyPressed = false;
        }
        
        // Toggle UV debug mode with 'V' key
        static bool vKeyPressed = false;
        static bool showUVs = false;
        
        if (input_ && input_->isKeyPressed(GLFW_KEY_V)) {
            if (!vKeyPressed) {
                showUVs = !showUVs;
                std::cout << "UV debug mode " << (showUVs ? "ON" : "OFF") << std::endl;
                std::cout << "Note: Restart to apply UV debug changes" << std::endl;
                vKeyPressed = true;
            }
        } else {
            vKeyPressed = false;
        }
        
        // Toggle ImGui debug window with 'G' key
        static bool gKeyPressed = false;
        
        if (input_ && input_->isKeyPressed(GLFW_KEY_G)) {
            if (!gKeyPressed) {
                if (showUnityInterface_) {
                    showUnityInterface_ = false;
                    showDebugWindow_ = true;
                } else {
                    showDebugWindow_ = !showDebugWindow_;
                }
                std::cout << "Debug window " << (showDebugWindow_ ? "ON" : "OFF") << std::endl;
                gKeyPressed = true;
            }
        } else {
            gKeyPressed = false;
        }
        
        // Toggle Unity interface with 'I' key
        static bool iKeyPressed = false;
        
        if (input_ && input_->isKeyPressed(GLFW_KEY_I)) {
            if (!iKeyPressed) {
                showUnityInterface_ = !showUnityInterface_;
                if (showUnityInterface_) {
                    showDebugWindow_ = false;
                }
                std::cout << "Unity interface " << (showUnityInterface_ ? "ON" : "OFF") << std::endl;
                iKeyPressed = true;
            }
        } else {
            iKeyPressed = false;
        }
        
        // Toggle ImGui demo with 'H' key
        static bool hKeyPressed = false;
        
        if (input_ && input_->isKeyPressed(GLFW_KEY_H)) {
            if (!hKeyPressed) {
                showImGuiDemo_ = !showImGuiDemo_;
                std::cout << "ImGui demo " << (showImGuiDemo_ ? "ON" : "OFF") << std::endl;
                hKeyPressed = true;
            }
        } else {
            hKeyPressed = false;
        }
        
        // Store texture mode for rendering
        static bool* textureMode = &useTextures;
        (void)textureMode; // Suppress unused variable warning
    }
    
    void render() {
        // Clear screen
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Setup camera
        int width, height;
        windowManager_->getWindowSize(width, height);
        
        float fov = config_.getFloat("camera_fov", 45.0f);
        glm::mat4 projection = glm::perspective(
            glm::radians(fov), 
            (float)width / (float)height, 
            0.1f, 
            100.0f
        );
        
        glm::mat4 view = camera_->getViewMatrix();
        
        if (showOBJModel_ && objModel_->isLoaded()) {
            renderOBJModel(projection, view);
        } else if (showFBXModel_ && fbxModel_->isLoaded()) {
            renderFBXModel(projection, view);
        } else if (showTriangle_) {
            renderTriangle(projection, view);
        }
        
        // Render ImGui
        renderImGui();
    }
    
    void renderOBJModel(const glm::mat4& projection, const glm::mat4& view) {
        // Use regular FBX shader for OBJ models (not rubiks cube shader)
        fbxShader_->use();
        
        // Model transformation
        glm::mat4 model = glm::mat4(1.0f);
        
        float rotationSpeed = config_.getFloat("rotation_speed", 0.0f);
        float modelScale = config_.getFloat("model_scale", 1.0f);
        
        if (rotationSpeed > 0.0f) {
            model = glm::rotate(model, totalTime_ * rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        model = glm::scale(model, glm::vec3(modelScale));
        
        // Debug: Print model info once
        static bool debugPrinted = false;
        if (!debugPrinted) {
            std::cout << "OBJ Model scale: " << modelScale << std::endl;
            std::cout << "Camera position: " << camera_->getPosition().x << ", " 
                      << camera_->getPosition().y << ", " << camera_->getPosition().z << std::endl;
            debugPrinted = true;
        }
        
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        
        // Set transformation matrices
        fbxShader_->setMat4("uModel", model);
        fbxShader_->setMat4("uView", view);
        fbxShader_->setMat4("uProjection", projection);
        fbxShader_->setMat3("uNormalMatrix", normalMatrix);
        
        // Set lighting uniforms
        glm::vec3 lightPos(
            config_.getFloat("light_pos_x", 2.0f),
            config_.getFloat("light_pos_y", 2.0f),
            config_.getFloat("light_pos_z", 2.0f)
        );
        
        glm::vec3 lightColor(
            config_.getFloat("light_color_r", 1.0f),
            config_.getFloat("light_color_g", 1.0f),
            config_.getFloat("light_color_b", 1.0f)
        );
        
        glm::vec3 objectColor(
            config_.getFloat("object_color_r", 0.8f),
            config_.getFloat("object_color_g", 0.6f),
            config_.getFloat("object_color_b", 0.4f)
        );
        
        // Set lighting uniforms
        fbxShader_->setVec3("uLightPos", lightPos);
        fbxShader_->setVec3("uLightColor", lightColor);
        fbxShader_->setVec3("uViewPos", camera_->getPosition());
        fbxShader_->setVec3("uObjectColor", objectColor);
        fbxShader_->setFloat("uTime", totalTime_);
        
        // Material properties
        fbxShader_->setFloat("uAmbientStrength", config_.getFloat("ambient_strength", 0.3f));
        fbxShader_->setFloat("uSpecularStrength", config_.getFloat("specular_strength", 0.5f));
        fbxShader_->setInt("uShininess", static_cast<int>(config_.getFloat("shininess", 32.0f)));
        
        // Texture settings - use actual texture, not procedural colors
        fbxShader_->setInt("uTexture", 0); // Texture unit 0
        fbxShader_->setBool("uUseTexture", true);
        
        // Render OBJ model
        objModel_->render();
    }
    
    void renderFBXModel(const glm::mat4& projection, const glm::mat4& view) {
        // Check if this is a Rubik's cube model
        std::string modelPath = config_.getString("model_path", "");
        bool isRubiksCube = (modelPath.find("rubiks-cube") != std::string::npos || modelPath.find("cube") != std::string::npos);
        
        // Use appropriate shader
        if (isRubiksCube && rubiksCubeShader_) {
            rubiksCubeShader_->use();
        } else {
            fbxShader_->use();
        }
        
        // Model transformation
        glm::mat4 model = glm::mat4(1.0f);
        
        float rotationSpeed = config_.getFloat("rotation_speed", 0.0f);
        float modelScale = config_.getFloat("model_scale", 1.0f);
        
        if (rotationSpeed > 0.0f) {
            model = glm::rotate(model, totalTime_ * rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        model = glm::scale(model, glm::vec3(modelScale));
        
        // Debug: Print model info once
        static bool debugPrinted = false;
        if (!debugPrinted) {
            std::cout << "Model scale: " << modelScale << std::endl;
            std::cout << "Camera position: " << camera_->getPosition().x << ", " 
                      << camera_->getPosition().y << ", " << camera_->getPosition().z << std::endl;
            debugPrinted = true;
        }
        
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        
        // Set transformation matrices for both shaders
        if (isRubiksCube && rubiksCubeShader_) {
            rubiksCubeShader_->setMat4("uModel", model);
            rubiksCubeShader_->setMat4("uView", view);
            rubiksCubeShader_->setMat4("uProjection", projection);
            rubiksCubeShader_->setMat3("uNormalMatrix", normalMatrix);
        } else {
            fbxShader_->setMat4("uModel", model);
            fbxShader_->setMat4("uView", view);
            fbxShader_->setMat4("uProjection", projection);
            fbxShader_->setMat3("uNormalMatrix", normalMatrix);
        }
        
        // Set lighting uniforms
        glm::vec3 lightPos(
            config_.getFloat("light_pos_x", 2.0f),
            config_.getFloat("light_pos_y", 2.0f),
            config_.getFloat("light_pos_z", 2.0f)
        );
        
        glm::vec3 lightColor(
            config_.getFloat("light_color_r", 1.0f),
            config_.getFloat("light_color_g", 1.0f),
            config_.getFloat("light_color_b", 1.0f)
        );
        
        glm::vec3 objectColor(
            config_.getFloat("object_color_r", 0.8f),
            config_.getFloat("object_color_g", 0.6f),
            config_.getFloat("object_color_b", 0.4f)
        );
        
        // Set lighting uniforms for both shaders
        if (isRubiksCube && rubiksCubeShader_) {
            rubiksCubeShader_->setVec3("uLightPos", lightPos);
            rubiksCubeShader_->setVec3("uLightColor", lightColor);
            rubiksCubeShader_->setVec3("uViewPos", camera_->getPosition());
            rubiksCubeShader_->setVec3("uObjectColor", objectColor);
            rubiksCubeShader_->setFloat("uTime", totalTime_);
            
            // Material properties
            rubiksCubeShader_->setFloat("uAmbientStrength", config_.getFloat("ambient_strength", 0.3f));
            rubiksCubeShader_->setFloat("uSpecularStrength", config_.getFloat("specular_strength", 0.5f));
            rubiksCubeShader_->setInt("uShininess", static_cast<int>(config_.getFloat("shininess", 32.0f)));
            
            // Texture settings (for Rubik's cube, this enables face-based coloring)
            rubiksCubeShader_->setInt("uTexture", 0); // Texture unit 0
            rubiksCubeShader_->setBool("uUseTexture", true); // Enable face-based coloring
        } else {
            fbxShader_->setVec3("uLightPos", lightPos);
            fbxShader_->setVec3("uLightColor", lightColor);
            fbxShader_->setVec3("uViewPos", camera_->getPosition());
            fbxShader_->setVec3("uObjectColor", objectColor);
            fbxShader_->setFloat("uTime", totalTime_);
            
            // Material properties
            fbxShader_->setFloat("uAmbientStrength", config_.getFloat("ambient_strength", 0.3f));
            fbxShader_->setFloat("uSpecularStrength", config_.getFloat("specular_strength", 0.5f));
            fbxShader_->setInt("uShininess", static_cast<int>(config_.getFloat("shininess", 32.0f)));
            
            // Texture settings
            fbxShader_->setInt("uTexture", 0); // Texture unit 0
            fbxShader_->setBool("uUseTexture", true);
        }
        
        // Render FBX model
        fbxModel_->render();
    }
    
    void renderTriangle(const glm::mat4& projection, const glm::mat4& view) {
        triangleShader_->use();
        
        // Model transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, totalTime_ * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        
        // Set uniforms
        triangleShader_->setMat4("uModel", model);
        triangleShader_->setMat4("uView", view);
        triangleShader_->setMat4("uProjection", projection);
        triangleShader_->setFloat("uTime", totalTime_);
        triangleShader_->setVec3("uTint", glm::vec3(1.0f, 0.8f, 0.6f));
        
        // Render triangle
        triangle_->render();
    }
    
    void renderImGui() {
        if (!imgui_ || !imgui_->isInitialized()) {
            return;
        }
        
        // Start ImGui frame
        imgui_->newFrame();
        
        if (showUnityInterface_) {
            // Unity-style interface
            imgui_->beginDockSpace();
            
            // Show Unity-style panels
            if (showHierarchy_) {
                imgui_->showHierarchyPanel(&showHierarchy_);
            }
            
            if (showInspector_) {
                imgui_->showInspectorPanel(&showInspector_);
            }
            
            if (showScene_) {
                imgui_->showScenePanel(&showScene_);
            }
            
            if (showGame_) {
                imgui_->showGamePanel(&showGame_);
            }
            
            if (showProject_) {
                imgui_->showProjectPanel(&showProject_);
            }
            
            if (showConsole_) {
                imgui_->showConsolePanel(&showConsole_);
            }
            
            imgui_->endDockSpace();
        } else {
            // Original debug interface
            if (showDebugWindow_) {
                imgui_->beginDebugWindow("Mental Engine Debug", &showDebugWindow_);
                
                // Engine info
                imgui_->text("Mental Game Engine v1.0.0");
                imgui_->textf("FPS: %.1f (%.3f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
                
                ImGui::Separator();
                
                // Model info
                if (showOBJModel_ && objModel_->isLoaded()) {
                    imgui_->text("Current Model: OBJ");
                    imgui_->textf("Meshes: %zu", objModel_->getMeshCount());
                } else if (showFBXModel_ && fbxModel_->isLoaded()) {
                    imgui_->text("Current Model: FBX");
                    imgui_->textf("Meshes: %zu", fbxModel_->getMeshCount());
                } else {
                    imgui_->text("Current Model: Triangle (fallback)");
                }
                
                ImGui::Separator();
                
                // Camera controls
                glm::vec3 camPos = camera_->getPosition();
                imgui_->textf("Camera Position: (%.2f, %.2f, %.2f)", camPos.x, camPos.y, camPos.z);
                
                float camSpeed = camera_->getMovementSpeed();
                if (imgui_->sliderFloat("Camera Speed", &camSpeed, 0.1f, 10.0f)) {
                    camera_->setMovementSpeed(camSpeed);
                }
                
                float mouseSens = camera_->getMouseSensitivity();
                if (imgui_->sliderFloat("Mouse Sensitivity", &mouseSens, 0.01f, 1.0f)) {
                    camera_->setMouseSensitivity(mouseSens);
                }
                
                ImGui::Separator();
                
                // Lighting controls
                static float lightPos[3] = {
                    config_.getFloat("light_pos_x", 2.0f),
                    config_.getFloat("light_pos_y", 2.0f),
                    config_.getFloat("light_pos_z", 2.0f)
                };
                
                if (ImGui::SliderFloat3("Light Position", lightPos, -10.0f, 10.0f)) {
                    // Update config (this would need a way to update config in real-time)
                }
                
                static float lightColor[3] = {
                    config_.getFloat("light_color_r", 1.0f),
                    config_.getFloat("light_color_g", 1.0f),
                    config_.getFloat("light_color_b", 1.0f)
                };
                
                imgui_->colorPicker("Light Color", lightColor);
                
                ImGui::Separator();
                
                // UI controls
                imgui_->checkbox("Show ImGui Demo", &showImGuiDemo_);
                imgui_->checkbox("Unity Interface", &showUnityInterface_);
                
                if (imgui_->button("Reset Camera")) {
                    camera_->setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
                    camera_->resetOrientation();
                }
                
                imgui_->endWindow();
            }
        }
        
        // Show ImGui demo window if enabled
        if (showImGuiDemo_) {
            imgui_->showDemoWindow(&showImGuiDemo_);
        }
        
        // Render ImGui
        imgui_->render();
    }
};

int main() {
    std::cout << "=== Mental Game Engine - FBX Model Viewer ===" << std::endl;
    
    MentalGameEngine engine;
    
    if (!engine.initialize()) {
        std::cerr << "Failed to initialize game engine!" << std::endl;
        return -1;
    }
    
    engine.run();
    
    std::cout << "Game engine shutdown complete." << std::endl;
    return 0;
}