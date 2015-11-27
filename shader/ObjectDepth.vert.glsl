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

uniform vec4 ClipPlane;

layout(location = VERT_POSITION) in vec3 Position;
layout(location = VERT_NORMAL)   in vec3 Normal;
layout(location = VERT_COLOR)    in vec4 Color;
layout(location = VERT_TEXTURE)  in vec2 Texture;

out float gl_ClipDistance[1];

void main()
{	   
    vec4 tmpClip = ClipPlane;
    tmpClip.y = 1.0;
    tmpClip.w = -0.1;
	
    gl_Position = matProjection * matView * matModel * vec4(Position, 1.0f);
    gl_ClipDistance[0] = dot(matModel * vec4(Position, 1.0f) -vec4(0, -0.25, 0 ,0), tmpClip);
}
