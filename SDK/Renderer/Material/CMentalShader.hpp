/**
 * @file CMentalShader.hpp
 * @brief Shader management for the Mental SDK
 * @author Mental SDK Team
 * @version 1.0.0
 */

#pragma once

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace mentalsdk
{

/**
 * @class CMentalShader
 * @brief Shader program management class
 * 
 * This class handles loading, compiling, and using OpenGL shaders.
 */
class CMentalShader
{
private:
    GLuint program_id_ = 0; ///< Shader program ID

public:
    /**
     * @brief Constructor
     * @param vertexPath Path to vertex shader file
     * @param fragmentPath Path to fragment shader file
     */
    CMentalShader(const std::string& vertexPath, const std::string& fragmentPath);
    
    /**
     * @brief Destructor
     */
    ~CMentalShader();

    /**
     * @brief Use this shader program
     */
    void use() const;
    
    /**
     * @brief Get the program ID
     * @return OpenGL program ID
     */
    GLuint getID() const { return program_id_; }

    // Utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;

private:
    /**
     * @brief Load shader source from file
     * @param path File path
     * @return Shader source code
     */
    std::string loadShaderSource(const std::string& path);
    
    /**
     * @brief Compile shader
     * @param source Shader source code
     * @param type Shader type (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
     * @return Compiled shader ID
     */
    GLuint compileShader(const std::string& source, GLenum type);
    
    /**
     * @brief Check compilation errors
     * @param shader Shader ID
     * @param type Error type
     */
    void checkCompileErrors(GLuint shader, const std::string& type);
};

// Implementation
inline CMentalShader::CMentalShader(const std::string& vertexPath, const std::string& fragmentPath) {
    // Load shader sources
    std::string vertexCode = loadShaderSource(vertexPath);
    std::string fragmentCode = loadShaderSource(fragmentPath);
    
    // Compile shaders
    GLuint vertex = compileShader(vertexCode, GL_VERTEX_SHADER);
    GLuint fragment = compileShader(fragmentCode, GL_FRAGMENT_SHADER);
    
    // Create shader program
    program_id_ = glCreateProgram();
    glAttachShader(program_id_, vertex);
    glAttachShader(program_id_, fragment);
    glLinkProgram(program_id_);
    checkCompileErrors(program_id_, "PROGRAM");
    
    // Clean up
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

inline CMentalShader::~CMentalShader() {
    if (program_id_ != 0) {
        glDeleteProgram(program_id_);
    }
}

inline void CMentalShader::use() const {
    glUseProgram(program_id_);
}

inline void CMentalShader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(program_id_, name.c_str()), (int)value);
}

inline void CMentalShader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(program_id_, name.c_str()), value);
}

inline void CMentalShader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(program_id_, name.c_str()), value);
}

inline void CMentalShader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(program_id_, name.c_str()), 1, glm::value_ptr(value));
}

inline void CMentalShader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(program_id_, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

inline void CMentalShader::setMat3(const std::string& name, const glm::mat3& mat) const {
    glUniformMatrix3fv(glGetUniformLocation(program_id_, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

inline std::string CMentalShader::loadShaderSource(const std::string& path) {
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        shaderFile.open(path);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        return shaderStream.str();
    } catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << path << std::endl;
        return "";
    }
}

inline GLuint CMentalShader::compileShader(const std::string& source, GLenum type) {
    GLuint shader = glCreateShader(type);
    const char* shaderCode = source.c_str();
    glShaderSource(shader, 1, &shaderCode, NULL);
    glCompileShader(shader);
    checkCompileErrors(shader, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
    return shader;
}

inline void CMentalShader::checkCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];
    
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
        }
    }
}

} // namespace mentalsdk