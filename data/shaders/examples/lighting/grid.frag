#version 330 core
out vec4 FragColor;

in vec3 gridColor;

in vec3 raw_position;

void main()
{
    float far=gl_DepthRange.far; float near=gl_DepthRange.near;

    float ndc_depth = gl_FragCoord.z / gl_FragCoord.w;
    float depth = (((far-near) * ndc_depth) + near + far) / 2.0;

    if(mod(raw_position.x,10.0) == 0.0 || mod(raw_position.z,10.0) == 0.0){
        FragColor = vec4(gridColor,1/depth*10.0);
    }else if(raw_position.x == 0.0 || raw_position.z == 0.0){
        FragColor = vec4(gridColor,1/depth+0.5);
    }else{
        FragColor = vec4(gridColor,1/depth*5.0);
    }   
}