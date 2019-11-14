#version 330 core

out vec4 fragColor;

in vec2 TexCoord;
in vec3 rawPos;

uniform float theta;
uniform float phi;

void main() {
    vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    if (dot(circCoord, circCoord) > 1.0) {
        discard;
    }
    fragColor = vec4(0.5, 0.6, 0.9, 0.8);
}