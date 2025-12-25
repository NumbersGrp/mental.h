/**
 * @file CMentalOBJModel.cpp
 * @brief Implementation of OBJ model loader using TinyObjLoader
 * @author Mental SDK Team
 * @version 1.0.0
 */

#include "CMentalOBJModel.hpp"
#include <iostream>

// Define implementation before including
#define TINYOBJLOADER_IMPLEMENTATION
#include "../../../External/tinyobjloader/tinyobjloader.h"

// Include stb_image for texture loading (without implementation)
#include "../../../External/stb_image/stb_image.h"

namespace mentalsdk
{

// MentalOBJMesh implementation
void MentalOBJMesh::setupMesh() {
    // Generate buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    // Load vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(MentalOBJVertex)), vertices.data(), GL_STATIC_DRAW);
    
    // Load index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(uint32_t)), indices.data(), GL_STATIC_DRAW);
    
    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MentalOBJVertex), reinterpret_cast<void*>(offsetof(MentalOBJVertex, position)));
    glEnableVertexAttribArray(0);
    
    // Normal attribute (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MentalOBJVertex), reinterpret_cast<void*>(offsetof(MentalOBJVertex, normal)));
    glEnableVertexAttribArray(1);
    
    // Texture coordinate attribute (location 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MentalOBJVertex), reinterpret_cast<void*>(offsetof(MentalOBJVertex, texCoord)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void MentalOBJMesh::render() const {
    if (VAO == 0) {
        return;
    }
    
    // Bind texture if available
    if (textureID != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    
    // Unbind texture
    if (textureID != 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void MentalOBJMesh::cleanup() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        VAO = VBO = EBO = 0;
    }
    
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }
}

bool MentalOBJMesh::loadTexture(const std::string& path, bool flipVertically) {
    // Set texture flip based on parameter
    stbi_set_flip_vertically_on_load(flipVertically);
    
    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    
    if (data == nullptr) {
        std::cout << "Failed to load texture: " << path << '\n';
        std::cout << "STB Error: " << stbi_failure_reason() << '\n';
        return false;
    }
    
    // Successfully loaded texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Determine format
    GLenum format = GL_RGB;
    if (channels == 1) {
        format = GL_RED;
    } else if (channels == 3) {
        format = GL_RGB;
    } else if (channels == 4) {
        format = GL_RGBA;
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Free image data
    stbi_image_free(data);
    
    texturePath = path;
    std::cout << "Loaded texture: " << path << " (" << width << "x" << height << ", " << channels << " channels)\n";
    
    return true;
}

// CMentalOBJModel implementation
CMentalOBJModel::CMentalOBJModel(const std::string& name) 
    : CMentalObject(name) 
{
}

CMentalOBJModel::~CMentalOBJModel() {
    cleanup();
}

bool CMentalOBJModel::loadFromFile(const std::string& filePath) {
    cleanup(); // Clean up previous data
    
    filePath_ = filePath;
    basePath_ = extractBasePath(filePath);
    
    // Load OBJ file using TinyObjLoader
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, 
                               filePath.c_str(), basePath_.c_str());
    
    if (!warn.empty()) {
        std::cout << "TinyObjLoader Warning: " << warn << '\n';
    }
    
    if (!err.empty()) {
        std::cerr << "TinyObjLoader Error: " << err << '\n';
    }
    
    if (!ret) {
        std::cerr << "Failed to load OBJ file: " << filePath << '\n';
        return false;
    }
    
    std::cout << "Loaded OBJ file: " << filePath << '\n';
    std::cout << "Shapes: " << shapes.size() << '\n';
    std::cout << "Materials: " << materials.size() << '\n';
    std::cout << "Vertices: " << attrib.vertices.size() / 3 << '\n';
    std::cout << "Normals: " << attrib.normals.size() / 3 << '\n';
    std::cout << "TexCoords: " << attrib.texcoords.size() / 2 << '\n';
    
    processOBJData(attrib, shapes, materials);
    loaded_ = true;
    
    return true;
}

void CMentalOBJModel::render() const {
    if (!loaded_) {
        return;
    }
    
    for (const auto& mesh : meshes_) {
        mesh.render();
    }
}

const MentalOBJMesh* CMentalOBJModel::getMesh(size_t index) const {
    if (index >= meshes_.size()) {
        return nullptr;
    }
    return &meshes_[index];
}

void CMentalOBJModel::cleanup() {
    for (auto& mesh : meshes_) {
        mesh.cleanup();
    }
    meshes_.clear();
    loaded_ = false;
}

void CMentalOBJModel::processOBJData(const tinyobj::attrib_t& attrib, 
                                    const std::vector<tinyobj::shape_t>& shapes,
                                    const std::vector<tinyobj::material_t>& materials) {
    
    std::cout << "Processing OBJ data with " << shapes.size() << " shapes\n";
    
    // Process each shape
    for (size_t shapeIdx = 0; shapeIdx < shapes.size(); shapeIdx++) {
        const auto& shape = shapes[shapeIdx];
        
        std::cout << "Processing shape " << shapeIdx << ": " << shape.name << '\n';
        std::cout << "  Indices: " << shape.mesh.indices.size() << '\n';
        std::cout << "  Material IDs: " << shape.mesh.material_ids.size() << '\n';
        
        // Group faces by material
        std::map<int, std::vector<tinyobj::index_t>> materialGroups;
        
        // Group indices by material
        for (size_t faceIdx = 0; faceIdx < shape.mesh.material_ids.size(); faceIdx++) {
            int materialId = shape.mesh.material_ids[faceIdx];
            
            // Each face has 3 indices (triangulated)
            size_t indexOffset = faceIdx * 3;
            for (int i = 0; i < 3; i++) {
                if (indexOffset + i < shape.mesh.indices.size()) {
                    materialGroups[materialId].push_back(shape.mesh.indices[indexOffset + i]);
                }
            }
        }
        
        // Create mesh for each material group
        for (const auto& materialGroup : materialGroups) {
            int materialId = materialGroup.first;
            const auto& indices = materialGroup.second;
            
            MentalOBJMesh mesh;
            
            // Set material name
            if (materialId >= 0 && materialId < static_cast<int>(materials.size())) {
                mesh.materialName = materials[materialId].name;
            } else {
                mesh.materialName = "default";
            }
            
            std::cout << "  Creating mesh for material: " << mesh.materialName << " (" << indices.size() << " indices)\n";
            
            // Create vertex data
            std::map<std::tuple<int, int, int>, uint32_t> vertexMap;
            
            for (const auto& index : indices) {
                auto key = std::make_tuple(index.vertex_index, index.normal_index, index.texcoord_index);
                
                if (vertexMap.find(key) == vertexMap.end()) {
                    MentalOBJVertex vertex;
                    
                    // Position
                    if (index.vertex_index >= 0 && index.vertex_index * 3 + 2 < static_cast<int>(attrib.vertices.size())) {
                        vertex.position.x = attrib.vertices[index.vertex_index * 3 + 0];
                        vertex.position.y = attrib.vertices[index.vertex_index * 3 + 1];
                        vertex.position.z = attrib.vertices[index.vertex_index * 3 + 2];
                    }
                    
                    // Normal
                    if (index.normal_index >= 0 && index.normal_index * 3 + 2 < static_cast<int>(attrib.normals.size())) {
                        vertex.normal.x = attrib.normals[index.normal_index * 3 + 0];
                        vertex.normal.y = attrib.normals[index.normal_index * 3 + 1];
                        vertex.normal.z = attrib.normals[index.normal_index * 3 + 2];
                    } else {
                        vertex.normal = glm::vec3(0.0F, 1.0F, 0.0F);
                    }
                    
                    // Texture coordinates
                    if (index.texcoord_index >= 0 && index.texcoord_index * 2 + 1 < static_cast<int>(attrib.texcoords.size())) {
                        vertex.texCoord.x = attrib.texcoords[index.texcoord_index * 2 + 0];
                        vertex.texCoord.y = attrib.texcoords[index.texcoord_index * 2 + 1];
                        
                        // Debug UV coordinates for first few vertices
                        if (mesh.vertices.size() < 5) {
                            std::cout << "    Vertex " << mesh.vertices.size() << " UV: (" 
                                      << vertex.texCoord.x << ", " << vertex.texCoord.y << ")\n";
                        }
                    } else {
                        vertex.texCoord = glm::vec2(0.0F, 0.0F);
                    }
                    
                    uint32_t vertexIndex = static_cast<uint32_t>(mesh.vertices.size());
                    mesh.vertices.push_back(vertex);
                    vertexMap[key] = vertexIndex;
                    mesh.indices.push_back(vertexIndex);
                } else {
                    mesh.indices.push_back(vertexMap[key]);
                }
            }
            
            if (!mesh.vertices.empty() && !mesh.indices.empty()) {
                std::cout << "  Created mesh: " << mesh.vertices.size() << " vertices, " 
                          << mesh.indices.size() << " indices\n";
                mesh.setupMesh();
                meshes_.push_back(std::move(mesh));
            }
        }
    }
    
    std::cout << "OBJ processing complete. Total meshes: " << meshes_.size() << '\n';
}

std::string CMentalOBJModel::extractBasePath(const std::string& filePath) const {
    size_t lastSlash = filePath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        return filePath.substr(0, lastSlash + 1);
    }
    return "";
}

bool CMentalOBJModel::loadTextureForMesh(size_t meshIndex, const std::string& texturePath, bool flipVertically) {
    if (meshIndex >= meshes_.size()) {
        std::cout << "Error: Invalid mesh index " << meshIndex << '\n';
        return false;
    }
    
    return meshes_[meshIndex].loadTexture(texturePath, flipVertically);
}

std::string CMentalOBJModel::getMaterialName(size_t meshIndex) const {
    if (meshIndex >= meshes_.size()) {
        return "";
    }
    
    return meshes_[meshIndex].materialName;
}

} // namespace mentalsdk