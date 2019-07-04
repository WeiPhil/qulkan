#version 330 core

out vec4 fragColor;

in vec2 TexCoord;
in vec3 rawPos;

uniform float alpha_x;
uniform float alpha_y;

uniform float theta;
uniform float phi;

#define PI     3.14159265358979323846
#define INV_PI 0.31830988618
#define u_Gamma 1.0
#define u_Scale 1.0

#define sqr(a) ( (a)*(a) )
#define EPSILON 0.004

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                             GGX Distribution                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

float CosTheta(vec3 w) { return w.z; }

float Cos2Theta(vec3 w) { return w.z * w.z; }

float Sin2Theta(vec3 w) {
    return max(0.0, 1.0 - Cos2Theta(w));
}

float SinTheta(vec3 w){
    return sqrt(Sin2Theta(w));
}

float Tan2Theta(vec3 w) {
    return Sin2Theta(w) / Cos2Theta(w);
}

float CosPhi(vec3 w) {
    float sinTheta = SinTheta(w);
    return ((sinTheta == 0.0) ? 1.0 : clamp(w.x / sinTheta, -1.0, 1.0));
}

float SinPhi(vec3 w) {
    float sinTheta = SinTheta(w);
    return ((sinTheta == 0.0) ? 0.0 : clamp(w.y / sinTheta, -1.0, 1.0));
}

float Cos2Phi(vec3 w) {
    return CosPhi(w) * CosPhi(w);
}
float Sin2Phi(vec3 w) {
    return SinPhi(w) * SinPhi(w);
}

float GGX_D(vec3 wi, vec3 wo, float a_x,float a_y) {
    vec3  wh     = normalize(wi+wo);
    
    float tan2Theta = Tan2Theta(wh);
    float cos4Theta = Cos2Theta(wh) * Cos2Theta(wh);
    float e = (Cos2Phi(wh) / (a_x * a_x) +
               Sin2Phi(wh) / (a_y * a_y)) * tan2Theta;
    return 1.0 / (PI * a_x * a_y * cos4Theta * (1.0 + e) * (1.0 + e));
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                            Jet Colormap                                   //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

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

void main() { 
    vec3 wi = normalize(vec3(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta)));

    vec3 wo = normalize(rawPos);

    // if(wo.z < 0)
        fragColor = vec4(colormap(0.0), 0.1);
    // else
    //     fragColor = vec4(colormap(min( GGX_D(wi,wo,alpha_x,alpha_y), 1.0 )), 0.75);

}