#ifndef MENTAL_SHADERS_H
#define MENTAL_SHADERS_H

#ifdef __cplusplus
extern "C" {
#endif

// Simple vertex shader for 2D rectangles
static const char* rectangleVertexShader = 
    "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 vertexColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
    "   vertexColor = aColor;\n"
    "}\0";

// Simple fragment shader for 2D rectangles
static const char* rectangleFragmentShader = 
    "#version 330 core\n"
    "in vec3 vertexColor;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(vertexColor, 1.0);\n"
    "}\0";

#ifdef __cplusplus
}
#endif

#endif // MENTAL_SHADERS_H
