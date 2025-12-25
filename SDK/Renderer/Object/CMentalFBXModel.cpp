/**
 * @file CMentalFBXModel.cpp
 * @brief Implementation of FBX model loader and renderer
 * @author Mental SDK Team
 * @version 1.0.0
 */

#include "CMentalFBXModel.hpp"
#include <iostream>

// Include stb_image for texture loading
#define STB_IMAGE_IMPLEMENTATION
#include "../../../External/stb_image/stb_image.h"

namespace mentalsdk
{

// MentalMesh implementation
void MentalMesh::setupMesh() {
    // Generate buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    // Load vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(MentalVertex)), vertices.data(), GL_STATIC_DRAW);
    
    // Load index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(uint32_t)), indices.data(), GL_STATIC_DRAW);
    
    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MentalVertex), reinterpret_cast<void*>(offsetof(MentalVertex, position)));
    glEnableVertexAttribArray(0);
    
    // Normal attribute (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MentalVertex), reinterpret_cast<void*>(offsetof(MentalVertex, normal)));
    glEnableVertexAttribArray(1);
    
    // Texture coordinate attribute (location 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MentalVertex), reinterpret_cast<void*>(offsetof(MentalVertex, texCoord)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void MentalMesh::render() const {
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

void MentalMesh::cleanup() {
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

bool MentalMesh::loadTexture(const std::string& path, bool flipVertically) {
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

// CMentalFBXModel implementation
CMentalFBXModel::CMentalFBXModel(const std::string& name) 
    : CMentalObject(name) 
{
}

CMentalFBXModel::~CMentalFBXModel() {
    cleanup();
}

bool CMentalFBXModel::loadFromFile(const std::string& filePath) {
    cleanup(); // Clean up previous data
    
    filePath_ = filePath;
    
    // Load FBX file
    ufbx_load_opts opts = {};
    ufbx_error error;
    scene_ = ufbx_load_file(filePath.c_str(), &opts, &error);
    
    if (scene_ == nullptr) {
        std::cerr << "Failed to load FBX file: " << filePath << '\n';
        std::cerr << "Error: " << error.description.data << '\n';
        return false;
    }
    
    std::cout << "Loaded FBX file: " << filePath << '\n';
    std::cout << "Meshes: " << scene_->meshes.count << '\n';
    std::cout << "Materials: " << scene_->materials.count << '\n';
    
    processScene();
    loaded_ = true;
    
    return true;
}

void CMentalFBXModel::render() const {
    if (!loaded_) {
        return;
    }
    
    for (const auto& mesh : meshes_) {
        mesh.render();
    }
}

const MentalMesh* CMentalFBXModel::getMesh(size_t index) const {
    if (index >= meshes_.size()) {
        return nullptr;
    }
    return &meshes_[index];
}

void CMentalFBXModel::cleanup() {
    for (auto& mesh : meshes_) {
        mesh.cleanup();
    }
    meshes_.clear();
    
    if (scene_ != nullptr) {
        ufbx_free_scene(scene_);
        scene_ = nullptr;
    }
    
    loaded_ = false;
}

void CMentalFBXModel::processScene() {
    if (scene_ == nullptr) {
        std::cout << "Error: No scene to process\n";
        return;
    }
    
    std::cout << "Processing scene with " << scene_->meshes.count << " meshes\n";
    std::cout << "Materials in scene: " << scene_->materials.count << '\n';
    
    // Print all materials for debugging
    for (size_t i = 0; i < scene_->materials.count; i++) {
        const ufbx_material* material = scene_->materials.data[i];
        if (material != nullptr && material->name.data != nullptr) {
            std::cout << "Material " << i << ": " << material->name.data << '\n';
        }
    }
    
    // Process all meshes
    for (size_t i = 0; i < scene_->meshes.count; i++) {
        const ufbx_mesh* fbxMesh = scene_->meshes.data[i];
        
        if (fbxMesh == nullptr) {
            std::cout << "Warning: Null mesh at index " << i << '\n';
            continue;
        }
        
        std::cout << "Processing mesh " << i << ": " << (fbxMesh->name.data != nullptr ? fbxMesh->name.data : "unnamed") << '\n';
        
        // Split mesh by materials
        auto materialMeshes = processMeshByMaterials(fbxMesh);
        
        // Add each material-specific mesh
        for (auto& mesh : materialMeshes) {
            if (!mesh.vertices.empty() && !mesh.indices.empty()) {
                std::string materialName = mesh.materialName; // Save name before move
                mesh.setupMesh();
                meshes_.push_back(std::move(mesh));
                std::cout << "Added mesh for material: " << materialName << " (texture loading handled by application)\n";
            }
        }
    }
    
    std::cout << "Scene processing complete. Total meshes: " << meshes_.size() << '\n';
}

glm::vec3 CMentalFBXModel::toGlmVec3(const ufbx_vec3& vec) {
    return {static_cast<float>(vec.x), static_cast<float>(vec.y), static_cast<float>(vec.z)};
}

glm::vec2 CMentalFBXModel::toGlmVec2(const ufbx_vec2& vec) {
    return {static_cast<float>(vec.x), static_cast<float>(vec.y)};
}

std::vector<MentalMesh> CMentalFBXModel::processMeshByMaterials(const ufbx_mesh* fbxMesh) {
    std::vector<MentalMesh> materialMeshes;
    
    if (fbxMesh == nullptr || fbxMesh->num_vertices == 0) {
        std::cout << "Warning: Empty or invalid mesh\n";
        return materialMeshes;
    }
    
    std::cout << "Splitting mesh with " << fbxMesh->num_vertices << " vertices by materials\n";
    
    // Create a map to store vertices and indices for each material
    std::map<std::string, MentalMesh> materialMeshMap;
    std::map<std::string, std::map<uint32_t, uint32_t>> vertexRemapping; // old index -> new index per material
    
    // Process all faces and group by material
    for (size_t faceIndex = 0; faceIndex < fbxMesh->faces.count; faceIndex++) {
        ufbx_face face = fbxMesh->faces.data[faceIndex];
        
        // Get material for this face
        std::string faceMaterial = "default";
        if (fbxMesh->face_material.data != nullptr && faceIndex < fbxMesh->face_material.count) {
            uint32_t materialIndex = fbxMesh->face_material.data[faceIndex];
            if (materialIndex < fbxMesh->materials.count && fbxMesh->materials.data[materialIndex] != nullptr) {
                faceMaterial = fbxMesh->materials.data[materialIndex]->name.data;
            }
        }
        
        // Initialize mesh for this material if not exists
        if (materialMeshMap.find(faceMaterial) == materialMeshMap.end()) {
            materialMeshMap[faceMaterial].materialName = faceMaterial;
        }
        
        MentalMesh& mesh = materialMeshMap[faceMaterial];
        auto& vertexMap = vertexRemapping[faceMaterial];
        
        // Process face triangles
        if (face.num_indices >= 3) {
            for (size_t triIndex = 0; triIndex < face.num_indices - 2; triIndex++) {
                // Get triangle vertices
                uint32_t originalIndices[3] = {
                    fbxMesh->vertex_indices.data[face.index_begin + 0],
                    fbxMesh->vertex_indices.data[face.index_begin + triIndex + 1],
                    fbxMesh->vertex_indices.data[face.index_begin + triIndex + 2]
                };
                
                // Add vertices to material mesh (with deduplication)
                for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
                    uint32_t originalIdx = originalIndices[vertexIndex];
                    
                    if (originalIdx >= fbxMesh->num_vertices) {
                        continue;
                    }
                    
                    uint32_t newIdx;
                    
                    // Check if we already added this vertex to this material mesh
                    if (vertexMap.find(originalIdx) != vertexMap.end()) {
                        newIdx = vertexMap[originalIdx];
                    } else {
                        // Add new vertex
                        MentalVertex vertex;
                        
                        // Position
                        if (fbxMesh->vertices.data != nullptr && originalIdx < fbxMesh->vertices.count) {
                            vertex.position = toGlmVec3(fbxMesh->vertices.data[originalIdx]);
                        } else {
                            vertex.position = glm::vec3(0.0F);
                        }
                        
                        // Normal
                        if (fbxMesh->vertex_normal.exists && fbxMesh->vertex_normal.values.data != nullptr) {
                            ufbx_vec3 normal = ufbx_get_vertex_vec3(&fbxMesh->vertex_normal, originalIdx);
                            vertex.normal = glm::normalize(toGlmVec3(normal));
                        } else {
                            vertex.normal = glm::vec3(0.0F, 1.0F, 0.0F);
                        }
                        
                        // Texture coordinates
                        if (fbxMesh->vertex_uv.exists && fbxMesh->vertex_uv.values.data != nullptr) {
                            ufbx_vec2 uv = ufbx_get_vertex_vec2(&fbxMesh->vertex_uv, originalIdx);
                            vertex.texCoord = toGlmVec2(uv);
                            
                            // Debug UV coordinates for first few vertices
                            if (originalIdx < 5) {
                                std::cout << "Vertex " << originalIdx << " UV: (" << uv.x << ", " << uv.y << ")\n";
                            }
                        } else {
                            vertex.texCoord = glm::vec2(0.0F, 0.0F);
                        }
                        
                        newIdx = static_cast<uint32_t>(mesh.vertices.size());
                        mesh.vertices.push_back(vertex);
                        vertexMap[originalIdx] = newIdx;
                    }
                    
                    mesh.indices.push_back(newIdx);
                }
            }
        }
    }
    
    // Convert map to vector
    for (auto& pair : materialMeshMap) {
        if (!pair.second.vertices.empty() && !pair.second.indices.empty()) {
            std::cout << "Created mesh for material '" << pair.first << "': " 
                      << pair.second.vertices.size() << " vertices, " 
                      << pair.second.indices.size() << " indices\n";
            materialMeshes.push_back(std::move(pair.second));
        }
    }
    
    return materialMeshes;
}

bool CMentalFBXModel::loadTextureForMesh(size_t meshIndex, const std::string& texturePath, bool flipVertically) {
    if (meshIndex >= meshes_.size()) {
        std::cout << "Error: Invalid mesh index " << meshIndex << '\n';
        return false;
    }
    
    return meshes_[meshIndex].loadTexture(texturePath, flipVertically);
}

std::string CMentalFBXModel::getMaterialName(size_t meshIndex) const {
    if (meshIndex >= meshes_.size()) {
        return "";
    }
    
    return meshes_[meshIndex].materialName;
}

} // namespace mentalsdk