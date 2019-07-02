#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 4) in vec2 aTextCoord;

out vec2 TexCoord;
out vec3 rawPos;

uniform mat4 mvp;

void main() {

    gl_Position = mvp * vec4(aPosition, 1.0); 
    TexCoord = aTextCoord;
    rawPos = aPosition;
}