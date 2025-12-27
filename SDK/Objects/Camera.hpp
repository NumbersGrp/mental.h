#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Forward declaration
namespace mentalsdk {
    class CMentalObject;
    enum CMentalObjectType : uint8_t;
}

#include "Object.hpp"

namespace mentalsdk {

const float DEFAULT_CAMERA_YAW = -90.0F;
const float DEFAULT_CAMERA_PITCH = 0.0F;
const float DEFAULT_CAMERA_SPEED = 2.5F;
const float DEFAULT_MOUSE_SENSIVITY = 2.5F;
const float DEFAULT_CAMERA_ZOOM = 45.0F;


enum CMentalCameraMovement : uint8_t {
    Forward = 0,
    Backward = 1,
    Left = 2,
    Right = 3,
    Up = 4,
    Down = 5,
};


class CMentalCamera : public CMentalObject
{
private:
    glm::vec3 position_ = glm::vec3(0.0F, 0.0F, 0.0F);
    glm::vec3 front_{};
    glm::vec3 up_ = glm::vec3(0.0F, 1.0F, 0.0F);
    glm::vec3 right_{};
    glm::vec3 worldUp_{};

    float yaw_ = DEFAULT_CAMERA_YAW;
    float pitch_ = DEFAULT_CAMERA_PITCH;

    float cameraSpeed_ = DEFAULT_CAMERA_SPEED;
    float mouseSensitivity_ = DEFAULT_MOUSE_SENSIVITY;
    float zoom_ = DEFAULT_CAMERA_ZOOM;

    bool firstMouse_ = true;
    float lastX_ = 0.0F;
    float lastY_ = 0.0F;

public:
    CMentalCamera() : CMentalObject("Camera", CMentalObjectType::Camera) {}
    ~CMentalCamera() = default;

    CMentalCamera(const CMentalCamera&) = delete;
    CMentalCamera& operator=(const CMentalCamera&) = delete;
    CMentalCamera(CMentalCamera&&) = delete;
    CMentalCamera& operator=(CMentalCamera&&) = delete;

    glm::vec3& getPosition() { return this->position_; }
    glm::vec3& getUp() { return this-> up_; }

    void setPosition(const glm::vec3& position) { this->position_ = position; }
    void setUp(const glm::vec3& upx) { this->up_ = upx; }

};

} // mentalsdk
