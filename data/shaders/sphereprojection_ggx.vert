#version 330 core

layout (location = 4) in vec2 aSample;

uniform mat4 mvp;

uniform float alpha_x;
uniform float alpha_y;
uniform float theta;
uniform float phi;

out float sampleValue;

#define PI     3.14159265358979323846


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

float GGX_D(vec3 wh, float a_x,float a_y) {
    
    float tan2Theta = Tan2Theta(wh);
    float cos4Theta = Cos2Theta(wh) * Cos2Theta(wh);
    float e = (Cos2Phi(wh) / (a_x * a_x) +
               Sin2Phi(wh) / (a_y * a_y)) * tan2Theta;
    return 1.0 / (PI * a_x * a_y * cos4Theta * (1.0 + e) * (1.0 + e));
}

// Input wi: view direction
// Input alpha_x, alpha_y: roughness parameters
// Input U1, U2: uniform random numbers
// Output Ne: normal sampled with PDF D_Ve(Ne) = G1(Ve) * max(0, dot(Ve, Ne)) * D(Ne) / Ve.z
vec3 Sample_GGX_VNDF(vec3 wi, float alpha_x, float alpha_y, vec2 uv){
    // Section 3.2: transforming the view direction to the hemisphere configuration
    vec3 Vh = normalize(vec3(alpha_x * wi.x, alpha_y * wi.y, wi.z));
    // Section 4.1: orthonormal basis
    vec3 T1 = (Vh.z < 0.9999) ? normalize(cross(vec3(0, 0, 1), Vh)) : vec3(1, 0, 0);
    vec3 T2 = cross(Vh, T1);
    // Section 4.2: parameterization of the projected area
    float r = sqrt(uv.x);
    float phi = 2.0 * PI * uv.y;
    float t1 = r * cos(phi);
    float t2 = r * sin(phi);
    float s = 0.5 * (1.0 + Vh.z);
    t2 = (1.0 - s)*sqrt(1.0 - t1*t1) + s*t2;
    // reprojection onto hemisphere
    vec3 Nh = t1*T1 + t2*T2 + sqrt(max(0.0, 1.0 - t1*t1 - t2*t2))*Vh;
    // transforming the normal back to the ellipsoid configuration
    vec3 Ne = normalize(vec3(alpha_x * Nh.x, alpha_y * Nh.y, max(0.0, Nh.z)));
    return Ne;
}

void main() {
    vec3 wi = normalize(vec3(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta)));

    vec3 wh = Sample_GGX_VNDF(wi,alpha_x,alpha_y,aSample);

    vec3 wo = reflect(-wi,wh);

    gl_Position = mvp * vec4(wo, 1.0); 

    if(wo.z > 0)
        sampleValue = GGX_D(wh,alpha_x,alpha_y)/GGX_D(vec3(0,0,1),alpha_x,alpha_y);
    else
        sampleValue = 0;

}