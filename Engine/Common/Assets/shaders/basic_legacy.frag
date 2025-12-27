#version 120

varying vec3 FragPos;
varying vec3 Normal;
varying vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform sampler2D texture1;
uniform int useTexture;

void main()
{
    // Get base color from texture or uniform
    vec3 baseColor;
    if (useTexture == 1) {
        baseColor = texture2D(texture1, TexCoord).rgb;
    } else {
        baseColor = objectColor;
    }
    
    // ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
    
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    vec3 result = (ambient + diffuse) * baseColor;
    gl_FragColor = vec4(result, 1.0);
}