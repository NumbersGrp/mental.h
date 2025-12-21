#pragma once
#include "../Object/CMentalObject.hpp"
#include <cstdint>
#include <vector>

const uint32_t MINIMAL_DEFAULT_RESERVED_FOR_PIPELINE = 10;

namespace mentalsdk
{

struct MentalRenderPipelineInfo {
    std::string name;
    uint32_t reserved = MINIMAL_DEFAULT_RESERVED_FOR_PIPELINE;
};

template <class O = CMentalObject>
class CMentalRenderPipeline
{
private:
    std::vector<std::unique_ptr<O>> render_objects_; ///< Vector of unique pointers to render objects
    MentalRenderPipelineInfo info_; ///< Pipeline configuration
public:
    explicit CMentalRenderPipeline(const MentalRenderPipelineInfo& info);
    ~CMentalRenderPipeline();

    /// @brief Deleted copy constructor
    CMentalRenderPipeline(const CMentalRenderPipeline&) = delete;
    
    /// @brief Deleted copy assignment operator
    CMentalRenderPipeline& operator=(const CMentalRenderPipeline&) = delete;
    
    /// @brief Deleted move constructor
    CMentalRenderPipeline(CMentalRenderPipeline&&) = delete;
    
    /// @brief Deleted move assignment operator
    CMentalRenderPipeline& operator=(CMentalRenderPipeline&&) = delete;

    void appendObject(std::unique_ptr<O> object);
    O* getObject(const std::string& name);
    std::string getName() const { return this->info_.name; }
};

template<class O>
CMentalRenderPipeline<O>::CMentalRenderPipeline(const MentalRenderPipelineInfo& info) : info_(info) {
    render_objects_.reserve(info.reserved);
}

template<class O>
CMentalRenderPipeline<O>::~CMentalRenderPipeline() = default;

template<class O>
void CMentalRenderPipeline<O>::appendObject(std::unique_ptr<O> object) {
    render_objects_.emplace_back(std::move(object));
}

template<class O>
O* CMentalRenderPipeline<O>::getObject(const std::string& name) {
    for (auto& obj : render_objects_) {
        if (obj && obj->getName() == name) {
            return obj.get();
        }
    }
    return nullptr; // Return nullptr if not found
}


} // namespace mentalsdk