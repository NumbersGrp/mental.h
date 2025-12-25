#version 330 core

in vec3 FragPos;
in vec3 Normal;
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

void main()
{
    // Debug mode: Show UV coordinates as colors (uncomment to debug)
    // Uncomment the next two lines to see UV coordinates as colors
    // FragColor = vec4(TexCoord, 0.0, 1.0);
    // return;
    
    // Get base color from texture or uniform
    vec3 baseColor;
    if (uUseTexture) {
        vec4 texColor = texture(uTexture, TexCoord);
        baseColor = texColor.rgb;
        // If texture has alpha, we could use it for transparency
        if (texColor.a < 0.1) {
            discard; // Discard transparent pixels
        }
    } else {
        baseColor = uObjectColor;
    }
    
    // Ensure normal is normalized and handle two-sided lighting
    vec3 norm = normalize(Normal);
    
    // For two-sided lighting, flip normal if facing away
    if (!gl_FrontFacing) {
        norm = -norm;
    }
    
    // Enhanced lighting model with brighter results
    vec3 ambient = uAmbientStrength * uLightColor;
    
    // Diffuse lighting
    vec3 lightDir = normalize(uLightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;
    
    // Specular lighting
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), float(uShininess));
    vec3 specular = uSpecularStrength * spec * uLightColor;
    
    // Combine lighting with enhanced brightness
    vec3 result = (ambient + diffuse + specular) * baseColor;
    
    // Boost overall brightness and ensure minimum visibility
    result = result * 1.3; // Increase overall brightness
    result = max(result, baseColor * 0.5); // Higher minimum brightness
    
    // Gamma correction for better color representation
    result = pow(result, vec3(1.0/2.2));
    
    FragColor = vec4(result, 1.0);
}