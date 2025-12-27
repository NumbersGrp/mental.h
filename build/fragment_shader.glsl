#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

void main() {
    // Bright magenta color - impossible to miss!
    FragColor = vec4(1.0, 0.0, 1.0, 1.0);
}