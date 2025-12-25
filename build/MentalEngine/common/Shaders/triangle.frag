#version 330 core

in vec3 vertexColor;
out vec4 FragColor;

uniform float uTime;
uniform vec3 uTint;

void main()
{
    // Apply color tint and time-based animation
    vec3 finalColor = vertexColor * uTint;
    
    // Add subtle pulsing effect
    float pulse = 0.8 + 0.2 * sin(uTime * 2.0);
    finalColor *= pulse;
    
    FragColor = vec4(finalColor, 1.0);
}