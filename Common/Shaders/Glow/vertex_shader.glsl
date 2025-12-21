#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float time;

out vec3 vertexColor;
out float glowIntensity;

void main()
{
    // Создаем пульсирующий эффект свечения
    glowIntensity = 0.5 + 0.5 * sin(time * 3.0);
    
    // Передаем цвет с учетом интенсивности свечения
    vertexColor = aColor * (1.0 + glowIntensity * 0.5);
    
    // Применяем матрицы камеры
    gl_Position = projectionMatrix * viewMatrix * vec4(aPos, 1.0);
}