#pragma once

#include <GL/glew.h>
#include <string>

namespace mentalsdk
{

class CMentalTexture
{
private:
    GLuint textureID_ = 0;
    int width_ = 0;
    int height_ = 0;
    int channels_ = 0;

public:
    CMentalTexture() = default;
    ~CMentalTexture() {
        if (textureID_ != 0) {
            glDeleteTextures(1, &textureID_);
        }
    }

    CMentalTexture(const CMentalTexture&) = delete;
    CMentalTexture& operator=(const CMentalTexture&) = delete;
    CMentalTexture(CMentalTexture&&) = delete;
    CMentalTexture& operator=(CMentalTexture&&) = delete;

    bool loadFromFile(const std::string& filePath);
    
    void bind(unsigned int unit = 0) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, textureID_);
    }
    
    void unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    [[nodiscard]] GLuint getID() const { return textureID_; }
    [[nodiscard]] bool isValid() const { return textureID_ != 0; }
    [[nodiscard]] int getWidth() const { return width_; }
    [[nodiscard]] int getHeight() const { return height_; }
};

} // mentalsdk
