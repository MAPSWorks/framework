#include "mesh.h"
#include <sstream>

Mesh::Mesh(){
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
}

Mesh::~Mesh(){
    if(m_vao != GL_INVALID_VALUE)
        glDeleteVertexArrays(1, &m_vao);

    if(m_vbo != GL_INVALID_VALUE)
        glDeleteBuffers(1, &m_vbo);

    if(m_ebo != GL_INVALID_VALUE)
        glDeleteBuffers(1, &m_ebo);
}

Mesh::Mesh(vector<Vertex>   vertices,
           vector<GLuint>   indices,
           vector<Texture>  textures){
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    this->m_vertices = std::move(vertices);
    this->m_indices  = std::move(indices);
    this->m_textures = std::move(textures);
    this->setupMesh();
}

void Mesh::setData(vector<Vertex>  vertices,
                   vector<GLuint>  indices,
                   vector<Texture> textures){
    this->m_vertices = std::move(vertices);
    this->m_indices  = std::move(indices);
    this->m_textures = std::move(textures);
    this->setupMesh();
}

void Mesh::render(unique_ptr<Shader>& shader){
    GLuint diffuseNr = 1;
    GLuint specularNr = 1;
    for(GLuint i = 0; i < this->m_textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE2 + i); // Activate proper texture unit before binding
        // Retrieve texture number (the N in diffuse_textureN)
        stringstream ss;
        string number;
        string name = this->m_textures[i].type;
        if(name == "DiffTex")
            ss << diffuseNr++; // Transfer GLuint to stream
        else if(name == "SpecTex")
            ss << specularNr++; // Transfer GLuint to stream
        number = ss.str(); 

        glBindTexture(GL_TEXTURE_2D, this->m_textures[i].id);
        shader->seti((name + number).c_str(), i+2);
    }

    // Draw mesh
    glBindVertexArray(this->m_vao);
    glDrawElements(GL_TRIANGLES, 
                   this->m_indices.size(), 
                   GL_UNSIGNED_INT, 
                   0);
    glBindVertexArray(0);
}

void Mesh::setupMesh(){
    glBindVertexArray(this->m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo);

    glBufferData(GL_ARRAY_BUFFER, 
                 this->m_vertices.size() * sizeof(Vertex), 
                 &this->m_vertices[0], 
                 GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 this->m_indices.size() * sizeof(GLuint), 
                 &this->m_indices[0], 
                 GL_STATIC_DRAW);

    // Vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    // Vertex Normal 
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
    // Vertex Color
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Color));
    // Vertex Texture Coords
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}
