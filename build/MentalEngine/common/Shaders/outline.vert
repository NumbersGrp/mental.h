#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform float uOutlineWidth;

void main()
{
    // Expand vertices along normals for outline effect
    vec3 expandedPos = aPos + normalize(aNormal) * uOutlineWidth;
    
    gl_Position = uProjection * uView * uModel * vec4(expandedPos, 1.0);
}