//Author: Chen Chen
//Date: 11/27/2015

#version 400 core

in vec3 VertNormal;
in vec4 VertColor;

uniform vec3 lightPos;

out vec4 outColor;

void main()
{
   vec4 color = VertColor;

   vec3 N = normalize(VertNormal);
   vec3 L = normalize(lightPos);

   float d = max(0.0, dot(N, L));

   color.xyz *= d;

   outColor = vec4(color);	
}
