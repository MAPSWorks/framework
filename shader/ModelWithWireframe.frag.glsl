//Author: Chen Chen
//Date: 11/27/2015
#version 400 core

// in 
in vec3 GPosition;
in vec3 GNormal;
in vec2 GTexCoords;
in vec4 GShadowCoord;
in vec3 GEdgeDistance;

// out
out vec4 FragColor;

// Textures
uniform sampler2D           ShadowMap;
uniform sampler3D           OffsetTex;
uniform sampler2D           DiffTex1;

// uniform
uniform vec3                camPos;
uniform vec3                OffsetTexSize; // (width, height, depth)
uniform float               Radius;

struct LineInfo{
    float Width;
    vec4  Color;
};
uniform LineInfo Line;

struct LightInfo {
    vec3 Position;
    vec3 Intensity;
};
uniform LightInfo Light;

struct MaterialInfo{
    vec3 Ka;            // Ambient reflectivity
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ks;            // Specular reflectivity
    float Shininess;    // Specular shininess factor
};
uniform MaterialInfo Material;

void phongModelDiffAndSpec(out vec3 ambient,
                           out vec3 diffuse,
                           out vec3 spec) {
    vec3 s = normalize( Light.Position.xyz - GPosition.xyz );
    vec3 v = normalize(camPos - GPosition);
    vec3 h = normalize( v + s );

    ambient = Material.Ka * Light.Intensity; 
    diffuse = Light.Intensity
              * Material.Kd 
              * max(0.0, dot(s, GNormal) );

    spec    = Light.Intensity 
              * Material.Ks 
              * pow( max(0.0, dot(h, GNormal)), Material.Shininess);
}

float CalculateShadow(){
    // Compute shadow
    vec3 lightDir = normalize(Light.Position - GPosition);
    float bias = max(0.001 * (1.0 - dot(GNormal, lightDir)), 0.0005);
    vec3 projCoords = GShadowCoord.xyz / GShadowCoord.w;
    projCoords = projCoords * 0.5 + 0.5;

    ivec3 offsetCoord;
    offsetCoord.xy = ivec2( mod( gl_FragCoord.xy, OffsetTexSize.xy ) );
    float currentDepth = projCoords.z;

    float sum = 0.0;
    int samplesDiv2 = int(OffsetTexSize.z);
    vec3 sc = projCoords;
    for( int i = 0 ; i < 4; i++ ) {
        offsetCoord.z = i;
        vec4 offsets = texelFetch(OffsetTex,offsetCoord,0) * 0.0015;
        sc.xy = projCoords.xy + offsets.xy;
        sum += (currentDepth - bias) > texture(ShadowMap, sc.xy).r ? 1.0 : 0.0;
        sc.xy = projCoords.xy + offsets.zw;
        sum += (currentDepth - bias) > texture(ShadowMap, sc.xy).r ? 1.0 : 0.0;
    }
    float shadow = sum / 8.0;
    if( shadow != 1.0 && shadow != 0.0 ) {
        for( int i = 4; i< samplesDiv2; i++ ) {
            offsetCoord.z = i;
            vec4 offsets = texelFetch(OffsetTex, offsetCoord,0) * 0.0015;
            sc.xy = projCoords.xy + offsets.xy;
            sum += (currentDepth - bias) > texture(ShadowMap, sc.xy).r ? 1.0 : 0.0;
            sc.xy = projCoords.xy + offsets.zw;
            sum += (currentDepth - bias) > texture(ShadowMap, sc.xy).r ? 1.0 : 0.0;
        }
        shadow = sum / float(samplesDiv2 * 2.0);
    }
    return shadow;
}

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

// Without shadow or texture
subroutine (RenderPassType)
    void shadeNoShadow(){
        vec3 ambient, diffuse, spec;
        phongModelDiffAndSpec(ambient, diffuse, spec);

        FragColor = vec4(diffuse + spec + ambient, 1.0);
    }

// Shading without shadow but with using texture
subroutine (RenderPassType)
    void shadeNoShadowWithTexture(){
        vec3 ambient, diffuse, spec;
        phongModelDiffAndSpec(ambient, diffuse, spec);

        float shadow = CalculateShadow();
        vec4 texColor = texture(DiffTex1, GTexCoords);
        vec4 ambAndDiff = vec4(ambient + diffuse, 1.0) * texColor;
        FragColor = ambAndDiff + vec4(spec, 1.0);
    }

// Shading with shadow (without using texture)
subroutine (RenderPassType)
    void shadeWithShadow(){
        vec3 ambient, diffuse, spec;
        phongModelDiffAndSpec(ambient, diffuse, spec);

        // Find the smallest distance
        float d = min( GEdgeDistance.x, GEdgeDistance.y );
        d = min( d, GEdgeDistance.z );
        // Determine the mix factor with the line color
        float mixVal = smoothstep( 0.0,    
                                   Line.Width + 1, d);

        float shadow = CalculateShadow();

        vec4 outColor = vec4((diffuse + spec)* (1.0 - shadow) + ambient, 1.0);
        FragColor = mix( Line.Color, outColor, mixVal );
    }

// Shading with shadow and tecture
subroutine (RenderPassType)
    void shadeWithShadowAndTexture(){
        vec3 ambient, diffuse, spec;
        phongModelDiffAndSpec(ambient, diffuse, spec);

        float shadow = CalculateShadow();
        vec4 texColor = texture(DiffTex1, GTexCoords);
        vec4 ambWithTex = vec4(ambient, 1.0) * texColor;
        vec4 diffWithTex = vec4(diffuse, 1.0) * texColor;

        FragColor = (diffWithTex + vec4(spec, 1.0)) * (1.0 - shadow)
                    + ambWithTex;
    }

// Shadow mapping run
subroutine (RenderPassType)
    void recordDepth(){
        // Do nothing. Depth will be written automatically
    }

void main()
{
    // This will call either shadeWithShadow or recordDepth
    RenderPass();
}
