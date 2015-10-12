//Author: Chen Chen
//Date: 10.09.2015

#version 400 core

#define VERT_POSITION	0
#define VERT_NORMAL     1
#define VERT_COLOR		2
#define VERT_TEXTURE    3

layout(location = VERT_POSITION) in vec4 Position;
layout(location = VERT_NORMAL)   in vec4 Normal;
layout(location = VERT_COLOR)    in vec4 Color;
layout(location = VERT_TEXTURE)  in vec4 Texture;

out vec2 TexCoords;

out VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4x4 matModel;
uniform mat4x4 matView;
uniform mat4x4 matLightView;
uniform mat4x4 matProjection;
uniform mat4x4 matViewProjection;

void main()
{	   
    gl_Position = matProjection * matView * matModel * Position;
    
    vs_out.FragPos = vec3(matModel * Position);
    vs_out.Normal = transpose(inverse(mat3(matModel))) * Normal.xyz;
    vs_out.TexCoords = Texture.xy;
    vs_out.FragPosLightSpace = matLightView * vec4(vs_out.FragPos, 1.0f);
}
