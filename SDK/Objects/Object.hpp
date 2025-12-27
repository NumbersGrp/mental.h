#pragma once

#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdint>
#include <utility>
#include <vector>
#include <memory>
#include "../Utils/Utils.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Environment.hpp"

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

    MentalEnvironmentType environmentType_ = MentalEnvironmentType::ClearColor;

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
        // Convert raw float data to Vertex objects
        this->vertices_.clear();
        this->vertices_.reserve(3); // Triangle has 3 vertices
        
        // Vertex 1: bottom left
        this->vertices_.emplace_back(Vertex{
            glm::vec3(-0.5F, -0.5F, 0.0F),  // position
            glm::vec3(0.0F, 0.0F, 1.0F),    // normal (pointing towards camera)
            glm::vec2(0.0F, 0.0F)           // texture coordinate
        });
        
        // Vertex 2: bottom right  
        this->vertices_.emplace_back(Vertex{
            glm::vec3(0.5F, -0.5F, 0.0F),   // position
            glm::vec3(0.0F, 0.0F, 1.0F),    // normal
            glm::vec2(1.0F, 0.0F)           // texture coordinate
        });
        
        // Vertex 3: top
        this->vertices_.emplace_back(Vertex{
            glm::vec3(0.0F, 0.5F, 0.0F),    // position
            glm::vec3(0.0F, 0.0F, 1.0F),    // normal
            glm::vec2(0.5F, 1.0F)           // texture coordinate
        });
    }

    void setNext(std::shared_ptr<CMentalObject> nextNode) { 
        this->nextNode_.emplace_back(std::move(nextNode)); 
    }
    
    void setObjModel(const std::string& filePath) {
        if (this->loadFromFile(filePath)) {
            objectType_ = CMentalObjectType::ObjModel;
        }
    }

    bool loadFromFile(const std::string& filePath);

    void setShader(std::unique_ptr<CMentalShader> shader) { this->shader_ = std::move(shader); }
    void setTexture(std::unique_ptr<CMentalTexture> texture) { this->texture_ = std::move(texture); }
    
    void render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const; // TODO: implement it

    void cleanup() {
        glDeleteVertexArrays(1, &vao_);
        glDeleteBuffers(1, &vbo_);
        glDeleteBuffers(1, &ebo_);
        this->shader_.reset();
        this->texture_.reset();
    }
};



} // mentalsdk
