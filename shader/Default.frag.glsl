//Author: Chen Chen
//Date: 11/27/2015
#version 400 core

in vec2 TexCoords;
in vec4 VertColor;

out vec4 FragColor;

uniform sampler2D DiffTex1;

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

// Render without texture 
subroutine (RenderPassType)                             
    void renderPlain(){                               
        FragColor = VertColor;
    }

// Render with texture 
subroutine (RenderPassType)                             
    void renderWithTexture(){                               
        FragColor = texture(DiffTex1, TexCoords);
    }

void main()
{
    RenderPass();
}
