//Author: Chen Chen
//Date: 11/27/2015
#version 400 core

#define VERT_POSITION	0
#define VERT_NORMAL     1
#define VERT_COLOR		2
#define VERT_TEXTURE    3

layout(location = VERT_POSITION) in vec3 VertexPosition;
layout(location = VERT_NORMAL)   in vec3 VertexNormal;
layout(location = VERT_COLOR)    in vec4 VertexColor;
layout(location = VERT_TEXTURE)  in vec2 VertexTexture;

uniform mat4 matModel;
uniform mat4 matViewProjection;

void main()
{	   
    gl_Position = matViewProjection * matModel * vec4(VertexPosition, 1.0);
}
