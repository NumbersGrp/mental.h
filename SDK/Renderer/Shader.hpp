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

class CShader
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
    CShader() = default;
    ~CShader();

    CShader(const CShader&) = delete;
    CShader& operator=(const CShader&) = delete;
    CShader(CShader&&) = delete;
    CShader& operator=(CShader&&) = delete;

    void loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
        const std::string& vertexData = this->readFile(vertexPath);
        const std::string& fragmentData = this->readFile(fragmentPath);
        this->createShaderProgram(vertexData, fragmentData);
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