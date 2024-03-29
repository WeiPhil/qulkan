#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 4) in vec2 aTextCoord;

out vec2 TexCoord;


uniform mat4 transform;

void main() { 
    gl_Position = transform*vec4(aPosition, 1.0); 
    TexCoord = aTextCoord;
}