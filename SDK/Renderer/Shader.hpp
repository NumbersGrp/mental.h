#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace mentalsdk
{

class CShader
{
private:
    GLuint programID_ = 0;
    
    GLuint compileShader(const std::string& source, GLenum shaderType);
    std::string readFile(const std::string& filePath);

public:
    CShader() = default;
    ~CShader();

    CShader(const CShader&) = delete;
    CShader& operator=(const CShader&) = delete;
    CShader(CShader&&) = delete;
    CShader& operator=(CShader&&) = delete;

    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;
    
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;
    
    [[nodiscard]] GLuint getProgramID() const { return programID_; }
    [[nodiscard]] bool isValid() const { return programID_ != 0; }
};

} // namespace mentalsdk