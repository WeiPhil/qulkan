#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 4) in vec2 aTextCoord;

out vec2 TexCoord;
out vec3 rawPos;

uniform mat4 mvp;

uniform mat3 transform;

void main() {
    gl_PointSize = 3;

    vec3 warped = transform * aPosition;
    gl_Position = mvp * vec4(warped, 1.0);
    TexCoord = aTextCoord;
    rawPos = aPosition;
}