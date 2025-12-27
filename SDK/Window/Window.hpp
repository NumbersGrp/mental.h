#pragma once

#include "../Renderer/Renderer.hpp"
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <memory>
#include <stdexcept>
#include "../Math/Math.hpp"

namespace mentalsdk
{

const int DEFAULT_WINDOW_WIDTH = 800;
const int DEFAULT_WINDOW_HEIGHT = 600;

const int DEFAULT_GLFW_CONTEXT_VERSION_MAJOR = 3;
const int DEFAULT_GLFW_CONTEXT_VERSION_MINOR = 3;

struct GLFWWindowDeleter {
    void operator()(GLFWwindow* window) const {
        if (window != nullptr) {
            glfwDestroyWindow(window);
        }
    }
};

template <typename R>
class CMentalWindow
{
private:
    std::unique_ptr<GLFWwindow, GLFWWindowDeleter> window_ = nullptr;
    std::shared_ptr<Vector2<int>> sizes_ = mentalsdk::vec2(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    std::shared_ptr<R> render_pool_ = nullptr;
public:
    explicit CMentalWindow() {
        if (glfwInit() == GLFW_FALSE) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, DEFAULT_GLFW_CONTEXT_VERSION_MAJOR);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, DEFAULT_GLFW_CONTEXT_VERSION_MINOR);
        
        GLFWwindow* window = glfwCreateWindow(this->sizes_.get()->x, this->sizes_.get()->y, 
                                            "Mental Engine", nullptr, nullptr);
        if (window == nullptr) {
            const char* description = nullptr;
            int code = glfwGetError(&description);
            std::cerr << "GLFW Error " << code << ": " << description << "\n";
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }
        
        window_.reset(window);
        glfwMakeContextCurrent(window);
        glEnable(GL_DEPTH_TEST);
    }

    ~CMentalWindow() {
        window_.reset();
        glfwTerminate();
    }

    CMentalWindow(const CMentalWindow&) = delete;
    CMentalWindow& operator=(const CMentalWindow&) = delete;
    CMentalWindow(CMentalWindow&&) = delete;
    CMentalWindow& operator=(CMentalWindow&&) = delete;

    
    void destroyWindow() { 
        if (window_ != nullptr) {
            window_.reset();
        }
    }
    static void pollEvents() { glfwPollEvents(); }
    void swapBuffers() { glfwSwapBuffers(this->window_.get()); }
    
    [[nodiscard]] bool shouldClose() const { return glfwWindowShouldClose(this->window_.get()) != 0;};
    [[nodiscard]] std::shared_ptr<Vector2<int>> getWindowSize() const { return this->sizes_; }
    [[nodiscard]] GLFWwindow* getWindow() const { return this->window_.get(); }

    void setWindow(GLFWwindow* pWindow) { 
        window_.reset(pWindow);
    }
    std::shared_ptr<R> getRenderPool() { return this->render_pool_; }
    void setRenderPool(std::shared_ptr<R> render_pool) { this->render_pool_ = render_pool; }
    void run();
};

template <typename R>
void CMentalWindow<R>::run() {
    while (!this->shouldClose()) {

        if (render_pool_) {
            render_pool_->render();
        }

        this->pollEvents();
        this->swapBuffers();
    }
}

} // namespace mentalsdk
