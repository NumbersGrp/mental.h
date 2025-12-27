#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace mentalsdk {

enum MentalEnvironmentType : uint8_t {
    ClearColor = 0,
};

class CMentalEnvironment
{
private:
    MentalEnvironmentType type_ = MentalEnvironmentType::ClearColor;
    float color_[3] = {1.0F, 1.0F, 1.0F};

public:
    CMentalEnvironment() = default;
    ~CMentalEnvironment() = default;

    CMentalEnvironment(const CMentalEnvironment&) = delete;
    CMentalEnvironment& operator=(const CMentalEnvironment&) = delete;
    CMentalEnvironment(CMentalEnvironment&&) = delete;
    CMentalEnvironment& operator=(CMentalEnvironment&&) = delete;

    void setColor(float red = 1.0F, float green = 1.0F, float blue = 1.0F, float alpha = 1.0F) {
        this->color_[0] = red; this->color_[1] = green; this->color_[2] = blue; this->color_[4] = alpha;
    }

    void renderClearColor() {
        glClearColor(this->color_[0], this->color_[1], this->color_[2], this->color_[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
};

} // mentalsdk
