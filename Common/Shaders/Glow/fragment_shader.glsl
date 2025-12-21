#version 330 core

in vec3 vertexColor;
in float glowIntensity;

uniform float time;
uniform vec4 objectColor;

out vec4 FragColor;

void main()
{
    // Создаем базовый цвет с учетом объектного цвета
    vec3 baseColor = vertexColor * objectColor.rgb;
    
    // Добавляем светящийся эффект
    vec3 glowColor = vec3(1.0, 0.8, 0.2); // Золотистое свечение
    
    // Пульсирующая интенсивность
    float pulse = 0.7 + 0.3 * sin(time * 4.0);
    
    // Смешиваем базовый цвет со свечением
    vec3 finalColor = baseColor + glowColor * glowIntensity * pulse * 0.6;
    
    // Добавляем внешнее свечение (bloom эффект)
    float bloomIntensity = glowIntensity * pulse * 0.4;
    finalColor += vec3(bloomIntensity);
    
    // Ограничиваем яркость чтобы избежать пересвета
    finalColor = min(finalColor, vec3(2.0));
    
    FragColor = vec4(finalColor, objectColor.a);
}