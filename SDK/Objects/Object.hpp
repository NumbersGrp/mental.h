#pragma once

#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdint>
#include <utility>
#include <vector>
#include <memory>
#include <iostream>
#include "../Utils/Utils.hpp"
#include "Texture.hpp"
#include "Environment.hpp"
#include "../Renderer/Shader.hpp"
#include "Script.hpp"

namespace mentalsdk
{

enum CMentalObjectType : uint8_t {
    Triangle = 0,
    Shader = 1,
    Image = 2,
    ObjModel = 3,
    Camera = 4,
    Environment = 5,
};

class CMentalObject 
{
private:
    std::string name_;
    glm::vec3 position_{ 0.0F, 0.0F, 0.0F };
    glm::vec3 rotation_{ 0.0F, 0.0F, 0.0F };
    glm::vec3 scale_{ 1.0F, 1.0F, 1.0F };
    CMentalObjectType objectType_;
    std::vector<std::shared_ptr<CMentalObject>> nextNode_;

    GLuint vao_ = 0, vbo_ = 0, ebo_ = 0;
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;

    std::string modelPath_;

    std::unique_ptr<CMentalShader> shader_ = nullptr;
    std::unique_ptr<CMentalTexture> texture_ = nullptr;
    std::unique_ptr<CMentalScript> script_ = nullptr;

    MentalEnvironmentType environmentType_ = MentalEnvironmentType::ClearColor;
    
    mutable bool scriptInitialized_ = false; // Flag to track if script init was called

public:
    explicit CMentalObject(std::string name_ = "Undefined node", CMentalObjectType type_ = CMentalObjectType::Triangle)
    : name_(std::move(name_)), objectType_(type_) {}
    
    ~CMentalObject() = default;

    CMentalObject(const CMentalObject&) = delete;
    CMentalObject& operator=(const CMentalObject&) = delete;
    CMentalObject(CMentalObject&&) = delete;
    CMentalObject& operator=(CMentalObject&&) = delete;

    [[nodiscard]] const glm::vec3& getPosition() const { return this->position_; }
    [[nodiscard]] const glm::vec3& getRotation() const { return this->rotation_; }
    [[nodiscard]] const glm::vec3& getScale() const { return this->scale_; }

    void setPosition(const glm::vec3& position) { this->position_ = position; }
    void setRotation(const glm::vec3& rotation) { this->rotation_ = rotation; }
    void setScale(const glm::vec3& scale) { this->scale_ = scale; }

    [[nodiscard]] glm::mat4 getTransformMatrix() const {
        auto transform = glm::mat4(1.0F);
        transform = glm::translate(transform, position_);
        transform = glm::rotate(transform, rotation_[0], glm::vec3(1.0F, 0.0F, 0.0F));
        transform = glm::rotate(transform, rotation_[1], glm::vec3(0.0F, 1.0F, 0.0F));
        transform = glm::rotate(transform, rotation_[2], glm::vec3(0.0F, 0.0F, 1.0F));
        transform = glm::scale(transform, scale_);
        return transform;
    }

    void render() const {
        for (const auto& next : this->nextNode_) {
            next->render();
        }
    }

    void initializeTriangle() {
        // Convert raw float data to Vertex objects - BIGGER triangle
        this->vertices_.clear();
        this->vertices_.reserve(3); // Triangle has 3 vertices
        
        // Vertex 1: bottom left - BIGGER
        this->vertices_.emplace_back(Vertex{
            glm::vec3(-1.0F, -1.0F, 0.0F),  // position (bigger)
            glm::vec3(0.0F, 0.0F, 1.0F),    // normal (pointing towards camera)
            glm::vec2(0.0F, 0.0F)           // texture coordinate
        });
        
        // Vertex 2: bottom right - BIGGER
        this->vertices_.emplace_back(Vertex{
            glm::vec3(1.0F, -1.0F, 0.0F),   // position (bigger)
            glm::vec3(0.0F, 0.0F, 1.0F),    // normal
            glm::vec2(1.0F, 0.0F)           // texture coordinate
        });
        
        // Vertex 3: top - BIGGER
        this->vertices_.emplace_back(Vertex{
            glm::vec3(0.0F, 1.0F, 0.0F),    // position (bigger)
            glm::vec3(0.0F, 0.0F, 1.0F),    // normal
            glm::vec2(0.5F, 1.0F)           // texture coordinate
        });
        
        // Set up indices for triangle
        this->indices_ = {0, 1, 2};
        
        // Set up OpenGL buffers
        this->setupBuffers();
    }

    static std::shared_ptr<CMentalObject> createTriangle(const std::string& name = "Triangle") {
        auto triangle = std::make_shared<CMentalObject>(name, CMentalObjectType::Triangle);
        triangle->initializeTriangle();
        std::cout << "Created triangle '" << name << "' with " << triangle->vertices_.size() << " vertices\n";
        return triangle;
    }

    void setupBuffers() {
        // Generate buffers
        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);
        glGenBuffers(1, &ebo_);
        
        // Bind VAO
        glBindVertexArray(vao_);
        
        // Bind and fill VBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STATIC_DRAW);
        
        // Bind and fill EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), indices_.data(), GL_STATIC_DRAW);
        
        // Set vertex attribute pointers
        // Position attribute (location = 0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
        glEnableVertexAttribArray(0);
        
        // Normal attribute (location = 1)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
        glEnableVertexAttribArray(1);
        
        // Texture coordinate attribute (location = 2)
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoord)));
        glEnableVertexAttribArray(2);
        
        // Unbind VAO
        glBindVertexArray(0);
    }

    void setNext(std::shared_ptr<CMentalObject> nextNode) { 
        this->nextNode_.emplace_back(std::move(nextNode)); 
    }
    
    void setObjModel(const std::string& filePath) {
        if (this->loadFromFile(filePath)) {
            objectType_ = CMentalObjectType::ObjModel;
        }
    }

    void connectShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
        this->shader_ = std::make_unique<CMentalShader>(vertexShaderPath, fragmentShaderPath);
    }

    void connectScript(const std::string& scriptPath) {
        this->script_ = std::make_unique<CMentalScript>(scriptPath);
        this->scriptInitialized_ = false; // Reset initialization flag when connecting new script
    }

    void resetScriptInitialization() {
        this->scriptInitialized_ = false;
    }

    bool loadFromFile(const std::string& filePath);

    void setShader(std::unique_ptr<CMentalShader> shader) { 
        this->shader_ = std::move(shader); 
    }
    void setTexture(std::unique_ptr<CMentalTexture> texture) { this->texture_ = std::move(texture); }
    
    void render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const {
        // Call script functions if script is available
        if (script_ && script_->hasScript()) {
            // Call init only once
            if (!scriptInitialized_) {
                script_->callInit();
                scriptInitialized_ = true;
            }
            
            // Pass current transform to script
            script_->setObjectTransform(position_, rotation_, scale_);
            
            // Call update every frame
            script_->callUpdate();
            
            // Get updated transforms from script (if available)
            float scriptRotation = script_->getRotationFromScript();
            if (scriptRotation != 0.0f) {
                // Apply rotation around Y axis
                const_cast<CMentalObject*>(this)->rotation_.y = scriptRotation;
            }
            
            // Try to get position and scale from script as well
            glm::vec3 scriptPosition = script_->getPositionFromScript();
            if (scriptPosition != glm::vec3(0.0f)) {
                const_cast<CMentalObject*>(this)->position_ = scriptPosition;
            }
            
            glm::vec3 scriptScale = script_->getScaleFromScript();
            if (scriptScale != glm::vec3(1.0f)) {
                const_cast<CMentalObject*>(this)->scale_ = scriptScale;
            }
        }
        
        if (!shader_ || !shader_->isValid()) {
            std::cerr << "Warning: No valid shader for object rendering\n";
            return; // No shader or invalid shader, can't render
        }
        
        // Check for shader hot reload (const_cast needed for hot reload functionality)
        const_cast<CMentalShader*>(shader_.get())->checkAndReload();
        
        // Use the shader
        shader_->use();
        
        // Calculate model matrix with current transformations
        glm::mat4 objectModel = getTransformMatrix();
        
        // Set matrices
        shader_->setMat4("model", objectModel);
        shader_->setMat4("view", view);
        shader_->setMat4("projection", projection);
        
        // Bind texture if available
        if (texture_ && texture_->isValid()) {
            texture_->bind(0); // Bind to texture unit 0
            shader_->setInt("texture1", 0);
        }
        
        // Bind VAO and draw
        glBindVertexArray(vao_);
        
        if (!indices_.empty()) {
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, nullptr);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices_.size()));
        }
        
        glBindVertexArray(0);
        
        // Check for OpenGL errors
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL error during rendering: " << error << "\n";
        }
    }

    void cleanup() {
        glDeleteVertexArrays(1, &vao_);
        glDeleteBuffers(1, &vbo_);
        glDeleteBuffers(1, &ebo_);
        this->shader_.reset();
        this->texture_.reset();
    }
};



} // mentalsdk
