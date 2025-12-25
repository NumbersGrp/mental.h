/**
 * @file CMentalTriangle.hpp
 * @brief Triangle primitive object for the Mental SDK renderer
 * @author Mental SDK Team
 * @version 1.0.0
 */

#pragma once

#include "CMentalObject.hpp"
#include <GL/glew.h>
#include <vector>

namespace mentalsdk
{

/**
 * @class CMentalTriangle
 * @brief Triangle primitive implementation
 * 
 * This class represents a triangle object that can be rendered.
 * It manages vertex data, VAO, VBO, and rendering operations.
 */
class CMentalTriangle : public CMentalObject
{
private:
    GLuint vao_ = 0;        ///< Vertex Array Object
    GLuint vbo_ = 0;        ///< Vertex Buffer Object
    bool initialized_ = false; ///< Initialization state

    /// Triangle vertices (position + color)
    std::vector<float> vertices_ = {
        // Position        // Color
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Bottom left (red)
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // Bottom right (green)
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // Top center (blue)
    };

public:
    /**
     * @brief Constructor
     * @param name Triangle name
     */
    explicit CMentalTriangle(const std::string& name = "Triangle");
    
    /**
     * @brief Destructor
     */
    ~CMentalTriangle();

    /**
     * @brief Initialize OpenGL resources
     */
    void initialize();
    
    /**
     * @brief Render the triangle
     */
    void render();
    
    /**
     * @brief Clean up OpenGL resources
     */
    void cleanup();
    
    /**
     * @brief Check if triangle is initialized
     * @return true if initialized, false otherwise
     */
    bool isInitialized() const { return initialized_; }

    /**
     * @brief Get vertex data
     * @return Reference to vertex data
     */
    const std::vector<float>& getVertices() const { return vertices_; }
    
    /**
     * @brief Set vertex data
     * @param vertices New vertex data
     */
    void setVertices(const std::vector<float>& vertices);
};

// Implementation
inline CMentalTriangle::CMentalTriangle(const std::string& name) 
    : CMentalObject(name) 
{
}

inline CMentalTriangle::~CMentalTriangle() {
    cleanup();
}

inline void CMentalTriangle::initialize() {
    if (initialized_) return;
    
    // Generate and bind VAO
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    
    // Generate and bind VBO
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(float), vertices_.data(), GL_STATIC_DRAW);
    
    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    initialized_ = true;
}

inline void CMentalTriangle::render() {
    if (!initialized_) return;
    
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

inline void CMentalTriangle::cleanup() {
    if (initialized_) {
        glDeleteVertexArrays(1, &vao_);
        glDeleteBuffers(1, &vbo_);
        vao_ = 0;
        vbo_ = 0;
        initialized_ = false;
    }
}

inline void CMentalTriangle::setVertices(const std::vector<float>& vertices) {
    vertices_ = vertices;
    if (initialized_) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(float), vertices_.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

} // namespace mentalsdk