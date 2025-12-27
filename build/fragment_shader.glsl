#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

void main() {
    // Bright cyan color - hot reload test!
    FragColor = vec4(0.2, 0.3, 1.0, 1.0);
}