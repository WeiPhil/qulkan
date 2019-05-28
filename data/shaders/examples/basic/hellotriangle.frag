#version 330 core

out vec4 fragColor;

uniform float Transparency;

in vec3 ourColor;

void main() { 
    fragColor = vec4(ourColor,Transparency); 
}