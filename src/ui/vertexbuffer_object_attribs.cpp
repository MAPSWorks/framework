#include <stdio.h>
#include "vertexbuffer_object_attribs.h"

VertexBufferObjectAttribs::VertexBufferObjectAttribs()
: m_useIndexBuffer(GL_FALSE),
  m_nrVertices(0),
  m_nrDynamicVertices(0),
  m_nrIndices(0),
  m_nrDynamicIndices(0),
  m_sizeBytesVertices(0),
  m_sizeBytesIndices(0),   
  m_vao(0),
  m_vbo(0),
  m_ebo(0),    
  m_primitiveMode(GL_POINTS),
  m_usage(GL_STATIC_DRAW),
  m_sizeAsStride(0),
  m_dynamicRendering(GL_FALSE)
{  
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
}

VertexBufferObjectAttribs::~VertexBufferObjectAttribs()
{
    if(m_vao != GL_INVALID_VALUE)
        glDeleteVertexArrays(1, &m_vao);

    if(m_vbo != GL_INVALID_VALUE)
        glDeleteBuffers(1, &m_vbo);

    if(m_ebo != GL_INVALID_VALUE)
        glDeleteBuffers(1, &m_ebo);
}

void VertexBufferObjectAttribs::bind()
{
    glBindVertexArray(m_vao);
}

void VertexBufferObjectAttribs::release()
{
    glBindVertexArray(0);
}

void VertexBufferObjectAttribs::render()
{    
    if(!m_useIndexBuffer){
        glBindVertexArray(m_vao);

        if(m_dynamicRendering)
        {
            glDrawArrays(m_primitiveMode, 0, m_nrDynamicVertices);
        }
        else
        {
            glDrawArrays(m_primitiveMode, 0, m_nrVertices);
        }

        glBindVertexArray(0);
    }
    else{
        glBindVertexArray(m_vao);

        if(m_dynamicRendering)
        {
            glDrawElements(m_primitiveMode, 
                           m_nrIndices,
                           GL_UNSIGNED_INT,
                           0);
        }
        else
        {
            glDrawElements(m_primitiveMode, 
                           m_nrIndices,
                           GL_UNSIGNED_INT,
                           0);
        }

        glBindVertexArray(0);
    }

}

/*=====================================================================================
       Create VBO or EBO, bind it, and transfer data 
=====================================================================================*/
void VertexBufferObjectAttribs::setupBuffer(GLenum target, GLuint dataSize, const void* data, GLenum usage)
{
    glBindVertexArray(this->m_vao);

    switch (target) { 
        case GL_ARRAY_BUFFER: 
            glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo);
            glBufferData(GL_ARRAY_BUFFER, dataSize, data, usage);
            break; 
        case GL_ELEMENT_ARRAY_BUFFER:
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, data, usage);
            break;
        default: 
            break; 
    } 
    
    glBindVertexArray(0);
}

/*=====================================================================================
        Transfer data to VBO

        - usage: GL_STATIC_DRAW, GL_DYNAMIC_DRAW
        - primitiveMode: GL_POINTS, GL_LINES, GL_TRIANGLES, etc.

=====================================================================================*/
void VertexBufferObjectAttribs::setData(const DATA *data, 
                                        GLenum usage, 
                                        GLuint nrVertices, 
                                        GLenum primitiveMode)
{
    m_sizeAsStride = sizeof(DATA);
    m_sizeBytesVertices = m_sizeAsStride * nrVertices;
    m_primitiveMode = primitiveMode;
    m_usage = usage;
    m_nrVertices = nrVertices;
    m_nrDynamicVertices = nrVertices;

    setupBuffer(GL_ARRAY_BUFFER, m_sizeBytesVertices, data, usage);
}

/*=====================================================================================
        Transfer data to EBO
=====================================================================================*/
void VertexBufferObjectAttribs::setIndexData(const GLvoid *data, 
                                             GLenum usage, 
                                             GLint nrIndices)
{
    m_nrIndices = nrIndices;
    m_useIndexBuffer = GL_TRUE;

    m_sizeBytesIndices = sizeof(uint) * nrIndices;
    
    setupBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sizeBytesIndices, data, usage); 
}

// Overwrite primitiveMode
void VertexBufferObjectAttribs::setIndexData(const GLvoid *data, 
                                             GLenum usage, 
                                             GLint nrIndices,
                                             GLenum primitiveMode)
{
    m_nrIndices      = nrIndices;
    m_useIndexBuffer = GL_TRUE;
    m_primitiveMode  = primitiveMode;

    m_sizeBytesIndices = sizeof(uint) * nrIndices;
    
    setupBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sizeBytesIndices, data, usage); 
}

/*=====================================================================================
        Setup Data Layout
=====================================================================================*/
void VertexBufferObjectAttribs::addAttrib(GLint attribLoc)
{
    m_attribLocations.push_back(attribLoc);
}

void VertexBufferObjectAttribs::bindAttribs()
{
    int size = m_attribLocations.size();
    glBindVertexArray(m_vao);
    for(int i=0; i<size; ++i)
    {
        GLint attribLoc = m_attribLocations[i];
        glEnableVertexAttribArray(attribLoc);
        glVertexAttribPointer(attribLoc, 4, GL_FLOAT, GL_FALSE, sizeof(DATA), (GLvoid*)(4 * sizeof(float) * i));
    }
    glBindVertexArray(0);
}

void VertexBufferObjectAttribs::setVerticesToRender(GLuint nrVertices)
{
    m_nrDynamicVertices = nrVertices;
}

void VertexBufferObjectAttribs::setDynamicRendering(GLboolean dynamicRendering)
{
    m_dynamicRendering = dynamicRendering;
}

GLuint VertexBufferObjectAttribs::nrVertices() const
{
    return m_nrVertices;
}

GLuint VertexBufferObjectAttribs::nrDynamicVertices() const
{
    return m_nrDynamicVertices;
}

GLuint VertexBufferObjectAttribs::id() const
{
    return m_vao;
}
