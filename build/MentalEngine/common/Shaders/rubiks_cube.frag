#version 330 core

in vec3 FragPos;
flat in vec3 Normal;  // Use flat interpolation for sharp edges
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uViewPos;
uniform vec3 uObjectColor;
uniform float uTime;
uniform sampler2D uTexture;
uniform bool uUseTexture;

// Material properties
uniform float uAmbientStrength;
uniform float uSpecularStrength;
uniform int uShininess;

vec3 getCubeColorByUV(vec2 uv) {
    // Use UV coordinates to determine color regions
    // This should work better with the complex cube geometry
    
    // Normalize UV to 0-1 range and determine which region we're in
    vec2 normalizedUV = fract(uv);
    
    // Create a grid pattern based on UV coordinates
    float u = normalizedUV.x;
    float v = normalizedUV.y;
    
    // Determine color based on UV regions
    if (u < 0.33) {
        if (v < 0.33) {
            return vec3(1.0, 0.0, 0.0); // Red
        } else if (v < 0.66) {
            return vec3(0.0, 1.0, 0.0); // Green
        } else {
            return vec3(0.0, 0.0, 1.0); // Blue
        }
    } else if (u < 0.66) {
        if (v < 0.33) {
            return vec3(1.0, 1.0, 0.0); // Yellow
        } else if (v < 0.66) {
            return vec3(1.0, 1.0, 1.0); // White
        } else {
            return vec3(1.0, 0.5, 0.0); // Orange
        }
    } else {
        // For now, just return a single color for the remaining region
        return vec3(0.5, 0.0, 0.5); // Purple
    }
}

void main()
{
    // Get base color based on UV coordinates for Rubik's cube
    vec3 baseColor;
    if (uUseTexture) {
        // For Rubik's cube, use UV-based coloring instead of texture
        baseColor = getCubeColorByUV(TexCoord);
    } else {
        baseColor = uObjectColor;
    }
    
    // Use the flat normal for lighting calculations
    vec3 norm = normalize(Normal);
    
    // Simplified lighting for clearer colors
    vec3 ambient = uAmbientStrength * uLightColor;
    
    // Diffuse lighting
    vec3 lightDir = normalize(uLightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor * 0.5; // Reduce diffuse contribution
    
    // Combine lighting with enhanced brightness
    vec3 result = (ambient + diffuse) * baseColor;
    
    // Ensure minimum visibility and boost brightness
    result = max(result, baseColor * 0.6); // Higher minimum brightness
    result = result * 1.1; // Slight brightness boost
    
    FragColor = vec4(result, 1.0);
}