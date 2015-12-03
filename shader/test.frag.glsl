//Author: Chen Chen
//Date: 11/27/2015
#version 400 core

// in 
in vec3 Position;
in vec3 Normal;
in vec2 TexCoords;
in vec4 ShadowCoord;

// out
out vec4 FragColor;

// Textures
uniform sampler2D           ShadowMap;
uniform sampler3D           OffsetTex;
uniform sampler2D           DiffTex1;

// uniform
uniform vec3                camPos;
uniform vec3                OffsetTexSize; // (width, height, depth)

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
    vec3 s = normalize( Light.Position.xyz - Position.xyz );
    vec3 v = normalize(camPos - Position);
    vec3 h = normalize( v + s );

    ambient = Material.Ka * Light.Intensity; 
    diffuse = Light.Intensity
              * Material.Kd 
              * max(0.0, dot(s, Normal) );

    spec    = Light.Intensity 
              * Material.Ks 
              * pow( max(0.0, dot(h, Normal)), Material.Shininess);
}

float CalculateShadow(){
   // Compute shadow
    vec3 lightDir = normalize(Light.Position - Position);
    float bias = max(0.001 * (1.0 - dot(Normal, lightDir)), 0.0005);
    vec3 projCoords = ShadowCoord.xyz / ShadowCoord.w;
    projCoords = projCoords * 0.5 + 0.5;

    ivec3 offsetCoord;
    offsetCoord.xy = ivec2( mod( gl_FragCoord.xy, OffsetTexSize.xy ) );
    float currentDepth = projCoords.z;

    return (currentDepth - bias) > texture(ShadowMap, projCoords.xy).r ? 1.0 : 0.0; 
    float sum = 0.0;
    int samplesDiv2 = int(OffsetTexSize.z);
    vec3 sc = projCoords;
    /*for( int i = 0 ; i < 4; i++ ) {*/
        //offsetCoord.z = i;
        vec4 asdf = texture(OffsetTex, offsetCoord);//, 0);
        //sc.xy = projCoords.xy + offsets.xy;
        //sum += (currentDepth - bias) > texture(ShadowMap, sc.xy).r ? 1.0 : 0.0;
        //sc.xy = projCoords.xy + offsets.zw;
        //sum += (currentDepth - bias) > texture(ShadowMap, sc.xy).r ? 1.0 : 0.0;
    /*} */
}

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;


/*void RenderPass(){                                                    */
/*    vec3 ambient, diffuse, spec;                                      */
/*    phongModelDiffAndSpec(ambient, diffuse, spec);                    */

/*    float shadow = CalculateShadow();                                 */

/*    FragColor = vec4((diffuse + spec)* (1.0 - shadow) + ambient, 1.0);*/
/*}                                                                     */

// Shading with shadow and texture
/*subroutine (RenderPassType)                                         */
/*    void shadeWithShadowAndTexture(){                               */
/*        vec3 ambient, diffuse, spec;                                */
/*        phongModelDiffAndSpec(ambient, diffuse, spec);              */

/*        float shadow = CalculateShadow();                           */
/*        vec4 texColor = texture(DiffTex1, TexCoords);               */
/*        vec4 ambWithTex = vec4(ambient, 1.0) * texColor;            */
/*        vec4 diffWithTex = vec4(diffuse, 1.0) * texColor;           */

/*        FragColor = (diffWithTex + vec4(spec, 1.0)) * (1.0 - shadow)*/
/*                    + ambWithTex;                                   */
/*    }                                                               */

subroutine (RenderPassType)
    void shadeWithShadow(){
        vec3 ambient, diffuse, spec;
        phongModelDiffAndSpec(ambient, diffuse, spec);

        float shadow = CalculateShadow();

        FragColor = vec4((diffuse + spec)* (1.0 - shadow) + ambient, 1.0);
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
