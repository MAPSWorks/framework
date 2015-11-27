//Author: Chen Chen
//Date: 11/27/2015

#version 400 core

in vec4 VertColor;

out vec4 outColor;

float near = 1.0; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    float r = 1.0f - LinearizeDepth(gl_FragCoord.z) / far;
    float d = length(gl_PointCoord - 0.5f) * 2.0f;
    if( d > r ){
        discard;
    }

    outColor = vec4(VertColor.xyz, 0.3f);	
}
