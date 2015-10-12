/*=====================================================================================
                                nice_grid.h
    Description:  Ground Plane
        
    Created by Chen Chen on 09/30/2015
=====================================================================================*/

#ifndef NICEGRID_H
#define NICEGRID_H

#include "headers.h"

class Shader;
class Texture;
class VertexBufferObjectAttribs;

class NiceGrid
{
public:
   NiceGrid(GLfloat size, GLfloat rep);
   ~NiceGrid();

   void render(Shader* shader);
   void setAmbientColor(float r, float g, float b);  
   void setDiffuseColor(float r, float g, float b);

private:
    void createVBO();

private:
    GLuint                    m_texID;
    GLfloat                   m_size;
    GLfloat                   m_rep;

    Texture                   *m_texture;
    VertexBufferObjectAttribs *m_vbo;

    glm::vec3                 m_ambient;
    glm::vec3                 m_diffuse;

    glm::vec3                 m_position;

    float                     m_backFaceAlpha;
};

#endif

