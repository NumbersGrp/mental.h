#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;
out vec2 fragPos;

uniform float time;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    // Добавляем небольшую анимацию позиции
    vec3 animatedPos = aPos;
    animatedPos.x += sin(time + aPos.y * 3.14159) * 0.1;
    animatedPos.y += cos(time + aPos.x * 3.14159) * 0.1;
    
    // Применяем матрицы камеры
    gl_Position = projectionMatrix * viewMatrix * vec4(animatedPos, 1.0);
    vertexColor = aColor;
    fragPos = aPos.xy;
}