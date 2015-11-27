//Author: Chen Chen
//Date: 11/27/2015

#version 400 core

out vec4 outColor;

float linearDepth(float expDepth, float zNear, float zFar)
{
    float linDepth = (2.0 * zNear) / (zFar + zNear - expDepth * (zFar - zNear));
    return linDepth;
}

void main()
{
    float moment1 = gl_FragCoord.z;
    float moment2 = moment1 * moment1;

    outColor = vec4(moment1, moment2, 0.0, 1.0);
}

