/**
 * @file CMentalRenderer.hpp
 * @brief OpenGL renderer component for the Mental SDK
 * @author Mental SDK Team
 * @version 1.0.0
 */

#pragma once

#include <cstdint>
#include <memory>
#include <GL/glew.h>
#include <stdexcept>
#include "RenderPipeline/CMentalRenderPipeline.hpp"

const uint32_t MINIMAL_OPENGL_VERSION_MAJOR = 3;
const uint32_t MINIMAL_OPENGL_VERSION_MINOR = 3;

namespace mentalsdk
{

/**
 * @struct MentalRendererInfo
 * @brief Configuration structure for the renderer
 */
struct MentalRendererInfo {
    int opengl_version[2] = {MINIMAL_OPENGL_VERSION_MAJOR, MINIMAL_OPENGL_VERSION_MINOR}; ///< OpenGL version [major, minor]
};

/**
 * @class CMentalRenderer
 * @brief Template-based OpenGL renderer class
 * @tparam P Pipeline type for custom rendering pipeline
 * 
 * This class manages OpenGL initialization and provides access to a custom
 * rendering pipeline. It uses GLEW for OpenGL extension loading.
 * 
 * @note This class is non-copyable and non-movable
 * 
 * Example usage:
 * @code
 * CMentalRenderer<MyPipeline> renderer;
 * renderer.initializeOpenGL();
 * auto* pipeline = renderer.getRenderPipeline();
 * @endcode
 */
template <class P>
class CMentalRenderer
{
private:
    std::unique_ptr<P> up_renderpipeline_; ///< Unique pointer to the render pipeline

public:
    /**
     * @brief Default constructor
     */
    explicit CMentalRenderer() = default;
    
    /**
     * @brief Destructor
     */
    ~CMentalRenderer() = default;

    /// @brief Deleted copy constructor
    CMentalRenderer(const CMentalRenderer&) = delete;
    
    /// @brief Deleted copy assignment operator
    CMentalRenderer& operator=(const CMentalRenderer&) = delete;
    
    /// @brief Deleted move constructor
    CMentalRenderer(CMentalRenderer&&) = delete;
    
    /// @brief Deleted move assignment operator
    CMentalRenderer& operator=(CMentalRenderer&&) = delete;

    /**
     * @brief Initialize OpenGL using GLEW
     * @throws std::runtime_error if GLEW initialization fails
     * 
     * This method must be called after creating an OpenGL context.
     * It initializes GLEW with experimental features enabled.
     */
    void initializeOpenGL();

    void initializePipeline(const MentalRenderPipelineInfo& info);
    
    /**
     * @brief Get a pointer to the render pipeline
     * @return Pointer to the render pipeline object
     */
    P* getRenderPipeline() { return up_renderpipeline_.get(); }
    
    /**
     * @brief Get a const pointer to the render pipeline
     * @return Const pointer to the render pipeline object
     */
    const P* getRenderPipeline() const { return up_renderpipeline_.get(); }
};

// Template implementation
template <class P>
void CMentalRenderer<P>::initializeOpenGL() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW library.");
    }
}

template<class P>
void CMentalRenderer<P>::initializePipeline(const MentalRenderPipelineInfo& info)
{
    this->up_renderpipeline_.reset(new P(info));
}

} // namespace mentalsdk