/**
 * @file CMentalImGui.cpp
 * @brief Implementation of ImGui wrapper for the Mental SDK
 * @author Mental SDK Team
 * @version 1.0.0
 */

#include "CMentalImGui.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <cstdarg>

namespace mentalsdk
{

CMentalImGui::~CMentalImGui() {
    cleanup();
}

bool CMentalImGui::initialize(GLFWwindow* window) {
    if (initialized_) {
        std::cout << "ImGui already initialized!" << std::endl;
        return true;
    }
    
    if (!window) {
        std::cerr << "Invalid window pointer for ImGui initialization!" << std::endl;
        return false;
    }
    
    window_ = window;
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;        // Enable Multi-Viewport / Platform Windows
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    const char* glsl_version = "#version 330";
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        std::cerr << "Failed to initialize ImGui GLFW backend!" << std::endl;
        return false;
    }
    
    if (!ImGui_ImplOpenGL3_Init(glsl_version)) {
        std::cerr << "Failed to initialize ImGui OpenGL3 backend!" << std::endl;
        ImGui_ImplGlfw_Shutdown();
        return false;
    }
    
    initialized_ = true;
    std::cout << "ImGui initialized successfully!" << std::endl;
    
    return true;
}

void CMentalImGui::newFrame() {
    if (!initialized_) {
        return;
    }
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void CMentalImGui::render() {
    if (!initialized_) {
        return;
    }
    
    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    // Update and Render additional Platform Windows
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void CMentalImGui::cleanup() {
    if (!initialized_) {
        return;
    }
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    initialized_ = false;
    window_ = nullptr;
    
    std::cout << "ImGui cleaned up." << std::endl;
}

void CMentalImGui::showDemoWindow(bool* show) {
    if (!initialized_) {
        return;
    }
    
    if (show && *show) {
        ImGui::ShowDemoWindow(show);
    } else if (!show) {
        static bool showDemo = true;
        ImGui::ShowDemoWindow(&showDemo);
    }
}

void CMentalImGui::beginDebugWindow(const char* title, bool* isOpen) {
    if (!initialized_) {
        return;
    }
    
    ImGui::Begin(title, isOpen);
}

void CMentalImGui::endWindow() {
    if (!initialized_) {
        return;
    }
    
    ImGui::End();
}

void CMentalImGui::textf(const char* format, ...) {
    if (!initialized_) {
        return;
    }
    
    va_list args;
    va_start(args, format);
    ImGui::TextV(format, args);
    va_end(args);
}

void CMentalImGui::text(const char* text) {
    if (!initialized_) {
        return;
    }
    
    ImGui::Text("%s", text);
}

bool CMentalImGui::sliderFloat(const char* label, float* value, float min, float max) {
    if (!initialized_) {
        return false;
    }
    
    return ImGui::SliderFloat(label, value, min, max);
}

bool CMentalImGui::sliderInt(const char* label, int* value, int min, int max) {
    if (!initialized_) {
        return false;
    }
    
    return ImGui::SliderInt(label, value, min, max);
}

bool CMentalImGui::checkbox(const char* label, bool* value) {
    if (!initialized_) {
        return false;
    }
    
    return ImGui::Checkbox(label, value);
}

bool CMentalImGui::button(const char* label) {
    if (!initialized_) {
        return false;
    }
    
    return ImGui::Button(label);
}

bool CMentalImGui::colorPicker(const char* label, float* color, bool hasAlpha) {
    if (!initialized_) {
        return false;
    }
    
    if (hasAlpha) {
        return ImGui::ColorEdit4(label, color);
    } else {
        return ImGui::ColorEdit3(label, color);
    }
}

void CMentalImGui::beginDockSpace() {
    if (!initialized_) {
        return;
    }
    
    static bool dockspaceOpen = true;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);
    
    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    
    // Menu Bar
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene", "Ctrl+N")) {}
            if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {}
            if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("GameObject")) {
            if (ImGui::MenuItem("Create Empty")) {}
            if (ImGui::BeginMenu("3D Object")) {
                if (ImGui::MenuItem("Cube")) {}
                if (ImGui::MenuItem("Sphere")) {}
                if (ImGui::MenuItem("Plane")) {}
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {
            if (ImGui::MenuItem("General", nullptr, false, false)) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Scene")) {}
            if (ImGui::MenuItem("Game")) {}
            if (ImGui::MenuItem("Inspector")) {}
            if (ImGui::MenuItem("Hierarchy")) {}
            if (ImGui::MenuItem("Project")) {}
            if (ImGui::MenuItem("Console")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void CMentalImGui::endDockSpace() {
    if (!initialized_) {
        return;
    }
    
    ImGui::End();
}

void CMentalImGui::showHierarchyPanel(bool* isOpen) {
    if (!initialized_) {
        return;
    }
    
    if (ImGui::Begin("Hierarchy", isOpen)) {
        // Search bar
        static char searchBuffer[256] = "";
        ImGui::SetNextItemWidth(-1);
        ImGui::InputTextWithHint("##search", "Search...", searchBuffer, sizeof(searchBuffer));
        
        ImGui::Separator();
        
        // Scene objects
        if (beginTreeNode("Scene")) {
            if (selectableItem("Main Camera", false)) {
                // Select camera
            }
            if (selectableItem("Directional Light", false)) {
                // Select light
            }
            if (beginTreeNode("Models")) {
                if (selectableItem("Mario", true)) {
                    // Select Mario model
                }
                if (selectableItem("Cube", false)) {
                    // Select cube
                }
                endTreeNode();
            }
            if (beginTreeNode("Environment")) {
                if (selectableItem("Skybox", false)) {
                    // Select skybox
                }
                endTreeNode();
            }
            endTreeNode();
        }
    }
    ImGui::End();
}

void CMentalImGui::showInspectorPanel(bool* isOpen) {
    if (!initialized_) {
        return;
    }
    
    if (ImGui::Begin("Inspector", isOpen)) {
        // Object name
        ImGui::Text("Mario (GameObject)");
        ImGui::Separator();
        
        // Transform component
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            static float position[3] = {0.0f, 0.0f, 0.0f};
            static float rotation[3] = {0.0f, 0.0f, 0.0f};
            static float scale[3] = {0.1f, 0.1f, 0.1f};
            
            vector3Field("Position", position);
            vector3Field("Rotation", rotation);
            vector3Field("Scale", scale);
        }
        
        // Mesh Renderer component
        if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
            propertyField("Cast Shadows", "On");
            propertyField("Receive Shadows", "On");
            propertyField("Materials", "8 materials");
        }
        
        // Mesh Filter component
        if (ImGui::CollapsingHeader("Mesh Filter")) {
            propertyField("Mesh", "Mario (Mesh)");
            propertyField("Vertices", "20,103");
            propertyField("Triangles", "33,482");
        }
        
        // Add Component button
        ImGui::Separator();
        if (ImGui::Button("Add Component", ImVec2(-1, 0))) {
            // Show component menu
        }
    }
    ImGui::End();
}

void CMentalImGui::showScenePanel(bool* isOpen) {
    if (!initialized_) {
        return;
    }
    
    if (ImGui::Begin("Scene", isOpen)) {
        // Scene view controls
        ImGui::Text("Scene View");
        ImGui::SameLine();
        if (ImGui::SmallButton("2D")) {}
        ImGui::SameLine();
        if (ImGui::SmallButton("Lighting")) {}
        ImGui::SameLine();
        if (ImGui::SmallButton("Audio")) {}
        
        ImGui::Separator();
        
        // Scene viewport (placeholder)
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        ImGui::Text("Scene Viewport: %.0fx%.0f", viewportSize.x, viewportSize.y);
        ImGui::InvisibleButton("SceneViewport", viewportSize);
        
        // Gizmo controls
        ImGui::SetCursorPos(ImVec2(10, 50));
        if (ImGui::Button("Move")) {}
        ImGui::SameLine();
        if (ImGui::Button("Rotate")) {}
        ImGui::SameLine();
        if (ImGui::Button("Scale")) {}
    }
    ImGui::End();
}

void CMentalImGui::showGamePanel(bool* isOpen) {
    if (!initialized_) {
        return;
    }
    
    if (ImGui::Begin("Game", isOpen)) {
        // Game view controls
        ImGui::Text("Game View");
        ImGui::SameLine();
        if (ImGui::SmallButton("Free Aspect")) {}
        ImGui::SameLine();
        if (ImGui::SmallButton("Stats")) {}
        ImGui::SameLine();
        if (ImGui::SmallButton("Gizmos")) {}
        
        ImGui::Separator();
        
        // Game viewport (this is where the actual game renders)
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        ImGui::Text("Game Viewport: %.0fx%.0f", viewportSize.x, viewportSize.y);
        ImGui::InvisibleButton("GameViewport", viewportSize);
        
        // Play controls
        ImGui::SetCursorPos(ImVec2(viewportSize.x / 2 - 60, 30));
        if (ImGui::Button("Play")) {}
        ImGui::SameLine();
        if (ImGui::Button("Pause")) {}
        ImGui::SameLine();
        if (ImGui::Button("Step")) {}
    }
    ImGui::End();
}

void CMentalImGui::showProjectPanel(bool* isOpen) {
    if (!initialized_) {
        return;
    }
    
    if (ImGui::Begin("Project", isOpen)) {
        // Project browser
        ImGui::Text("Assets");
        ImGui::Separator();
        
        // Folder structure
        if (beginTreeNode("Assets")) {
            if (beginTreeNode("FBX Assets")) {
                if (beginTreeNode("mario-obj")) {
                    selectableItem("Mario.obj", false);
                    if (beginTreeNode("textures")) {
                        selectableItem("0x39b36443.png", false);
                        selectableItem("0x3b7d69bc.png", false);
                        selectableItem("0x4c506d2f.png", false);
                        endTreeNode();
                    }
                    endTreeNode();
                }
                if (beginTreeNode("cube")) {
                    selectableItem("cube.obj", false);
                    selectableItem("cube.mtl", false);
                    selectableItem("texture.png", false);
                    endTreeNode();
                }
                endTreeNode();
            }
            if (beginTreeNode("Shaders")) {
                selectableItem("fbx_model.vert", false);
                selectableItem("fbx_model.frag", false);
                selectableItem("rubiks_cube.frag", false);
                endTreeNode();
            }
            if (beginTreeNode("Scripts")) {
                selectableItem("fbx_demo.txt", false);
                selectableItem("camera_controller.lua", false);
                endTreeNode();
            }
            endTreeNode();
        }
    }
    ImGui::End();
}

void CMentalImGui::showConsolePanel(bool* isOpen) {
    if (!initialized_) {
        return;
    }
    
    if (ImGui::Begin("Console", isOpen)) {
        // Console controls
        if (ImGui::Button("Clear")) {}
        ImGui::SameLine();
        if (ImGui::Button("Collapse")) {}
        ImGui::SameLine();
        if (ImGui::Button("Clear on Play")) {}
        ImGui::SameLine();
        if (ImGui::Button("Error Pause")) {}
        
        ImGui::Separator();
        
        // Console messages
        ImGui::Text("[Info] Mental Game Engine v1.0.0 initialized");
        ImGui::Text("[Info] ImGui initialized successfully");
        ImGui::Text("[Info] OBJ model loaded: Mario.obj");
        ImGui::Text("[Info] Loaded 8 meshes with 20,103 vertices");
        ImGui::Text("[Warning] Material file Mario.mtl not found");
        ImGui::Text("[Info] Using default materials");
    }
    ImGui::End();
}

bool CMentalImGui::selectableItem(const char* label, bool isSelected) {
    if (!initialized_) {
        return false;
    }
    
    return ImGui::Selectable(label, isSelected);
}

bool CMentalImGui::beginTreeNode(const char* label) {
    if (!initialized_) {
        return false;
    }
    
    return ImGui::TreeNode(label);
}

void CMentalImGui::endTreeNode() {
    if (!initialized_) {
        return;
    }
    
    ImGui::TreePop();
}

void CMentalImGui::propertyField(const char* label, const char* value) {
    if (!initialized_) {
        return;
    }
    
    ImGui::Text("%s:", label);
    ImGui::SameLine();
    ImGui::Text("%s", value);
}

bool CMentalImGui::vector3Field(const char* label, float* vec) {
    if (!initialized_) {
        return false;
    }
    
    return ImGui::DragFloat3(label, vec, 0.01f);
}

void CMentalImGui::separator() {
    if (!initialized_) {
        return;
    }
    
    ImGui::Separator();
}

} // namespace mentalsdk