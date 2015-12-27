#include "mesh.h"
#include <sstream>

Mesh::Mesh() {
    params::inst().glFuncs->glGenVertexArrays(1, &m_vao);
    params::inst().glFuncs->glGenBuffers(1, &m_vbo);
    params::inst().glFuncs->glGenBuffers(1, &m_ebo);
}

Mesh::~Mesh() {
    if (m_vao != GL_INVALID_VALUE)
        params::inst().glFuncs->glDeleteVertexArrays(1, &m_vao);

    if (m_vbo != GL_INVALID_VALUE)
        params::inst().glFuncs->glDeleteBuffers(1, &m_vbo);

    if (m_ebo != GL_INVALID_VALUE)
        params::inst().glFuncs->glDeleteBuffers(1, &m_ebo);
}

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices,
           vector<Texture> textures) {
    params::inst().glFuncs->glGenVertexArrays(1, &m_vao);
    params::inst().glFuncs->glGenBuffers(1, &m_vbo);
    params::inst().glFuncs->glGenBuffers(1, &m_ebo);

    this->m_vertices = std::move(vertices);
    this->m_indices = std::move(indices);
    this->m_textures = std::move(textures);
    this->setupMesh();
}

void Mesh::setData(vector<Vertex> vertices, vector<GLuint> indices,
                   vector<Texture> textures) {
    this->m_vertices = std::move(vertices);
    this->m_indices = std::move(indices);
    this->m_textures = std::move(textures);
    this->setupMesh();
}

void Mesh::render(unique_ptr<Shader>& shader) {
    GLuint diffuseNr = 1;
    GLuint specularNr = 1;
    for (GLuint i = 0; i < this->m_textures.size(); i++) {
        params::inst().glFuncs->glActiveTexture(
            GL_TEXTURE2 + i);  // Activate proper texture unit before binding
        // Retrieve texture number (the N in diffuse_textureN)
        stringstream ss;
        string number;
        string name = this->m_textures[i].type;
        if (name == "DiffTex")
            ss << diffuseNr++;  // Transfer GLuint to stream
        else if (name == "SpecTex")
            ss << specularNr++;  // Transfer GLuint to stream
        number = ss.str();

        params::inst().glFuncs->glBindTexture(GL_TEXTURE_2D,
                                              this->m_textures[i].id);
        shader->seti((name + number).c_str(), i + 2);
    }

    // Draw mesh
    params::inst().glFuncs->glBindVertexArray(this->m_vao);
    params::inst().glFuncs->glDrawElements(GL_TRIANGLES, this->m_indices.size(),
                                           GL_UNSIGNED_INT, 0);
    params::inst().glFuncs->glBindVertexArray(0);
}

void Mesh::setupMesh() {
    params::inst().glFuncs->glBindVertexArray(this->m_vao);
    params::inst().glFuncs->glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo);

    params::inst().glFuncs->glBufferData(
        GL_ARRAY_BUFFER, this->m_vertices.size() * sizeof(Vertex),
        &this->m_vertices[0], GL_STATIC_DRAW);

    params::inst().glFuncs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ebo);
    params::inst().glFuncs->glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, this->m_indices.size() * sizeof(GLuint),
        &this->m_indices[0], GL_STATIC_DRAW);

    // Vertex Positions
    params::inst().glFuncs->glEnableVertexAttribArray(0);
    params::inst().glFuncs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                                                  sizeof(Vertex), (GLvoid*)0);
    // Vertex Normal
    params::inst().glFuncs->glEnableVertexAttribArray(1);
    params::inst().glFuncs->glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (GLvoid*)offsetof(Vertex, Normal));
    // Vertex Color
    params::inst().glFuncs->glEnableVertexAttribArray(2);
    params::inst().glFuncs->glVertexAttribPointer(
        2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (GLvoid*)offsetof(Vertex, Color));
    // Vertex Texture Coords
    params::inst().glFuncs->glEnableVertexAttribArray(3);
    params::inst().glFuncs->glVertexAttribPointer(
        3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (GLvoid*)offsetof(Vertex, TexCoords));

    params::inst().glFuncs->glBindVertexArray(0);
}
