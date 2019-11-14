#version 330 core

out vec4 fragColor;

in vec2 TexCoord;
in vec3 rawPos;

uniform float u_scale;

#define PI 3.14159265358979323846
#define INV_PI 0.31830988618
#define u_Gamma 1.0
#define u_Scale 1.0

#define sqr(a) ((a) * (a))
#define EPSILON 0.004

float cos_theta(vec3 w) { return w.z; }

float colormap_red(float x) {
    if (x < 0.7) {
        return 4.0 * x - 1.5;
    } else {
        return -4.0 * x + 4.5;
    }
}

float colormap_green(float x) {
    if (x < 0.5) {
        return 4.0 * x - 0.5;
    } else {
        return -4.0 * x + 3.5;
    }
}

float colormap_blue(float x) {
    if (x < 0.3) {
        return 4.0 * x + 0.5;
    } else {
        return -4.0 * x + 2.5;
    }
}

vec3 colormap(float x) {
    float r = clamp(colormap_red(x), 0.0, 1.0);
    float g = clamp(colormap_green(x), 0.0, 1.0);
    float b = clamp(colormap_blue(x), 0.0, 1.0);
    return vec3(r, g, b);
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                             Main                                          //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

mat4 rotation_matrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 0.0, oc * axis.x * axis.y + axis.z * s,
                oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s, 0.0, oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s,
                oc * axis.z * axis.z + c, 0.0, 0.0, 0.0, 0.0, 1.0);
}

void main() {

    vec3 n1 = normalize(rawPos);

    fragColor = vec4(colormap(u_scale * (abs(n1.z) + abs(n1.y) + abs(n1.x))), 0.7);
}