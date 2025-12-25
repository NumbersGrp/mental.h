/**
 * @file CMentalInput.hpp
 * @brief Input system for the Mental SDK
 * @author Mental SDK Team
 * @version 1.0.0
 */

#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>
#include <functional>

namespace mentalsdk
{

/**
 * @class CMentalInput
 * @brief Input management system
 * 
 * This class handles keyboard and mouse input using GLFW callbacks.
 * It provides a simple interface for checking key states and mouse movement.
 */
class CMentalInput
{
private:
    static CMentalInput* instance_;
    
    GLFWwindow* window_;
    std::unordered_map<int, bool> keyStates_;
    std::unordered_map<int, bool> mouseButtonStates_;
    
    double mouseX_ = 0.0;
    double mouseY_ = 0.0;
    double lastMouseX_ = 0.0;
    double lastMouseY_ = 0.0;
    bool firstMouse_ = true;
    
    std::function<void(double, double)> mouseMoveCallback_;
    std::function<void(double)> scrollCallback_;

public:
    /**
     * @brief Constructor
     * @param window GLFW window pointer
     */
    explicit CMentalInput(GLFWwindow* window);
    
    /**
     * @brief Destructor
     */
    ~CMentalInput();
    
    /**
     * @brief Initialize input system
     */
    void initialize();
    
    /**
     * @brief Update input state (call each frame)
     */
    void update();
    
    /**
     * @brief Check if key is pressed
     * @param key GLFW key code
     * @return true if key is pressed
     */
    bool isKeyPressed(int key) const;
    
    /**
     * @brief Check if mouse button is pressed
     * @param button GLFW mouse button code
     * @return true if button is pressed
     */
    bool isMouseButtonPressed(int button) const;
    
    /**
     * @brief Get mouse position
     * @param x Output X position
     * @param y Output Y position
     */
    void getMousePosition(double& x, double& y) const;
    
    /**
     * @brief Get mouse delta (movement since last frame)
     * @param deltaX Output X delta
     * @param deltaY Output Y delta
     */
    void getMouseDelta(double& deltaX, double& deltaY) const;
    
    /**
     * @brief Set mouse move callback
     * @param callback Callback function
     */
    void setMouseMoveCallback(std::function<void(double, double)> callback);
    
    /**
     * @brief Set scroll callback
     * @param callback Callback function
     */
    void setScrollCallback(std::function<void(double)> callback);
    
    /**
     * @brief Reset mouse state
     */
    void resetMouse();

private:
    // GLFW callbacks
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

// Static instance pointer
inline CMentalInput* CMentalInput::instance_ = nullptr;

// Implementation
inline CMentalInput::CMentalInput(GLFWwindow* window) : window_(window) {
    instance_ = this;
}

inline CMentalInput::~CMentalInput() {
    instance_ = nullptr;
}

inline void CMentalInput::initialize() {
    // Set GLFW callbacks
    glfwSetKeyCallback(window_, keyCallback);
    glfwSetMouseButtonCallback(window_, mouseButtonCallback);
    glfwSetCursorPosCallback(window_, cursorPosCallback);
    glfwSetScrollCallback(window_, scrollCallback);
    
    // Get initial mouse position
    glfwGetCursorPos(window_, &mouseX_, &mouseY_);
    lastMouseX_ = mouseX_;
    lastMouseY_ = mouseY_;
}

inline void CMentalInput::update() {
    // Update mouse position
    glfwGetCursorPos(window_, &mouseX_, &mouseY_);
}

inline bool CMentalInput::isKeyPressed(int key) const {
    auto it = keyStates_.find(key);
    return it != keyStates_.end() && it->second;
}

inline bool CMentalInput::isMouseButtonPressed(int button) const {
    auto it = mouseButtonStates_.find(button);
    return it != mouseButtonStates_.end() && it->second;
}

inline void CMentalInput::getMousePosition(double& x, double& y) const {
    x = mouseX_;
    y = mouseY_;
}

inline void CMentalInput::getMouseDelta(double& deltaX, double& deltaY) const {
    deltaX = mouseX_ - lastMouseX_;
    deltaY = lastMouseY_ - mouseY_; // Reversed since y-coordinates go from bottom to top
}

inline void CMentalInput::setMouseMoveCallback(std::function<void(double, double)> callback) {
    mouseMoveCallback_ = callback;
}

inline void CMentalInput::setScrollCallback(std::function<void(double)> callback) {
    scrollCallback_ = callback;
}

inline void CMentalInput::resetMouse() {
    firstMouse_ = true;
    glfwGetCursorPos(window_, &mouseX_, &mouseY_);
    lastMouseX_ = mouseX_;
    lastMouseY_ = mouseY_;
}

// GLFW Callbacks
inline void CMentalInput::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (!instance_) return;
    
    if (action == GLFW_PRESS) {
        instance_->keyStates_[key] = true;
    } else if (action == GLFW_RELEASE) {
        instance_->keyStates_[key] = false;
    }
}

inline void CMentalInput::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (!instance_) return;
    
    if (action == GLFW_PRESS) {
        instance_->mouseButtonStates_[button] = true;
    } else if (action == GLFW_RELEASE) {
        instance_->mouseButtonStates_[button] = false;
    }
}

inline void CMentalInput::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    if (!instance_) return;
    
    if (instance_->firstMouse_) {
        instance_->lastMouseX_ = xpos;
        instance_->lastMouseY_ = ypos;
        instance_->firstMouse_ = false;
    }
    
    double deltaX = xpos - instance_->lastMouseX_;
    double deltaY = instance_->lastMouseY_ - ypos; // Reversed
    
    instance_->lastMouseX_ = xpos;
    instance_->lastMouseY_ = ypos;
    
    if (instance_->mouseMoveCallback_) {
        instance_->mouseMoveCallback_(deltaX, deltaY);
    }
}

inline void CMentalInput::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (!instance_) return;
    
    if (instance_->scrollCallback_) {
        instance_->scrollCallback_(yoffset);
    }
}

} // namespace mentalsdk