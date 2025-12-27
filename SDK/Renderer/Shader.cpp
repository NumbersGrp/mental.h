#include "Shader.hpp"

namespace mentalsdk {

void CMentalShader::use() const {
    if (programID_ != 0) {
        glUseProgram(programID_);
    }
}

void CMentalShader::setMat4(const std::string& name, const glm::mat4& mat) const {
    if (programID_ != 0) {
        GLint location = glGetUniformLocation(programID_, name.c_str());
        if (location != -1) {
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
        }
    }
}

void CMentalShader::setVec3(const std::string& name, const glm::vec3& value) const {
    if (programID_ != 0) {
        GLint location = glGetUniformLocation(programID_, name.c_str());
        if (location != -1) {
            glUniform3fv(location, 1, glm::value_ptr(value));
        }
    }
}

void CMentalShader::setFloat(const std::string& name, float value) const {
    if (programID_ != 0) {
        GLint location = glGetUniformLocation(programID_, name.c_str());
        if (location != -1) {
            glUniform1f(location, value);
        }
    }
}

void CMentalShader::setInt(const std::string& name, int value) const {
    if (programID_ != 0) {
        GLint location = glGetUniformLocation(programID_, name.c_str());
        if (location != -1) {
            glUniform1i(location, value);
        }
    }
}

} // namespace mentalsdk