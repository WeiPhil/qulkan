#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 4) in vec2 aUV;

out vec3 ourColor;
out vec2 vPos;

void main() { 
    gl_Position = vec4(aPosition, 1.0); 
    vPos = aUV;
}