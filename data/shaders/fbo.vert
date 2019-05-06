#version 330 core

precision highp float;
precision highp int;

in vec3 Position;

void main() { 
    gl_Position = vec4(Position, 1.0); 
}