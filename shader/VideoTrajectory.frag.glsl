//Author: Chen Chen
//Date: 11/27/2015
#version 400 core

in vec3 TexCoords;
in vec4 VertColor;

out vec4 FragColor;

uniform sampler2DArray quadTex;
uniform float     gamma; // Gamma correction

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

// Render without texture 
subroutine (RenderPassType)                   
    void renderPlain(){                               
        FragColor = pow(VertColor, vec4(1.0 / gamma));
    }

// Render with texture 
subroutine (RenderPassType)                             
    void renderWithTexture(){                               
        vec4 texColor = texture(quadTex, TexCoords);
        FragColor = pow(vec4(texColor.xyz, VertColor.w), vec4(1.0 / gamma));
    }

void main()
{
    RenderPass();
}
