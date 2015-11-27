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
uniform mat4x4 matLightView;

uniform vec4 clipPlane;

layout(location = VERT_POSITION) in vec3 Position;
layout(location = VERT_NORMAL)   in vec3 Normal;
layout(location = VERT_COLOR)    in vec4 Color;
layout(location = VERT_TEXTURE)  in vec2 Texture;

out vec4 VertPosition;
out vec4 VertNormal;
out vec4 VertColor;
out vec4 VertTexture;
out vec4 VertShadowCoord;

void main()
{	   
    VertPosition    = matModel * vec4(Position, 1.0f); 
    VertNormal      = vec4(transpose(inverse(matModel)) * vec4(Normal, 1.0));    
	VertColor       = Color;
	VertTexture     = vec4(Texture, 0.0f, 0.0f);
    VertShadowCoord = matLightView * matModel * vec4(Position, 1.0f);

    vec4 tmpClip = clipPlane;
    tmpClip.y = 1.0;
    tmpClip.w = -0.1;

    gl_ClipDistance[0] = dot(matModel * vec4(Position, 1.0f) -vec4(0, -0.25, 0 ,0), tmpClip);	
    gl_Position = matProjection * matView * matModel * vec4(Position, 1.0f);
}
