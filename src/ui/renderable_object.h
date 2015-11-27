/*=====================================================================================
                                renderable_object.h
Description:  
    - Each instance of RenderableObject contains some data, and the corresponding method to draw the data.
    - To use RenderableObject, please follow the following procedure:
        1) Setup data:
            vector<RenderableObject::DATA> data; // Then transfer data to this vector

        2) Create RenderableObject object
            RenderableObject *m_vbo = new RenderableObject(); 

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

#ifndef RENDERABLE_OBJECT_H
#define RENDERABLE_OBJECT_H

#include "headers.h"

/*
 *  Default values:
 *      - Default usage: m_usage = GL_STATIC_DRAW
 */
class RenderableObject
{
public:
    struct Vertex 
    {
        glm::vec3   Position  = glm::vec3(0.0f, 0.0f, 0.0f);             // Coordinate
        glm::vec3   Normal    = glm::vec3(0.0f, 0.0f, 0.0f);             // Normal vector
        glm::vec4   Color     = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);       // RGBA color
        glm::vec2   TexCoords = glm::vec2(0.0f, 0.0f);                   // Texture coordinate
    };

public:
    RenderableObject();
    ~RenderableObject();

    void render();
    
    // Setup vertex data
        // Without element buffer
    void setData(const vector<Vertex>& data,
                 GLenum primitiveMode,
                 GLenum usage = GL_STATIC_DRAW);

        // With element buffer
    void setData(const vector<Vertex>& data,
                 const vector<GLuint>& indices,
                 GLenum primitiveMode,
                 GLenum usage = GL_STATIC_DRAW);

private:
    // Private functions
    void bind();
    void release();

    bool                          m_useIndexBuffer;
    GLuint                        m_vao;
    GLuint                        m_vbo;
    GLuint                        m_ebo;
    
    GLenum                        m_usage;          // GL_STATIC_DRAW, GL_DYNAMIC_DRAW, etc.
    GLenum                        m_primitiveMode;  // GL_POINTS, GL_LINES, etc.

    int                           m_nVertices;
};

#endif
