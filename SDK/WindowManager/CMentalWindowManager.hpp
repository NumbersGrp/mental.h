/**
 * @file CMentalWindowManager.hpp
 * @brief Window management component for the Mental SDK
 * @author Mental SDK Team
 * @version 1.0.0
 */

#pragma once

#include <memory>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <exception>
#include "../Renderer/RenderPipeline/CMentalRenderPipeline.hpp"

const uint32_t MINIMAL_WINDOW_SIZE_WIDTH = 800;
const uint32_t MINIMAL_WINDOW_SIZE_HEIGHT = 600;
const uint8_t  ENABLE_VSYNC = 1;

namespace mentalsdk
{

/// @brief Type alias for GLFW window structure
using GLFWwindow = struct GLFWwindow;

/**
 * @struct WindowManagerInfo
 * @brief Configuration structure for window creation
 */
struct WindowManagerInfo {
    int aSizes[2] = {MINIMAL_WINDOW_SIZE_WIDTH, MINIMAL_WINDOW_SIZE_HEIGHT};                    ///< Window dimensions [width, height]
    std::string title = "Mental Game Engine";      ///< Window title
    int enable_vsync = ENABLE_VSYNC;                          ///< Enable vertical synchronization (1 = enabled, 0 = disabled)
};

/**
 * @class CMentalWindowManager
 * @brief Template-based window management class using GLFW
 * @tparam T Window type (typically GLFWwindow)
 * @tparam R Renderer type (typically CMentalRenderer<P>)
 * 
 * This class manages window creation, OpenGL context setup, and integrates
 * with a renderer component. It provides a complete windowing solution with
 * event handling and rendering loop.
 * 
 * @note This class is non-copyable and non-movable
 * 
 * Key features:
 * - GLFW window creation and management
 * - OpenGL context setup (version 3.3 core profile)
 * - Integrated renderer initialization
 * - Event polling and buffer swapping
 * - Main rendering loop
 * 
 * Example usage:
 * @code
 * WindowManagerInfo info{};
 * info.aSizes[0] = 1024;
 * info.aSizes[1] = 768;
 * info.title = "My Game";
 * 
 * CMentalWindowManager<GLFWwindow, CMentalRenderer<int>> window(info);
 * window.run(); // Start the main loop
 * @endcode
 */
template <typename T, class R>
class CMentalWindowManager
{
private:
    std::unique_ptr<T, void(*)(T*)> up_window_;    ///< Unique pointer to window with custom deleter
    std::unique_ptr<R> up_renderer_;               ///< Unique pointer to renderer

public:
    /**
     * @brief Constructor that creates and initializes the window
     * @param info Configuration structure containing window parameters
     * @throws std::runtime_error if GLFW initialization fails
     * @throws std::runtime_error if window creation fails
     * @throws std::runtime_error if renderer initialization fails
     * 
     * This constructor:
     * 1. Initializes GLFW
     * 2. Sets OpenGL context hints (version 3.3 core profile)
     * 3. Creates the window
     * 4. Makes the OpenGL context current
     * 5. Sets up VSync
     * 6. Initializes the renderer
     */
    explicit CMentalWindowManager(const struct WindowManagerInfo& info);
    
    /**
     * @brief Destructor that cleans up resources
     * 
     * Automatically destroys the window (via custom deleter) and terminates GLFW.
     */
    ~CMentalWindowManager();

    /// @brief Deleted copy constructor
    CMentalWindowManager(const CMentalWindowManager&) = delete;
    
    /// @brief Deleted copy assignment operator
    CMentalWindowManager& operator=(const CMentalWindowManager&) = delete;
    
    /// @brief Deleted move constructor
    CMentalWindowManager(CMentalWindowManager&&) = delete;
    
    /// @brief Deleted move assignment operator
    CMentalWindowManager& operator=(CMentalWindowManager&&) = delete;

    /**
     * @brief Check if the window should close
     * @return true if the window should close, false otherwise
     * 
     * This typically returns true when the user clicks the close button
     * or presses Alt+F4 (on Windows).
     */
    bool shouldClose() const;
    
    /**
     * @brief Poll for window events
     * 
     * Processes all pending window events such as keyboard input,
     * mouse movement, window resize, etc.
     */
    void pollEvents();
    
    /**
     * @brief Swap the front and back buffers
     * 
     * This displays the rendered frame to the screen. Should be called
     * after rendering is complete for each frame.
     */
    void swapBuffers();
    
    /**
     * @brief Clear the framebuffer with specified color
     * @param red Red component (0.0 to 1.0)
     * @param green Green component (0.0 to 1.0)
     * @param blue Blue component (0.0 to 1.0)
     * @param alpha Alpha component (0.0 to 1.0)
     * 
     * Clears both the color buffer and depth buffer with the specified color.
     */
    void clear(float red = 0.2F, float green = 0.3F, float blue = 0.2F, float alpha = 1.0F);
    
    /**
     * @brief Set the window title
     * @param title New window title
     */
    void setTitle(const std::string& title);
    
    /**
     * @brief Get the current window size
     * @param[out] width Reference to store the window width
     * @param[out] height Reference to store the window height
     * 
     * If the window is invalid, both width and height are set to 0.
     */
    void getWindowSize(int& width, int& height) const;
    
    /**
     * @brief Start the main rendering loop
     * 
     * This method runs the main game/application loop:
     * 1. Clear the framebuffer
     * 2. Poll for events
     * 3. Swap buffers
     * 4. Repeat until window should close
     * 
     * @note This method blocks until the window is closed
     */
    void run();

    /**
     * @brief Get a pointer to the renderer
     * @return Pointer to the renderer object
     */
    R* getRenderer() { return up_renderer_.get(); }
    
    /**
     * @brief Get a const pointer to the renderer
     * @return Const pointer to the renderer object
     */
    const R* getRenderer() const { return up_renderer_.get(); }
};

// Template implementations
template <typename T, class R>
CMentalWindowManager<T, R>::CMentalWindowManager(const struct WindowManagerInfo& info)
: up_window_(nullptr, [](T* ptr) { 
    if (ptr) { 
        glfwDestroyWindow(ptr); 
    } 
}),
  up_renderer_(std::make_unique<R>())
{
    if (glfwInit() == 0) { 
        throw std::runtime_error("Failed to initialize GLFW library."); 
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GL_TRUE);

    T* window = glfwCreateWindow(info.aSizes[0], info.aSizes[1], info.title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window.");
    }
    up_window_.reset(window);
    glfwMakeContextCurrent(up_window_.get());
    glfwSwapInterval(info.enable_vsync);

    try {
        this->up_renderer_->initializeOpenGL();
        
        // Create default pipeline info
        MentalRenderPipelineInfo pipelineInfo{};
        pipelineInfo.name = "DefaultPipeline";
        this->up_renderer_->initializePipeline(pipelineInfo);
    } catch (const std::exception& exception) {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize renderer: " + std::string(exception.what()));
    }
}

template<typename T, class R>
CMentalWindowManager<T, R>::~CMentalWindowManager()
{
    // Window is automatically destroyed by the unique_ptr's custom deleter
    // Just terminate GLFW
    glfwTerminate();
}

template<typename T, class R>
bool CMentalWindowManager<T, R>::shouldClose() const {
    if (!up_window_) { return true; }
    return glfwWindowShouldClose(up_window_.get());
}

template<typename T, class R>
void CMentalWindowManager<T, R>::pollEvents() {
    glfwPollEvents();
}

template<typename T, class R>
void CMentalWindowManager<T, R>::swapBuffers() {
    if (this->up_window_)
    {
        glfwSwapBuffers(this->up_window_.get());
    }
}

template<typename T, class R>
void CMentalWindowManager<T, R>::clear(float red, float green, float blue, float alpha) {
    glClearColor(red, green, blue, alpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

template<typename T, class R>
void CMentalWindowManager<T, R>::setTitle(const std::string& title) {
    if (this->up_window_) {
        glfwSetWindowTitle(this->up_window_.get(), title.c_str());
    }
}

template<typename T, class R>
void CMentalWindowManager<T, R>::getWindowSize(int& width, int& height) const {
    if (this->up_window_) {
        glfwGetWindowSize(this->up_window_.get(), &width, &height);
    } else {
        width = height = 0;
    }
}

template<typename T, class R>
void CMentalWindowManager<T, R>::run() {
    while (!glfwWindowShouldClose(this->up_window_.get())) {
        this->clear();
        this->pollEvents();
        this->swapBuffers();
    }
}

} // namespace mentalsdk