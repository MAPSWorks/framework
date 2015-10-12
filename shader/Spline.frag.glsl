//Author: Chen Chen
//Date: 02.10.2015

#version 400 core

in vec4 VertColor;
in vec4 VertNormal;

out vec4 outColor;

void main()
{
    float d = length(gl_PointCoord - 0.5f);
    if (d > VertNormal.x)
        discard;
    else
        outColor = VertColor;	
}
