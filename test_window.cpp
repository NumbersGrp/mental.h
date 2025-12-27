#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    std::cout << "Testing GLFW window creation..." << std::endl;
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // Try the most basic window creation
    GLFWwindow* window = glfwCreateWindow(800, 600, "Test Window", nullptr, nullptr);
    
    if (!window) {
        const char* description;
        int code = glfwGetError(&description);
        std::cerr << "GLFW Error " << code << ": " << description << std::endl;
        glfwTerminate();
        return -1;
    }
    
    std::cout << "Window created successfully!" << std::endl;
    
    glfwMakeContextCurrent(window);
    
    // Simple render loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}