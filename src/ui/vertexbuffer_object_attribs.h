/*=====================================================================================
                                vertexbuffer_object_attribs.h
Description:  
    - Each instance of VertexBufferObjectAttribs contains some data, and the corresponding method to draw the data.
    - To use VertexBufferObjectAttribs, please follow the following procedure:
        1) Setup data:
            vector<VertexBufferObjectAttribs::DATA> data; // Then transfer data to this vector

        2) Create VertexBufferObjectAttribs object
            VertexBufferObjectAttribs *m_vbo = new VertexBufferObjectAttribs(); 

        3) Setup data layout
            m_vbo->addAttrib(VERTEX_POSITION); // Look up your shader, this corresponds to your layout (location = VERTEX_POSITION) 
            m_vbo->addAttrib(xxx);
            // NOTICE: only do this once! 

        4) Set data
            m_vbo->setData(&data[0], GL_STATIC_DRAW, data.size(), GL_TRIANGLES);
            // It's up to you to use GL_DYNAMIC_DRAW, GL_LINES, GL_POINTS, etc. to draw the data
            
        5) Optional: set index data
            m_vbo->setIndexData(&index[0], GL_STATIC_DRAW, index.size());

        6) Bind attributes
            m_vbo->bindAttribs();

        Now you are ready, you can call m_vbo->render() anytime you want to draw the data, but don't forget to choose shader and set uniform
        variables first:
            e.g., 
                m_shader->bind();
                    
                    // Setup texture
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, texId);
                    m_shader->seti("tex", 0);

                    // Setup uniforms
                    m_shader->setMatrix("model", model);
                    ...

                    // Now render!
                    m_vbo->render();

                m_shader->release();

    Created by Chen Chen on 09/30/2015
=====================================================================================*/

#ifndef VERTEXBUFFEROBJECTATTRIBS_H
#define VERTEXBUFFEROBJECTATTRIBS_H

#include "headers.h"

class VertexBufferObjectAttribs
{
public:
    struct DATA
    {
        // 3D coordinates
        float vx = 0.0f;
        float vy = 0.0f;
        float vz = 0.0f;
        float vw = 0.0f;

        // Normal Vector
        float nx = 0.0f;
        float ny = 0.0f;
        float nz = 0.0f;
        float nw = 0.0f;

        // Color
        float cx = 1.0f;
        float cy = 1.0f;
        float cz = 1.0f;
        float cw = 1.0f;        

        // Texture Coordinate
        float tx = 0.0f;
        float ty = 0.0f;
        float tz = 0.0f; 
        float tw = 0.0f;
    };

public:
    VertexBufferObjectAttribs();
    ~VertexBufferObjectAttribs();

    void bind();
    GLuint id() const;
    void release();
    void render();
    
    // Common function
	void setupBuffer(GLenum target, GLuint dataSize, const void* data, GLenum usage);

    void setData(const DATA *data, 
                 GLenum usage, 
                 GLuint nrVertices, 
                 GLenum primitiveMode);

    void setIndexData(const GLvoid *data, 
                      GLenum usage, 
                      GLint nrIndices);

    void setIndexData(const GLvoid *data, 
                      GLenum usage, 
                      GLint nrIndices,
                      GLenum primitiveMode);

    void addAttrib(GLint attribLoc);
    void bindAttribs();

    void setVerticesToRender(GLuint nrVertices); 
    void setDynamicRendering(GLboolean dynamicRendering);

    GLuint nrVertices() const;
    GLuint nrDynamicVertices() const;

private:
    GLuint                        m_nrVertices;
    GLuint                        m_nrDynamicVertices;
    GLuint                        m_nrIndices;
    GLuint                        m_nrDynamicIndices;
    GLuint                        m_sizeBytesVertices;
    GLuint                        m_sizeBytesIndices;
    GLboolean                     m_useIndexBuffer;
    
    GLuint                        m_vao;
    GLuint                        m_vbo;
    GLuint                        m_ebo;
    
    GLenum                        m_primitiveMode;
    GLenum                        m_usage;

    GLuint                        m_sizeAsStride;

    vector<GLint>                 m_attribLocations;

    GLboolean                     m_dynamicRendering;
};

#endif
