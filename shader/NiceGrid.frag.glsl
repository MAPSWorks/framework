//Author: Chen Chen
//Date: 11/27/2015

#version 400 core

uniform sampler2D tex; 

uniform vec3 diffuseColor;
uniform vec3 ambientColor;
uniform vec3 specularColor;

uniform vec3 camPos;
uniform vec3 lightPos;
uniform vec3 lightDir;
uniform float shadowIntensity;
uniform int renderMode;
uniform float alpha;
uniform int applyShadow;
uniform vec3 attenuation;
uniform vec2 lightCone;

uniform sampler2D shadowMap;

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

out vec4 outColor;

float distAttenuation(vec3 P, vec3 L, float kC, float kL, float kQ)
{
    float d = distance(P, lightPos);
    float r = 1 / (kC + kL * d + kQ * d * d);
    return r;
}

float dualConeSpotlight(vec3 P, vec3 L, float outerCone, float innerCone)
{
    vec3 V = normalize(-L);
    float d = dot(V, P-L);
    return smoothstep(outerCone, innerCone, d);
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5f + 0.5f;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check whether current frag pos is in shadow
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    float bias = max(0.01 * (1.0f - dot(fs_in.Normal, lightDir)), 0.001f);
    for(int x = -3; x <= 3; ++x)
    {
        for(int y = -3; y <= 3; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 36.0;
    
    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    shadow *= 0.5f;

    return shadow;
} 

void main()
{
    float shadow = 0.0;
    if(applyShadow == 1)    
    {
       shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    }

    vec4 texColor = texture(tex, fs_in.TexCoords.st);

    vec3 N = normalize(fs_in.Normal.xyz);
    vec3 L = normalize(lightPos-fs_in.FragPos.xyz);

    float d = max(0.0, dot(N, L));   
    float t = dualConeSpotlight(fs_in.FragPos.xyz, lightPos, lightCone.x, lightCone.y);
    float a = distAttenuation(fs_in.FragPos.xyz, lightPos-fs_in.FragPos.xyz, attenuation.x, attenuation.y, attenuation.z);

    vec3 ambient  = ambientColor;
    vec3 diffuse = diffuseColor * a; //*t;

    vec4 color = texColor;
    if(renderMode == 0)
    {
        color.xyz = (ambient + (1.0 - shadow) * (diffuse)) * texColor.rgb * d;
        color.a = alpha;
    }
    if(renderMode == 1)
    {
        color.xyz = vec3(0.2, 0.2, 0.2) * (1.0 - shadow);
        color.a = alpha;
    }
    if(renderMode == 2)
    {
        color.xyz = vec3(1.0, 1.0, 1.0) * (1.0 - shadow);
        color.a = alpha;
    }
    if(renderMode == 3)
    {
        color.xyz = vec3(1.0, 1.0, 1.0) * (1.0 - shadow) * d;
        color.a = alpha;
    }

    outColor = vec4(color.xyz, alpha);	
}
