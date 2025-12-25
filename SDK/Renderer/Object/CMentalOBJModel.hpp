/**
 * @file CMentalOBJModel.hpp
 * @brief OBJ model loader using TinyObjLoader for the Mental SDK
 * @author Mental SDK Team
 * @version 1.0.0
 */

#pragma once

#include "CMentalObject.hpp"
#include <GL/glew.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>

// Forward declarations
namespace tinyobj {
    struct attrib_t;
    struct shape_t;
    struct material_t;
}

namespace mentalsdk
{

/**
 * @struct MentalOBJVertex
 * @brief Vertex structure for OBJ models
 */
struct MentalOBJVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    
    MentalOBJVertex() = default;
    MentalOBJVertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& uv)
        : position(pos), normal(norm), texCoord(uv) {}
};

/**
 * @struct MentalOBJMesh
 * @brief Mesh data structure for OBJ models
 */
struct MentalOBJMesh {
    std::vector<MentalOBJVertex> vertices;
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
 * @class CMentalOBJModel
 * @brief OBJ model loader using TinyObjLoader
 * 
 * This class loads OBJ files using TinyObjLoader library and renders them using OpenGL.
 * Properly handles UV coordinates, materials, and textures.
 */
class CMentalOBJModel : public CMentalObject
{
private:
    std::vector<MentalOBJMesh> meshes_;
    bool loaded_ = false;
    std::string filePath_;
    std::string basePath_;

public:
    /**
     * @brief Constructor
     * @param name Model name
     */
    explicit CMentalOBJModel(const std::string& name = "OBJModel");
    
    /**
     * @brief Destructor
     */
    ~CMentalOBJModel();

    /**
     * @brief Load OBJ file
     * @param filePath Path to OBJ file
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
    const MentalOBJMesh* getMesh(size_t index) const;
    
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
     * @brief Process loaded OBJ data
     * @param attrib Vertex attributes
     * @param shapes Shape data
     * @param materials Material data
     */
    void processOBJData(const tinyobj::attrib_t& attrib, 
                       const std::vector<tinyobj::shape_t>& shapes,
                       const std::vector<tinyobj::material_t>& materials);
    
    /**
     * @brief Extract base path from file path
     * @param filePath Full file path
     * @return Base directory path
     */
    std::string extractBasePath(const std::string& filePath) const;
};

} // namespace mentalsdk