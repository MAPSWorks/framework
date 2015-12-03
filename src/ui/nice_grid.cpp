#include "nice_grid.h"
#include "shader.h"
#include "renderable_object.h"
#include "texture.h"
#include "resource_manager.h"

NiceGrid::NiceGrid(GLfloat size, GLfloat rep)
: m_size(size),
  m_rep(rep),
  m_backFaceAlpha(0.2),
  m_ambient(0.12f, 0.12f, 0.1f),
  m_diffuse(1.0f, 1.0f, 0.9f),
  m_position(0.0f, 0.0f, 0.0f)
{
    createVBO();
}

NiceGrid::~NiceGrid()
{
}

void NiceGrid::render(unique_ptr<Shader>& shader)
{
    m_vbo->render();
}

void NiceGrid::setAmbientColor(float r, float g, float b)
{
    m_ambient.x = r;
    m_ambient.y = g;
    m_ambient.z = b;
}

void NiceGrid::setDiffuseColor(float r, float g, float b)
{
    m_diffuse.x = r;
    m_diffuse.y = g;
    m_diffuse.z = b;
}

void NiceGrid::createVBO()
{
    uint nrVertices = 6;
    vector<RenderableObject::Vertex> data(nrVertices);

    // first triangle
    data[0].Position  = glm::vec3(-m_size, 0.0f, -m_size);
    data[0].Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
    data[0].TexCoords = glm::vec2(0.0f, m_rep);

    data[1].Position  = glm::vec3(-m_size, 0.0f, m_size);
    data[1].Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
    data[1].TexCoords = glm::vec2(0.0f, 0.0f);

    data[2].Position  = glm::vec3(m_size, 0.0f, m_size);
    data[2].Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
    data[2].TexCoords = glm::vec2(m_rep, 0.0f);

    // Second triangle
    data[3].Position  = glm::vec3(m_size, 0.0f, m_size);
    data[3].Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
    data[3].TexCoords = glm::vec2(m_rep, 0.0f);

    data[4].Position  = glm::vec3(m_size, 0.0f, -m_size);
    data[4].Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
    data[4].TexCoords = glm::vec2(m_rep, m_rep);

    data[5].Position  = glm::vec3(-m_size, 0.0f, -m_size);
    data[5].Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
    data[5].TexCoords = glm::vec2(0, m_rep);

    m_vbo.reset(new RenderableObject());
    m_vbo->setData(data, GL_TRIANGLES);
}
