/**
 * @file CMentalCameraController.hpp
 * @brief Camera controller with script-like behavior
 * @author Mental SDK Team
 * @version 1.0.0
 */

#pragma once

#include "../Camera/CMentalCamera.hpp"
#include "../Input/CMentalInput.hpp"
#include <GLFW/glfw3.h>

namespace mentalsdk
{

/**
 * @struct InputState
 * @brief Input state structure for camera controller
 */
struct InputState {
    bool keys[512] = {false}; // GLFW key states
    bool mouseButtons[8] = {false}; // GLFW mouse button states
    double mouseX = 0.0;
    double mouseY = 0.0;
    double mouseDeltaX = 0.0;
    double mouseDeltaY = 0.0;
};

/**
 * @class CMentalCameraController
 * @brief Camera controller that mimics Lua script behavior
 * 
 * This class provides camera control similar to what would be implemented
 * in Lua, but using C++ for better performance and integration.
 */
class CMentalCameraController
{
private:
    CMentalCamera* camera_;
    CMentalInput* input_;
    
    // Settings (mimicking Lua script variables)
    float movementSpeed_ = 5.0F;
    float mouseSensitivity_ = 0.1F;
    
    // Key bindings
    int forwardKey_ = GLFW_KEY_W;
    int backwardKey_ = GLFW_KEY_S;
    int leftKey_ = GLFW_KEY_A;
    int rightKey_ = GLFW_KEY_D;
    int upKey_ = GLFW_KEY_SPACE;
    int downKey_ = GLFW_KEY_LEFT_SHIFT;
    int mouseLookButton_ = GLFW_MOUSE_BUTTON_LEFT;
    
    // Mouse state
    bool firstMouse_ = true;
    double lastMouseX_ = 0.0;
    double lastMouseY_ = 0.0;

public:
    /**
     * @brief Constructor
     * @param camera Camera to control
     * @param input Input system
     */
    CMentalCameraController(CMentalCamera* camera, CMentalInput* input);
    
    /**
     * @brief Initialize controller
     */
    void init();
    
    /**
     * @brief Update camera based on input
     * @param deltaTime Time since last frame
     */
    void update(float deltaTime);
    
    /**
     * @brief Set movement speed
     * @param speed New movement speed
     */
    void setMovementSpeed(float speed) { movementSpeed_ = speed; }
    
    /**
     * @brief Set mouse sensitivity
     * @param sensitivity New mouse sensitivity
     */
    void setMouseSensitivity(float sensitivity) { mouseSensitivity_ = sensitivity; }
    
    /**
     * @brief Get movement speed
     * @return Current movement speed
     */
    float getMovementSpeed() const { return movementSpeed_; }
    
    /**
     * @brief Get mouse sensitivity
     * @return Current mouse sensitivity
     */
    float getMouseSensitivity() const { return mouseSensitivity_; }

private:
    /**
     * @brief Process keyboard input
     * @param deltaTime Time since last frame
     */
    void processKeyboard(float deltaTime);
    
    /**
     * @brief Process mouse input
     */
    void processMouse();
};

// Implementation
inline CMentalCameraController::CMentalCameraController(CMentalCamera* camera, CMentalInput* input)
    : camera_(camera), input_(input) {
}

inline void CMentalCameraController::init() {
    std::cout << "Camera controller initialized" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD - Move camera" << std::endl;
    std::cout << "  Hold Left Mouse + Move - Look around" << std::endl;
    std::cout << "  Space - Move up" << std::endl;
    std::cout << "  Left Shift - Move down" << std::endl;
    
    // Set camera properties
    if (camera_) {
        camera_->setMovementSpeed(movementSpeed_);
        camera_->setMouseSensitivity(mouseSensitivity_);
    }
}

inline void CMentalCameraController::update(float deltaTime) {
    if (!camera_ || !input_) return;
    
    processKeyboard(deltaTime);
    processMouse();
}

inline void CMentalCameraController::processKeyboard(float deltaTime) {
    // Forward/Backward
    if (input_->isKeyPressed(forwardKey_)) {
        camera_->processKeyboard(FORWARD, deltaTime);
    }
    if (input_->isKeyPressed(backwardKey_)) {
        camera_->processKeyboard(BACKWARD, deltaTime);
    }
    
    // Left/Right
    if (input_->isKeyPressed(leftKey_)) {
        camera_->processKeyboard(LEFT, deltaTime);
    }
    if (input_->isKeyPressed(rightKey_)) {
        camera_->processKeyboard(RIGHT, deltaTime);
    }
    
    // Up/Down
    if (input_->isKeyPressed(upKey_)) {
        camera_->processKeyboard(UP, deltaTime);
    }
    if (input_->isKeyPressed(downKey_)) {
        camera_->processKeyboard(DOWN, deltaTime);
    }
}

inline void CMentalCameraController::processMouse() {
    // Only process mouse look when left mouse button is held
    if (input_->isMouseButtonPressed(mouseLookButton_)) {
        double mouseX, mouseY;
        input_->getMousePosition(mouseX, mouseY);
        
        if (firstMouse_) {
            lastMouseX_ = mouseX;
            lastMouseY_ = mouseY;
            firstMouse_ = false;
        }
        
        double deltaX = mouseX - lastMouseX_;
        double deltaY = lastMouseY_ - mouseY; // Reversed since y-coordinates go from bottom to top
        
        lastMouseX_ = mouseX;
        lastMouseY_ = mouseY;
        
        camera_->processMouseMovement(static_cast<float>(deltaX), static_cast<float>(deltaY));
    } else {
        // Reset mouse state when button is not pressed
        firstMouse_ = true;
        double mouseX, mouseY;
        input_->getMousePosition(mouseX, mouseY);
        lastMouseX_ = mouseX;
        lastMouseY_ = mouseY;
    }
}

} // namespace mentalsdk