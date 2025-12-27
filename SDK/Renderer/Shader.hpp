#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace mentalsdk
{

const int MAX_LOG_INFO_LENGTH = 512;

class CMentalShader
{
private:
    GLuint programID_ = 0;
    
    static GLuint compileShader(const std::string& source, GLenum shaderType) {
        unsigned int shader = glCreateShader(shaderType);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        int success = 0;
        char logInfo[MAX_LOG_INFO_LENGTH];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        
        if (success == 0) {
            glGetShaderInfoLog(shader, MAX_LOG_INFO_LENGTH, nullptr, logInfo);
            std::cerr << "Shader compilation error:\n" << logInfo << "\n";
            return 0;
        }
        return shader;
    }

    void createShaderProgram(const std::string& vertexData, const std::string& fragmentData) {
        unsigned int vertexShader = this->compileShader(vertexData, GL_VERTEX_SHADER);
        unsigned int fragmentShader = this->compileShader(fragmentData, GL_FRAGMENT_SHADER);

        this->programID_ = glCreateProgram();
        glAttachShader(this->programID_, vertexShader);
        glAttachShader(this->programID_, fragmentShader);
        glLinkProgram(this->programID_);

        int success = 0;
        char logInfo[MAX_LOG_INFO_LENGTH];
        glGetProgramiv(this->programID_, GL_LINK_STATUS, &success);
        if (success == 0) {
            glGetProgramInfoLog(this->programID_, MAX_LOG_INFO_LENGTH, nullptr, logInfo);
            std::cerr << "Shader program linking error:\n" << logInfo << "\n";
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
    
    static std::string readFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) { return ""; }

        std::stringstream stream;
        stream << file.rdbuf();
        return stream.str();
    }

public:
    CMentalShader() = default;
    CMentalShader(const std::string& vertexPath, const std::string& fragmentPath) {
        loadFromFiles(vertexPath, fragmentPath);
    }
    ~CMentalShader() = default;

    CMentalShader(const CMentalShader&) = delete;
    CMentalShader& operator=(const CMentalShader&) = delete;
    CMentalShader(CMentalShader&&) = delete;
    CMentalShader& operator=(CMentalShader&&) = delete;

    void loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
        std::cout << "Loading shaders: " << vertexPath << " and " << fragmentPath << "\n";
        const std::string& vertexData = this->readFile(vertexPath);
        const std::string& fragmentData = this->readFile(fragmentPath);
        
        if (vertexData.empty()) {
            std::cerr << "Error: Could not read vertex shader file: " << vertexPath << "\n";
            return;
        }
        if (fragmentData.empty()) {
            std::cerr << "Error: Could not read fragment shader file: " << fragmentPath << "\n";
            return;
        }
        
        std::cout << "Vertex shader loaded (" << vertexData.length() << " chars)\n";
        std::cout << "Fragment shader loaded (" << fragmentData.length() << " chars)\n";
        
        this->createShaderProgram(vertexData, fragmentData);
        
        if (this->isValid()) {
            std::cout << "Shader program created successfully with ID: " << programID_ << "\n";
        } else {
            std::cerr << "Failed to create shader program\n";
        }
    }

    void use() const;
    
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;
    
    [[nodiscard]] GLuint getProgramID() const { return programID_; }
    [[nodiscard]] bool isValid() const { return programID_ != 0; }
};

} // namespace mentalsdk