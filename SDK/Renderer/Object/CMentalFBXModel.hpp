/**
 * @file CMentalFBXModel.hpp
 * @brief FBX model loader and renderer for the Mental SDK
 * @author Mental SDK Team
 * @version 1.0.0
 */

#pragma once

#include "CMentalObject.hpp"
#include <GL/glew.h>
#include <ufbx.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <map>

namespace mentalsdk
{

/**
 * @struct MentalVertex
 * @brief Vertex structure for FBX models
 */
struct MentalVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    
    MentalVertex() = default;
    MentalVertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& uv)
        : position(pos), normal(norm), texCoord(uv) {}
};

/**
 * @struct MentalMesh
 * @brief Mesh data structure
 */
struct MentalMesh {
    std::vector<MentalVertex> vertices;
    std::vector<uint32_t> indices;
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    GLuint textureID = 0;
    std::string materialName;
    std::string texturePath;
    
    void setupMesh();
    void render() const;
    void cleanup();
    bool loadTexture(const std::string& path, bool flipVertically = false);
};

/**
 * @class CMentalFBXModel
 * @brief FBX model loader and renderer
 * 
 * This class loads FBX files using ufbx library and renders them using OpenGL.
 * Supports meshes, materials, and basic animations.
 */
class CMentalFBXModel : public CMentalObject
{
private:
    std::vector<MentalMesh> meshes_;
    ufbx_scene* scene_ = nullptr;
    bool loaded_ = false;
    std::string filePath_;

public:
    /**
     * @brief Constructor
     * @param name Model name
     */
    explicit CMentalFBXModel(const std::string& name = "FBXModel");
    
    /**
     * @brief Destructor
     */
    ~CMentalFBXModel();

    /**
     * @brief Load FBX file
     * @param filePath Path to FBX file
     * @return true if loaded successfully
     */
    bool loadFromFile(const std::string& filePath);
    
    /**
     * @brief Render all meshes
     */
    void render() const;
    
    /**
     * @brief Check if model is loaded
     * @return true if loaded
     */
    bool isLoaded() const { return loaded_; }
    
    /**
     * @brief Get number of meshes
     * @return Number of meshes
     */
    size_t getMeshCount() const { return meshes_.size(); }
    
    /**
     * @brief Get mesh by index
     * @param index Mesh index
     * @return Pointer to mesh or nullptr
     */
    const MentalMesh* getMesh(size_t index) const;
    
    /**
     * @brief Load texture for mesh by index
     * @param meshIndex Index of the mesh
     * @param texturePath Path to texture file
     * @param flipVertically Whether to flip texture vertically
     * @return true if loaded successfully
     */
    bool loadTextureForMesh(size_t meshIndex, const std::string& texturePath, bool flipVertically = false);
    
    /**
     * @brief Get material name for mesh by index
     * @param meshIndex Index of the mesh
     * @return Material name or empty string if invalid index
     */
    std::string getMaterialName(size_t meshIndex) const;
    
    /**
     * @brief Clean up resources
     */
    void cleanup();

private:
    /**
     * @brief Process FBX scene
     */
    void processScene();
    
    /**
     * @brief Process FBX mesh and split by materials
     * @param fbxMesh FBX mesh data
     * @return Vector of meshes split by materials
     */
    std::vector<MentalMesh> processMeshByMaterials(const ufbx_mesh* fbxMesh);
    
    /**
     * @brief Convert ufbx vector to glm vector
     * @param vec ufbx vector
     * @return glm vector
     */
    static glm::vec3 toGlmVec3(const ufbx_vec3& vec);
    
    /**
     * @brief Convert ufbx vector to glm vector2
     * @param vec ufbx vector
     * @return glm vector2
     */
    static glm::vec2 toGlmVec2(const ufbx_vec2& vec);
};

} // namespace mentalsdk