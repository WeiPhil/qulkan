#version 330 core
precision highp float;

in vec2 vPos;
out vec4 fragColor;

uniform vec3  wi;

#define NB_SAMPLES 25600
#define NB_PER_RUN 512

// Rougness from light bounce on first layer 
uniform float  u_alpha_x_1;
uniform float  u_alpha_y_1;

// Rougness from light bounce on second layer
uniform float  u_alpha_x_2;
uniform float  u_alpha_y_2;

uniform float  u_Scale;
uniform float  u_Gamma;

uniform int u_PassNumber;

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                              Common constants                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#define PI     3.14159265358979323846
#define INV_PI 0.31830988618
#define EPSILON 0.004

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                        Common helper functions                            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

vec2 QMC_Additive_2D(int n) {
    return mod(vec2(0.5545497, 0.308517) * float(n), vec2(1.0, 1.0));
}

float sqr(float x) {
    return x*x;
}

vec2 sqr(vec2 x) {
    return x*x;
}

vec3 sqr(vec3 x) {
    return x*x;
}

/* Create a tangent frame 't', 'b', 'n' from a normal 'n' using Jeppe 
 * Frisvad's method.
 */
void FrameFrisvad(in vec3 n, out vec3 t, out vec3 b) {
    if(n.z < -0.9999999) { // Handle the singularity
        t = vec3( 0.0, -1.0, 0.0);
        b = vec3(-1.0,  0.0, 0.0);
        return;
    }
    float x = 1.0/(1.0 + n.z);
    float y = -n.x*n.y*x;
    t = vec3(1.0 - n.x*n.x*x, y, -n.x);
    b = vec3(y, 1.0 - n.y*n.y*x, -n.y);
}

/* Spherical Gaussian
 */
float SphericalGaussian(vec3 x, vec3 mu, float var) {
    float k   = 1.0 / var;
    float xmu = dot(x, mu);
    float C3k = k / (2.0*PI * (exp(k) - exp(-k)));
    return exp(k * (dot(x, mu) - 1.0));
}

/* Tangential Gaussian distribution
 */
float TangentialGaussian(vec3 w, vec3 wi, float dev) {

    vec3 t, b, n;
    FrameFrisvad(wi, t, b);

    float dotWT = dot(w, t);
    float dotWB = dot(w, b);
    float dotWN = dot(w, wi);
    float sini = sqrt(sqr(dotWT) + sqr(dotWB));
    float cosi = dotWN;
    if(cosi < 0.0) {
        return 0.0;
    }

    float dist = sini/cosi;
    return exp(- 0.5 * sqr(dist / dev));
}


/* Projected Disc Gaussian distribution
 */
float ProjectedDiscGaussian(vec3 w, vec3 wi, float dev) {

    float dist = length(w.xy - wi.xy);
    return exp(- 0.5 * sqr(dist / dev));
}


int mod(int n, int N) {
    return n - N*(n / N);
}


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                         Common math functions                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

float sinh(float x) {
    return 0.5 * (1.0 - exp(- 2.0 * x)) / exp(- x);
}

float cosh(float x) {
    return 0.5 * (1.0 + exp(- 2.0 * x)) / exp(- x);
}

float coth(float x) {
    return (1.0 - exp(- 2.0 * x)) / (1.0 + exp(- 2.0 * x));
}

float erf(float x) {
   float x2 = x*x;
   float a  = 0.14;
   return sqrt(1.0 - exp(- x2 * (4.0/PI + a*x2) / (1.0 + a*x2)));
}


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                            Trigonometrix Helpers                          //
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

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                             GGX Distribution                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


float GGX_D(vec3 wi, vec3 wo, vec3 n, float a_x,float a_y) {
    vec3  wh     = normalize(wi+wo);
    
    float tan2Theta = Tan2Theta(wh);
    float cos4Theta = Cos2Theta(wh) * Cos2Theta(wh);
    float e = (Cos2Phi(wh) / (a_x * a_x) +
               Sin2Phi(wh) / (a_y * a_y)) * tan2Theta;
    return 1.0 / (PI * a_x * a_y * cos4Theta * (1.0 + e) * (1.0 + e));
}

/// Compute the effective roughness projected on direction \c v
float projectRoughness(vec3 v,float alpha_x,float alpha_y) {
    float sin2Theta = Sin2Theta(v);
    float invSinTheta2 = sin2Theta == 0.0 ? 0.0 : 1.0 / sin2Theta;

    if (invSinTheta2 <= 0.0)
        return alpha_x;

    float cosPhi2 = v.x * v.x * invSinTheta2;
    float sinPhi2 = v.y * v.y * invSinTheta2;

    return sqrt(cosPhi2 * alpha_x * alpha_x + sinPhi2 * alpha_y * alpha_y);
}

/* Smith GGX geometric functions
 */
float GGX_G1(vec3 v, vec3 n,float alpha_x, float alpha_y) {
    float a =  projectRoughness(v,alpha_x, alpha_y);
    float NdotV = dot(v,n);

    float a2 = sqr(a);
    return 2.0/(1.0 + sqrt(1.0 + a2 * (1.0-sqr(NdotV)) / sqr(NdotV) ));
}

/* Smith uncorrelated shadowing/masking function anisotropic.
 */
float GGX_G(vec3 wi, vec3 wo,float alpha_x,float alpha_y) {
    vec3 n = vec3(0.0,0.0,1.0);
    return GGX_G1(wi,n,alpha_x,alpha_y) * GGX_G1(wo,n,alpha_x,alpha_y);
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

float GGX_Reflection(vec3 wi, vec3 wo, vec3 n, float a_x,float a_y){
    vec3  wh     = normalize(wi+wo);
    float cosThetaWi = dot(n, wi);
    float cosThetaWo = dot(n, wo);

    float G = GGX_G(wi, wo, a_x,a_y);
    float D = GGX_D(wi, wo, vec3(0.0, 0.0, 1.0),a_x,a_y);

    return G*D/(abs(4.0*cosThetaWi));
}

/* Gamma transformation
 */
vec3 Gamma(vec3 R) {
    return exp(u_Gamma * log(u_Scale * R));
}

/* Evaluate an emissive term
 */
vec3 Emission(vec3 wi,vec3 wo, float a_x, float a_y) {
    return GGX_Reflection(wi, wo, vec3(0.0, 0.0, 1.0),a_x,a_y) * vec3(1.0);
}

vec4 drawGrid(in vec4 fragColor ,in float sinTo){
    for(float i = 0.0 ; i < PI/2.0 ; i += PI/12.0){
        if(asin(sinTo+EPSILON) >= i && asin(sinTo) <= i){
            return vec4(0.5,0.2,0.1,1.0); 
        }
    }
    return fragColor;
}

void main()
{

    vec3 wo;
    wo.xy = vPos.xy;
    float sinTo = length(wo.xy);

    if(sinTo <= 1.0) {
        
        wo.z = sqrt(1.0 - sqr(sinTo));

        for(int i=0; i<NB_PER_RUN; ++i) {

            /* Radom number
            */ 
            int  idx = mod(u_PassNumber*NB_PER_RUN+i, NB_SAMPLES);        
            vec2 rnd = QMC_Additive_2D(idx);

            /* Generate a sample from the GGX distribution */
            vec3 m = Sample_GGX_VNDF(wo, u_alpha_x_2,u_alpha_y_2, rnd.xy);
            vec3 ws = reflect(-wo, m); 
            ws.xy = -ws.xy;

            /* Evaluate radiance
             */
            vec3 value = vec3(0,0,0);
            if(ws.z > 0.0) {
                value = Emission(wi, ws, u_alpha_x_1,u_alpha_y_1);
            }

            /* Perform random evaluation of the layered structure using 'wiT' and 'woT'
            * as the incoming and outgoing directions.
            */
            fragColor.xyz += value;
        }

        fragColor.xyz = Gamma(fragColor.xyz / float(NB_PER_RUN));
        fragColor.w = 1.0;

        fragColor = drawGrid(fragColor,sinTo);

    } else {
        fragColor = vec4(0.09, 0.09, 0.09, 0.94);
    }
    fragColor.w = 1.0;
}