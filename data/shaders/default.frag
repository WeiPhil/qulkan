#version 330 core

out vec4 fragColor;

uniform float Transparency;

void main() { 
    fragColor = vec4(1.0,1.0,1.0,Transparency); 
}