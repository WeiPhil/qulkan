#version 330 core

out vec4 fragColor;

uniform float Mix;

uniform sampler2D texture1;
uniform sampler2D texture2;

in vec2 TexCoord;

void main() { 
    fragColor = mix(texture(texture1, TexCoord),texture(texture2, TexCoord*2.0-vec2(0.0,0.5)),Mix);
}