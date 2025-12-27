#pragma once
#include <memory>
#include <glm/glm.hpp>

namespace mentalsdk {
    template<typename T>
    using SharedPtr = std::shared_ptr<T>;

    template<typename T, typename... Args>
    auto shared(Args&&... args) -> std::shared_ptr<T> {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };
}