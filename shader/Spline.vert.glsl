//Author: Chen Chen
//Date: 11/27/2015

#version 400 core

#define VERT_POSITION	0
#define VERT_NORMAL     1
#define VERT_COLOR		2
#define VERT_TEXTURE    3

uniform mat4x4 matModel;
uniform mat4x4 matView;
uniform mat4x4 matProjection;

layout(location = VERT_POSITION) in vec3 Position;
layout(location = VERT_NORMAL)   in vec3 Normal;
layout(location = VERT_COLOR)    in vec4 Color;
layout(location = VERT_TEXTURE)  in vec2 Texture;

out vec4 VertColor;
out vec3 VertNormal;

void main()
{	   
	VertColor    = Color;
    VertNormal   = Normal;
	
    gl_Position = matProjection * matView * matModel * vec4(Position, 1);
}
