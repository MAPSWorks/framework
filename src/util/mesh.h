#ifndef MESH_H
#define MESH_H

#include "headers.h"
#include "shader.h"

class Mesh
{
    public:
        struct Vertex{
            glm::vec3   Position  = glm::vec3(0.0f, 0.0f, 0.0f);             // Coordinate
            glm::vec3   Normal    = glm::vec3(0.0f, 0.0f, 0.0f);             // Normal vector
            glm::vec4   Color     = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);       // RGBA color
            glm::vec2   TexCoords = glm::vec2(0.0f, 0.0f);                   // Texture coordinate
        };

        struct Texture{
            GLuint id;
            string type;
            string path;
        };

        // std::move semantics
        Mesh(Mesh&& x){
            m_vao = x.m_vao;
            m_vbo = x.m_vbo;
            m_ebo = x.m_ebo;
            m_vertices = std::move(x.m_vertices);
            m_indices = std::move(x.m_indices);
            m_textures = std::move(m_textures);
        }
        Mesh& operator=(Mesh&& x){
            if(this != &x){
                m_vao = x.m_vao;
                m_vbo = x.m_vbo;
                m_ebo = x.m_ebo;
                m_vertices = std::move(x.m_vertices);
                m_indices = std::move(x.m_indices);
                m_textures = std::move(m_textures);
            }
            return *this;
        }

        Mesh();
        ~Mesh();

        Mesh(vector<Vertex>     vertices,
             vector<GLuint>     indices,
             vector<Texture>    textures);
        
        void setData(vector<Vertex>  vertices,
                     vector<GLuint>  indices,
                     vector<Texture> textures);

        // Render
        void render(unique_ptr<Shader>& shader);

    private:
        // Disable copying
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        GLuint                           m_vao;
        GLuint                           m_vbo;
        GLuint                           m_ebo;

        /*  Mesh Data  */
        vector<Vertex>                   m_vertices;
        vector<GLuint>                   m_indices;
        vector<Texture>                  m_textures;

        void setupMesh();
};

#endif

