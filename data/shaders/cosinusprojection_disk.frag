#version 330 core

precision highp float;
precision highp int;

out vec4 fragColor;

in vec3 diskColor;


void main() { 
    fragColor = vec4(diskColor,1.0);
}