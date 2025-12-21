#pragma once


namespace mentalsdk
{

class CMentalShaderMaterial
{
public:
    explicit CMentalShaderMaterial() = default;
    ~CMentalShaderMaterial() = delete;

    CMentalShaderMaterial(const CMentalShaderMaterial&) = delete;
    
    /// @brief Deleted copy assignment operator
    CMentalShaderMaterial& operator=(const CMentalShaderMaterial&) = delete;
    
    /// @brief Deleted move constructor
    CMentalShaderMaterial(CMentalShaderMaterial&&) = delete;
    
    /// @brief Deleted move assignment operator
    CMentalShaderMaterial& operator=(CMentalShaderMaterial&&) = delete;
};

}