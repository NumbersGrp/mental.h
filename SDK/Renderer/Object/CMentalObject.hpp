
/**
 * @file CMentalObject.hpp
 * @brief 3D object representation for the Mental SDK renderer
 * @author Mental SDK Team
 * @version 1.0.0
 */

#pragma once

#include <cstdint>
#include <string>

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
};

} // namespace mentalsdk