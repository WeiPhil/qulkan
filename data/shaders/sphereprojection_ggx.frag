#version 330 core

out vec4 fragColor;


in float sampleValue;


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
    if(sampleValue <= 0){
        discard;
    }
    fragColor = vec4(colormap(min(sampleValue, 1.0 )),1);
    // fragColor = vec4(0.5,0.5,0.5,0.6);
}