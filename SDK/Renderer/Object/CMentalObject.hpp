/**
 * @file CMentalObject.hpp
 * @brief 3D object representation for the Mental SDK renderer
 * @author Mental SDK Team
 * @version 1.0.0
 */

#pragma once

#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace mentalsdk
{

/**
 * @enum MentalObjectType
 * @brief Enumeration of supported 3D object types
 * 
 * Uses std::uint8_t as the underlying type for memory efficiency.
 */
enum MentalObjectType : std::uint8_t {
    Triangle = 0,  ///< Triangle primitive object
    Fbx = 1, ///< Fbx model
};

/**
 * @class CMentalObject
 * @brief Base class for 3D objects in the Mental SDK
 * 
 * This class represents a 3D object that can be rendered by the Mental SDK.
 * It provides the basic interface for object management and rendering.
 * 
 * @note This class is non-copyable and non-movable
 */
class CMentalObject
{
private:
    std::string name_; ///< Object name for identification
    glm::vec3 position_{0.0F, 0.0F, 0.0F}; ///< Object position in 3D space
    glm::vec3 rotation_{0.0F, 0.0F, 0.0F}; ///< Object rotation (Euler angles)
    glm::vec3 scale_{1.0F, 1.0F, 1.0F}; ///< Object scale

public:
    /**
     * @brief Constructor with name
     * @param name Object name
     */
    explicit CMentalObject(const std::string& name = "UnnamedObject") : name_(name) {}
    
    /**
     * @brief Destructor
     */
    ~CMentalObject() = default;

    /// @brief Deleted copy constructor
    CMentalObject(const CMentalObject&) = delete;
    
    /// @brief Deleted copy assignment operator
    CMentalObject& operator=(const CMentalObject&) = delete;
    
    /// @brief Deleted move constructor
    CMentalObject(CMentalObject&&) = delete;
    
    /// @brief Deleted move assignment operator
    CMentalObject& operator=(CMentalObject&&) = delete;

    /**
     * @brief Get the object name
     * @return Object name
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief Set the object name
     * @param name New object name
     */
    void setName(const std::string& name) { name_ = name; }

    /**
     * @brief Get the object position
     * @return Position vector
     */
    const glm::vec3& getPosition() const { return position_; }
    
    /**
     * @brief Set the object position
     * @param position New position vector
     */
    void setPosition(const glm::vec3& position) { position_ = position; }

    /**
     * @brief Get the object rotation
     * @return Rotation vector (Euler angles)
     */
    const glm::vec3& getRotation() const { return rotation_; }
    
    /**
     * @brief Set the object rotation
     * @param rotation New rotation vector (Euler angles)
     */
    void setRotation(const glm::vec3& rotation) { rotation_ = rotation; }

    /**
     * @brief Get the object scale
     * @return Scale vector
     */
    const glm::vec3& getScale() const { return scale_; }
    
    /**
     * @brief Set the object scale
     * @param scale New scale vector
     */
    void setScale(const glm::vec3& scale) { scale_ = scale; }

    /**
     * @brief Get the transformation matrix
     * @return 4x4 transformation matrix
     */
    glm::mat4 getTransformMatrix() const {
        glm::mat4 transform = glm::mat4(1.0F);
        transform = glm::translate(transform, position_);
        transform = glm::rotate(transform, rotation_.x, glm::vec3(1.0F, 0.0F, 0.0F));
        transform = glm::rotate(transform, rotation_.y, glm::vec3(0.0F, 1.0F, 0.0F));
        transform = glm::rotate(transform, rotation_.z, glm::vec3(0.0F, 0.0F, 1.0F));
        transform = glm::scale(transform, scale_);
        return transform;
    }
};

} // namespace mentalsdk