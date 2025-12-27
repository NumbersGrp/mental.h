#pragma once

#include <string>
#include <GL/glew.h>

namespace mentalsdk
{

class CTexture
{
private:
    GLuint textureID_ = 0;
    int width_ = 0;
    int height_ = 0;
    int channels_ = 0;
    std::string filePath_;

public:
    CTexture() = default;
    ~CTexture();

    CTexture(const CTexture&) = delete;
    CTexture& operator=(const CTexture&) = delete;
    CTexture(CTexture&&) = delete;
    CTexture& operator=(CTexture&&) = delete;

    bool loadFromFile(const std::string& filePath);
    void bind(GLuint textureUnit = 0) const;
    void unbind() const;
    
    [[nodiscard]] GLuint getTextureID() const { return textureID_; }
    [[nodiscard]] bool isValid() const { return textureID_ != 0; }
    [[nodiscard]] int getWidth() const { return width_; }
    [[nodiscard]] int getHeight() const { return height_; }
    [[nodiscard]] const std::string& getFilePath() const { return filePath_; }
};

} // namespace mentalsdk