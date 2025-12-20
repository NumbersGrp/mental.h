#version 330 core

in vec3 vertexColor;
in vec2 fragPos;
out vec4 FragColor;

uniform float time;

vec3 rainbow(float t) {
    t = fract(t);
    float r = abs(t * 6.0 - 3.0) - 1.0;
    float g = 2.0 - abs(t * 6.0 - 2.0);
    float b = 2.0 - abs(t * 6.0 - 4.0);
    return clamp(vec3(r, g, b), 0.0, 1.0);
}

void main()
{
    // Создаем радужный эффект на основе позиции и времени
    float dist = length(fragPos);
    float angle = atan(fragPos.y, fragPos.x);
    
    // Радужная спираль
    float rainbowTime = time * 0.5 + dist * 2.0 + angle * 0.5;
    vec3 rainbowColor = rainbow(rainbowTime);
    
    // Смешиваем с исходным цветом вершины
    vec3 finalColor = mix(vertexColor, rainbowColor, 0.7);
    
    // Добавляем пульсацию яркости
    float pulse = sin(time * 3.0) * 0.2 + 0.8;
    finalColor *= pulse;
    
    FragColor = vec4(finalColor, 1.0);
}