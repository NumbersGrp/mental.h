/**
 * @file CMentalCamera.hpp
 * @brief Camera system for the Mental SDK
 * @author Mental SDK Team
 * @version 1.0.0
 */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace mentalsdk
{

/**
 * @enum CameraMovement
 * @brief Camera movement directions
 */
enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

/**
 * @class CMentalCamera
 * @brief First-person camera implementation
 * 
 * This class provides a first-person camera with mouse look and WASD movement.
 * It generates view matrices for 3D rendering.
 */
class CMentalCamera
{
private:
    // Camera attributes
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    glm::vec3 worldUp_;
    
    // Euler angles
    float yaw_;
    float pitch_;
    
    // Camera options
    float movementSpeed_;
    float mouseSensitivity_;
    float zoom_;
    
    // Mouse state
    bool firstMouse_;
    float lastX_;
    float lastY_;

public:
    /**
     * @brief Constructor with vectors
     * @param position Camera position
     * @param up World up vector
     * @param yaw Initial yaw angle
     * @param pitch Initial pitch angle
     */
    explicit CMentalCamera(
        glm::vec3 position = glm::vec3(0.0F, 0.0F, 3.0F),
        glm::vec3 up = glm::vec3(0.0F, 1.0F, 0.0F),
        float yaw = -90.0F,
        float pitch = 0.0F
    );
    
    /**
     * @brief Get the view matrix
     * @return View matrix
     */
    glm::mat4 getViewMatrix() const;
    
    /**
     * @brief Process keyboard input
     * @param direction Movement direction
     * @param deltaTime Time since last frame
     */
    void processKeyboard(CameraMovement direction, float deltaTime);
    
    /**
     * @brief Process mouse movement
     * @param xOffset Mouse X offset
     * @param yOffset Mouse Y offset
     * @param constrainPitch Whether to constrain pitch
     */
    void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);
    
    /**
     * @brief Process mouse scroll
     * @param yOffset Scroll Y offset
     */
    void processMouseScroll(float yOffset);
    
    /**
     * @brief Get camera position
     * @return Camera position
     */
    const glm::vec3& getPosition() const { return position_; }
    
    /**
     * @brief Get camera front vector
     * @return Camera front vector
     */
    const glm::vec3& getFront() const { return front_; }
    
    /**
     * @brief Get camera zoom (FOV)
     * @return Zoom value
     */
    float getZoom() const { return zoom_; }
    
    /**
     * @brief Set camera position
     * @param position New position
     */
    void setPosition(const glm::vec3& position) { position_ = position; }
    
    /**
     * @brief Set movement speed
     * @param speed New movement speed
     */
    void setMovementSpeed(float speed) { movementSpeed_ = speed; }
    
    /**
     * @brief Get movement speed
     * @return Current movement speed
     */
    float getMovementSpeed() const { return movementSpeed_; }
    
    /**
     * @brief Set mouse sensitivity
     * @param sensitivity New mouse sensitivity
     */
    void setMouseSensitivity(float sensitivity) { mouseSensitivity_ = sensitivity; }
    
    /**
     * @brief Get mouse sensitivity
     * @return Current mouse sensitivity
     */
    float getMouseSensitivity() const { return mouseSensitivity_; }
    
    /**
     * @brief Reset camera orientation to default
     */
    void resetOrientation() {
        yaw_ = -90.0f;
        pitch_ = 0.0f;
        updateCameraVectors();
    }
    
    /**
     * @brief Reset mouse state (call when mouse enters window)
     */
    void resetMouse() { firstMouse_ = true; }

private:
    /**
     * @brief Update camera vectors
     */
    void updateCameraVectors();
};

// Implementation
inline CMentalCamera::CMentalCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : position_(position)
    , worldUp_(up)
    , yaw_(yaw)
    , pitch_(pitch)
    , movementSpeed_(2.5F)
    , mouseSensitivity_(0.1F)
    , zoom_(45.0F)
    , firstMouse_(true)
    , lastX_(0.0F)
    , lastY_(0.0F)
{
    updateCameraVectors();
}

inline glm::mat4 CMentalCamera::getViewMatrix() const {
    return glm::lookAt(position_, position_ + front_, up_);
}

inline void CMentalCamera::processKeyboard(CameraMovement direction, float deltaTime) {
    float velocity = movementSpeed_ * deltaTime;
    
    switch (direction) {
        case FORWARD:
            position_ += front_ * velocity;
            break;
        case BACKWARD:
            position_ -= front_ * velocity;
            break;
        case LEFT:
            position_ -= right_ * velocity;
            break;
        case RIGHT:
            position_ += right_ * velocity;
            break;
        case UP:
            position_ += up_ * velocity;
            break;
        case DOWN:
            position_ -= up_ * velocity;
            break;
    }
}

inline void CMentalCamera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
    xOffset *= mouseSensitivity_;
    yOffset *= mouseSensitivity_;
    
    yaw_ += xOffset;
    pitch_ += yOffset;
    
    // Constrain pitch to avoid screen flip
    if (constrainPitch) {
        if (pitch_ > 89.0F) {
            pitch_ = 89.0F;
        }
        if (pitch_ < -89.0F) {
            pitch_ = -89.0F;
        }
    }
    
    updateCameraVectors();
}

inline void CMentalCamera::processMouseScroll(float yOffset) {
    zoom_ -= yOffset;
    if (zoom_ < 1.0F) {
        zoom_ = 1.0F;
    }
    if (zoom_ > 45.0F) {
        zoom_ = 45.0F;
    }
}

inline void CMentalCamera::updateCameraVectors() {
    // Calculate new front vector
    glm::vec3 front;
    front.x = std::cos(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
    front.y = std::sin(glm::radians(pitch_));
    front.z = std::sin(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
    front_ = glm::normalize(front);
    
    // Re-calculate right and up vectors
    right_ = glm::normalize(glm::cross(front_, worldUp_));
    up_ = glm::normalize(glm::cross(right_, front_));
}

} // namespace mentalsdk