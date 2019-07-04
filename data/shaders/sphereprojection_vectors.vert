#version 330 core

layout (location = 0) in vec3 aPosition;

uniform mat4 mvp;

uniform float theta;
uniform float phi;

void main() {
    vec3 wi = normalize(vec3(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta)));

    vec3 wo = reflect(-wi,vec3(0,0,1));

    if(aPosition.x == 1){ // wi

        gl_Position = mvp * vec4(wi, 1.0); 

    }else if(aPosition.z == 1){ // wo

        gl_Position = mvp * vec4(wo, 1.0); 

    }else {

        gl_Position = mvp * vec4(0,0,0, 1.0); 

    }

}