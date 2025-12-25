/**
 * @file CMentalImGui.hpp
 * @brief ImGui wrapper for the Mental SDK
 * @author Mental SDK Team
 * @version 1.0.0
 */

#pragma once

#include <GLFW/glfw3.h>
#include <memory>

// Forward declaration to avoid including imgui.h in header
struct ImGuiIO;

namespace mentalsdk
{

/**
 * @class CMentalImGui
 * @brief ImGui wrapper class for easy integration
 * 
 * This class provides a simple interface for integrating ImGui into the Mental SDK.
 * It handles initialization, rendering, and cleanup of ImGui.
 */
class CMentalImGui
{
private:
    bool initialized_ = false;
    GLFWwindow* window_ = nullptr;

public:
    /**
     * @brief Constructor
     */
    CMentalImGui() = default;
    
    /**
     * @brief Destructor
     */
    ~CMentalImGui();

    /**
     * @brief Initialize ImGui
     * @param window GLFW window pointer
     * @return true if initialization was successful
     */
    bool initialize(GLFWwindow* window);
    
    /**
     * @brief Start a new ImGui frame
     */
    void newFrame();
    
    /**
     * @brief Render ImGui
     */
    void render();
    
    /**
     * @brief Check if ImGui is initialized
     * @return true if initialized
     */
    bool isInitialized() const { return initialized_; }
    
    /**
     * @brief Cleanup ImGui resources
     */
    void cleanup();
    
    /**
     * @brief Show demo window (for testing)
     * @param show Pointer to bool controlling visibility
     */
    void showDemoWindow(bool* show = nullptr);
    
    /**
     * @brief Create a simple debug window
     * @param title Window title
     * @param isOpen Pointer to bool controlling visibility
     */
    void beginDebugWindow(const char* title, bool* isOpen = nullptr);
    
    /**
     * @brief End the current window
     */
    void endWindow();
    
    /**
     * @brief Add formatted text to current window
     * @param format Format string (printf-style)
     * @param ... Arguments for format string
     */
    void textf(const char* format, ...);
    
    /**
     * @brief Add simple text to current window
     * @param text Text to display
     */
    void text(const char* text);
    
    /**
     * @brief Add a slider for float values
     * @param label Slider label
     * @param value Pointer to float value
     * @param min Minimum value
     * @param max Maximum value
     * @return true if value was changed
     */
    bool sliderFloat(const char* label, float* value, float min, float max);
    
    /**
     * @brief Add a slider for int values
     * @param label Slider label
     * @param value Pointer to int value
     * @param min Minimum value
     * @param max Maximum value
     * @return true if value was changed
     */
    bool sliderInt(const char* label, int* value, int min, int max);
    
    /**
     * @brief Add a checkbox
     * @param label Checkbox label
     * @param value Pointer to bool value
     * @return true if value was changed
     */
    bool checkbox(const char* label, bool* value);
    
    /**
     * @brief Add a button
     * @param label Button label
     * @return true if button was pressed
     */
    bool button(const char* label);
    
    /**
     * @brief Add a color picker
     * @param label Color picker label
     * @param color Pointer to float array (RGB or RGBA)
     * @param hasAlpha Whether to include alpha channel
     * @return true if color was changed
     */
    bool colorPicker(const char* label, float* color, bool hasAlpha = false);
    
    /**
     * @brief Begin Unity-style dockspace
     */
    void beginDockSpace();
    
    /**
     * @brief End Unity-style dockspace
     */
    void endDockSpace();
    
    /**
     * @brief Create Unity-style hierarchy panel
     * @param isOpen Pointer to bool controlling visibility
     */
    void showHierarchyPanel(bool* isOpen = nullptr);
    
    /**
     * @brief Create Unity-style inspector panel
     * @param isOpen Pointer to bool controlling visibility
     */
    void showInspectorPanel(bool* isOpen = nullptr);
    
    /**
     * @brief Create Unity-style scene panel
     * @param isOpen Pointer to bool controlling visibility
     */
    void showScenePanel(bool* isOpen = nullptr);
    
    /**
     * @brief Create Unity-style game panel
     * @param isOpen Pointer to bool controlling visibility
     */
    void showGamePanel(bool* isOpen = nullptr);
    
    /**
     * @brief Create Unity-style project panel
     * @param isOpen Pointer to bool controlling visibility
     */
    void showProjectPanel(bool* isOpen = nullptr);
    
    /**
     * @brief Create Unity-style console panel
     * @param isOpen Pointer to bool controlling visibility
     */
    void showConsolePanel(bool* isOpen = nullptr);
    
    /**
     * @brief Add selectable item (for hierarchy)
     * @param label Item label
     * @param isSelected Whether item is selected
     * @return true if item was clicked
     */
    bool selectableItem(const char* label, bool isSelected);
    
    /**
     * @brief Begin tree node (for hierarchy)
     * @param label Node label
     * @return true if node is open
     */
    bool beginTreeNode(const char* label);
    
    /**
     * @brief End tree node
     */
    void endTreeNode();
    
    /**
     * @brief Add property field (for inspector)
     * @param label Property label
     * @param value Property value as string
     */
    void propertyField(const char* label, const char* value);
    
    /**
     * @brief Add vector3 field (for inspector)
     * @param label Field label
     * @param vec Pointer to 3-component vector
     * @return true if value was changed
     */
    bool vector3Field(const char* label, float* vec);
    
    /**
     * @brief Add separator line
     */
    void separator();
};

} // namespace mentalsdk