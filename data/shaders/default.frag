#version 330 core

uniform float Transparency;

out vec4 color;

in vec3 ourColor;
void main() { 
    color = vec4(ourColor.xy,Transparency,Transparency); 
}