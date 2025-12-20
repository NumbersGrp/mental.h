#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;
out vec2 fragPos;

uniform float time;

void main()
{
    // Добавляем небольшую анимацию позиции
    vec2 animatedPos = aPos;
    animatedPos.x += sin(time + aPos.y * 3.14159) * 0.1;
    animatedPos.y += cos(time + aPos.x * 3.14159) * 0.1;
    
    gl_Position = vec4(animatedPos, 0.0, 1.0);
    vertexColor = aColor;
    fragPos = aPos;
}