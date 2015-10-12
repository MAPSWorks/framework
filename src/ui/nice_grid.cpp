#include "nice_grid.h"
#include "shader.h"
#include "vertexbuffer_object_attribs.h"
#include "texture.h"
#include "mesh.h"

NiceGrid::NiceGrid(GLfloat size, GLfloat rep)
: m_size(size),
  m_rep(rep),
  m_texture(NULL),
  m_backFaceAlpha(0.2),
  m_ambient(0.12f, 0.12f, 0.1f),
  m_diffuse(1.0f, 1.0f, 0.9f),
  m_vbo(NULL),
  m_position(0.0f, 0.0f, 0.0f)
{
    QImage tex("../data/floor_grey.png");

    m_texture = new Texture(tex);
    m_texture->setWrapMode(GL_REPEAT);

    createVBO();
}

NiceGrid::~NiceGrid()
{
    delete m_texture;

    if (m_vbo != NULL) { 
        delete m_vbo;
    } 
}

void NiceGrid::render(Shader* shader)
{
    glm::mat4 model(1.0);
    model = glm::translate(model, m_position);

    shader->bind();  

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture->id());    
        shader->seti("tex", 0);    

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, params::inst()->shadowMapID);    
        shader->seti("shadowMap", 1);   

        shader->set3f("lightPos", params::inst()->lightPos);
        shader->set3f("lightDir", params::inst()->lightDir);
        shader->set3f("camPos", params::inst()->camPos);
        shader->set3f("diffuseColor", m_diffuse.x, m_diffuse.y, m_diffuse.z);
        shader->set3f("ambientColor", m_ambient.x, m_ambient.z, m_ambient.z);
        shader->setf("alpha", 1.0);
        shader->seti("applyShadow", params::inst()->applyShadow);
        shader->seti("renderMode", params::inst()->gridRenderMode);
        shader->setf("shadowIntensity", params::inst()->shadowIntensity);
        shader->set3f("attenuation", params::inst()->attenuation);
        shader->set2f("lightCone", params::inst()->lightCone);

        shader->setMatrix("matModel", model); 

        glEnable(GL_CULL_FACE);    
        glCullFace(GL_BACK);

        m_vbo->render();

        glCullFace(GL_FRONT);
        shader->setf("alpha", m_backFaceAlpha);
        shader->seti("renderMode", 1);

        m_vbo->render();

        glDisable(GL_CULL_FACE);    

    shader->release();

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
    VertexBufferObjectAttribs::DATA *data = new VertexBufferObjectAttribs::DATA[nrVertices];

    // first triangle
    data[0].vx = -m_size;
    data[0].vy = 0.0f;;
    data[0].vz = -m_size;
    data[0].vw = 1.0f;

    data[0].nx = 0.0f;
    data[0].ny = 1.0f;
    data[0].nz = 0.0f;
    data[0].nw = 0.0f;

    data[0].cx = 1.0f;
    data[0].cy = 1.0f;
    data[0].cz = 1.0f;
    data[0].cw = 1.0f;

    data[0].tx = 0.0f;
    data[0].ty = m_rep;
    data[0].tz = 0.0f;
    data[0].tw = 0.0f;

    data[1].vx = -m_size;
    data[1].vy = 0.0f;
    data[1].vz = m_size;
    data[1].vw = 1.0f;

    data[1].nx = 0.0f;
    data[1].ny = 1.0f;
    data[1].nz = 0.0f;
    data[1].nw = 0.0f;

    data[1].cx = 1.0f;
    data[1].cy = 1.0f;
    data[1].cz = 1.0f;
    data[1].cw = 1.0f;

    data[1].tx = 0.0f;
    data[1].ty = 0.0f;
    data[1].tz = 0.0f;
    data[1].tw = 0.0f;

    data[2].vx = m_size;
    data[2].vy = 0.0f;
    data[2].vz = m_size;
    data[2].vw = 1.0f;

    data[2].nx = 0.0f;
    data[2].ny = 1.0f;
    data[2].nz = 0.0f;
    data[2].nw = 0.0f;

    data[2].cx = 1.0f;
    data[2].cy = 1.0f;
    data[2].cz = 1.0f;
    data[2].cw = 1.0f;

    data[2].tx = m_rep;
    data[2].ty = 0.0f;
    data[2].tz = 0.0f;
    data[2].tw = 0.0f;

    // Second triangle
    data[3].vx = m_size;
    data[3].vy = 0.0f;
    data[3].vz = m_size;
    data[3].vw = 1.0f;

    data[3].nx = 0.0f;
    data[3].ny = 1.0f;
    data[3].nz = 0.0f;
    data[3].nw = 0.0f;

    data[3].cx = 1.0f;
    data[3].cy = 1.0f;
    data[3].cz = 1.0f;
    data[3].cw = 1.0f;

    data[3].tx = m_rep;
    data[3].ty = 0.0f;
    data[3].tz = 0.0f;
    data[3].tw = 0.0f;

    data[4].vx = m_size;
    data[4].vy = 0.0f;
    data[4].vz = -m_size;
    data[4].vw = 1.0f;

    data[4].nx = 0.0f;
    data[4].ny = 1.0f;
    data[4].nz = 0.0f;
    data[4].nw = 0.0f;

    data[4].cx = 1.0f;
    data[4].cy = 1.0f;
    data[4].cz = 1.0f;
    data[4].cw = 1.0f;

    data[4].tx = m_rep;
    data[4].ty = m_rep;
    data[4].tz = 0.0f;
    data[4].tw = 0.0f;

    data[5].vx = -m_size;
    data[5].vy = 0.0f;
    data[5].vz = -m_size;
    data[5].vw = 1.0f;

    data[5].nx = 0.0f;
    data[5].ny = 1.0f;
    data[5].nz = 0.0f;
    data[5].nw = 0.0f;

    data[5].cx = 1.0f;
    data[5].cy = 1.0f;
    data[5].cz = 1.0f;
    data[5].cw = 1.0f;

    data[5].tx = 0;
    data[5].ty = m_rep;
    data[5].tz = 0.0f;
    data[5].tw = 0.0f;

    m_vbo = new VertexBufferObjectAttribs();
    m_vbo->setData(data, GL_STATIC_DRAW, nrVertices, GL_TRIANGLES);

    m_vbo->addAttrib(VERTEX_POSITION);
    m_vbo->addAttrib(VERTEX_NORMAL);
    m_vbo->addAttrib(VERTEX_COLOR);
    m_vbo->addAttrib(VERTEX_TEXTURE);
    m_vbo->bindAttribs();

    delete[] data;
}
