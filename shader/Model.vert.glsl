//Author: Chen Chen
//Date: 11/27/2015
#version 400 core

/*
   Textures:
        - sampler2D ShadowMap
        - sampler3D OffsetTex
        - sampler2D DiffTex1

   Uniforms: 
        - Vertex shader
            - mat4 matModel
            - mat3 matNormal
            - mat4 matViewProjection
            - mat4 matLightSpace

        - Fragment shader
            - vec3 camPos
            - vec3 OffsetTexSize

            - struct LightInfo{
                  vec3 Position;
                  vec3 Intensity;
              } Light;

            - struct MaterialInfo{
                  vec3 Ka;
                  vec3 Kd;
                  vec3 Ks;
                  float Shininess;
              } Material;
   */

#define VERT_POSITION	0
#define VERT_NORMAL     1
#define VERT_COLOR		2
#define VERT_TEXTURE    3

layout(location = VERT_POSITION) in vec3 VertexPosition;
layout(location = VERT_NORMAL)   in vec3 VertexNormal;
layout(location = VERT_COLOR)    in vec4 VertexColor;
layout(location = VERT_TEXTURE)  in vec4 VertexTexture;

out vec3 Position;
out vec2 TexCoords;
out vec3 Normal;
out vec4 ShadowCoord; // Coordinate to be used for shadow map lookup

uniform mat4 matModel;
uniform mat3 matNormal;
uniform mat4 matViewProjection;
uniform mat4 matLightSpace; // LightProjection * LightView

void main()
{	   
    Position  = (matModel * vec4(VertexPosition, 1.0)).xyz;
    TexCoords = VertexTexture.xy;
    Normal    = normalize(matNormal * VertexNormal);

    // Shadow Matrix converts from modeling coordinates to shadow map coordinates
    ShadowCoord = matLightSpace * matModel * vec4(VertexPosition, 1.0);

    gl_Position = matViewProjection * matModel * vec4(VertexPosition, 1.0);
}
