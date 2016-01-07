#include <stdio.h>
#include "renderable_object.h"

RenderableObject::RenderableObject()
: m_useIndexBuffer(false),
  m_vao(0),
  m_vbo(0),
  m_ebo(0),    
  m_nVertices(0),
  m_primitiveMode(GL_POINTS),
  m_usage(GL_STATIC_DRAW)
{
    params::inst().glFuncs->glGenVertexArrays(1, &m_vao);
    params::inst().glFuncs->glGenBuffers(1, &m_vbo);
    params::inst().glFuncs->glGenBuffers(1, &m_ebo);
}

RenderableObject::~RenderableObject()
{
    if(m_vao != GL_INVALID_VALUE)
        params::inst().glFuncs->glDeleteVertexArrays(1, &m_vao);

    if(m_vbo != GL_INVALID_VALUE)
        params::inst().glFuncs->glDeleteBuffers(1, &m_vbo);

    if(m_ebo != GL_INVALID_VALUE)
        params::inst().glFuncs->glDeleteBuffers(1, &m_ebo);
}

void RenderableObject::bind()
{
    params::inst().glFuncs->glBindVertexArray(m_vao);
}

void RenderableObject::release()
{
    params::inst().glFuncs->glBindVertexArray(0);
}

void RenderableObject::render()
{    
    if(m_nVertices == 0) { 
        return; 
    } 

    params::inst().glFuncs->glBindVertexArray(m_vao);
    if(!m_useIndexBuffer){
        params::inst().glFuncs->glDrawArrays(m_primitiveMode, 0, m_nVertices);
    }
    else{
        params::inst().glFuncs->glDrawElements(m_primitiveMode, 
                       m_nVertices,
                       GL_UNSIGNED_INT,
                       0);
    }
    params::inst().glFuncs->glBindVertexArray(0);
}

/*=====================================================================================
        Transfer data to VBO

        - usage: GL_STATIC_DRAW, GL_DYNAMIC_DRAW
        - primitiveMode: GL_POINTS, GL_LINES, GL_TRIANGLES, etc.

=====================================================================================*/
void RenderableObject::setData(vector<Vertex> data,
                               GLenum primitiveMode,
                               GLenum usage){
    m_vertices = std::move(data);

    m_useIndexBuffer = false;
    m_nVertices = m_vertices.size(); 
    m_primitiveMode = primitiveMode;
    m_usage = usage;

    this->bind();
    // Bind vertex buffer
    params::inst().glFuncs->glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo);

    params::inst().glFuncs->glBufferData(GL_ARRAY_BUFFER, 
                 m_vertices.size() * sizeof(Vertex),
                 &m_vertices[0],
                 m_usage);

    // Set the vertex attribute pointers
    // Vertex Position
    params::inst().glFuncs->glEnableVertexAttribArray(0);
    params::inst().glFuncs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    // Vertex Normal 
    params::inst().glFuncs->glEnableVertexAttribArray(1);
    params::inst().glFuncs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
    // Vertex Color
    params::inst().glFuncs->glEnableVertexAttribArray(2);
    params::inst().glFuncs->glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Color));
    // Vertex Texture Coords
    params::inst().glFuncs->glEnableVertexAttribArray(3);
    params::inst().glFuncs->glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

    this->release();
}

void RenderableObject::setData(vector<Vertex> data,
                               vector<GLuint> indices,
                               GLenum primitiveMode,
                               GLenum usage){
    this->m_vertices = std::move(data);
    this->m_indices  = std::move(indices);

    m_useIndexBuffer = true;
    m_nVertices = m_indices.size();
    m_primitiveMode = primitiveMode;
    m_usage = usage;

    this->bind();
    // Bind vertex buffer
    params::inst().glFuncs->glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo);

    params::inst().glFuncs->glBufferData(GL_ARRAY_BUFFER, 
                 m_vertices.size() * sizeof(Vertex),
                 &m_vertices[0],
                 m_usage);

    params::inst().glFuncs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ebo);
    params::inst().glFuncs->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 m_indices.size() * sizeof(GLuint),
                 &m_indices[0],
                 m_usage);

    // Set the vertex attribute pointers
    // Vertex Position
    params::inst().glFuncs->glEnableVertexAttribArray(0);
    params::inst().glFuncs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    // Vertex Normal 
    params::inst().glFuncs->glEnableVertexAttribArray(1);
    params::inst().glFuncs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
    // Vertex Color
    params::inst().glFuncs->glEnableVertexAttribArray(2);
    params::inst().glFuncs->glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Color));
    // Vertex Texture Coords
    params::inst().glFuncs->glEnableVertexAttribArray(3);
    params::inst().glFuncs->glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

    this->release();
}

unique_ptr<RenderableObject> RenderableObject::quadLines(int startX, 
                                                         int startY, 
                                                         int width, 
                                                         int height, 
                                                         const glm::vec4 &color)
{
    glm::vec3 mi(startX, startY, 0.0f);
    glm::vec3 ma(startX + width, startY + height, 0.0f);

    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;

    float d = 0.1;

    vertices.push_back(glm::vec3(mi.x, mi.y, d));
    vertices.push_back(glm::vec3(mi.x, ma.y, d));

    vertices.push_back(glm::vec3(mi.x, ma.y, d));
    vertices.push_back(glm::vec3(ma.x, ma.y, d));

    vertices.push_back(glm::vec3(ma.x, ma.y, d));
    vertices.push_back(glm::vec3(ma.x, mi.y, d));

    vertices.push_back(glm::vec3(ma.x, mi.y, d));
    vertices.push_back(glm::vec3(mi.x, mi.y, d));

    uint nrVertices = vertices.size();
    vector<RenderableObject::Vertex> attrData(nrVertices);

    for(uint i=0; i<nrVertices; ++i)
    {    
        glm::vec3 v = vertices[i];

        attrData[i].Position = v;
        attrData[i].Color = color;
    }

    unique_ptr<RenderableObject> vbo(new RenderableObject);
    vbo->setData(attrData, GL_LINES);

    return vbo;
}

unique_ptr<RenderableObject> RenderableObject::quad(int startX, 
                                                    int startY, 
                                                    int width, 
                                                    int height, 
                                                    const glm::vec4 &color, GLenum primitive)
{
    glm::vec3 mi(startX, startY, 0.0f);
    glm::vec3 ma(startX + width, startY + height, 0.0f);

    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;
    vector<glm::vec3> texCoords;

    float d = 0.1;

    vertices.push_back(glm::vec3(mi.x, mi.y, d));
    vertices.push_back(glm::vec3(mi.x, ma.y, d));
    vertices.push_back(glm::vec3(ma.x, ma.y, d));
    vertices.push_back(glm::vec3(ma.x, mi.y, d));

    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

    texCoords.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    texCoords.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    texCoords.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
    texCoords.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

    // Indices
    vector<GLuint> indices ={
        0, 1, 2,
        0, 2, 3
    };

    uint nrVertices = vertices.size();
    vector<RenderableObject::Vertex> attrData(nrVertices);

    for(uint i=0; i<nrVertices; ++i)
    {    
        glm::vec3 v = vertices[i];
        glm::vec3 n = normals[i];
        glm::vec3 t = texCoords[i];

        attrData[i].Position = v;
        attrData[i].Normal = n;
        attrData[i].Color = color;
        attrData[i].TexCoords = glm::vec4(t.x, t.y, 0.0f, 0.0f);
    }

    unique_ptr<RenderableObject> vbo(new RenderableObject);
    vbo->setData(attrData, indices, primitive);

    return vbo;
}

unique_ptr<RenderableObject> RenderableObject::quad(int width, 
                                                    int height, 
                                                    const glm::vec4 &color, 
                                                    GLenum primitive)
{
    glm::vec3 mi(-width/2.0f, -height/2.0f, 0.0f);
    glm::vec3 ma(width/2.0f, height/2.0f, 0.0f);

    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;
    vector<glm::vec3> texCoords;

    vertices.push_back(glm::vec3(mi.x, mi.y, 0.0f));
    vertices.push_back(glm::vec3(mi.x, ma.y, 0.0f));
    vertices.push_back(glm::vec3(ma.x, ma.y, 0.0f));
    vertices.push_back(glm::vec3(ma.x, mi.y, 0.0f));

    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

    texCoords.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    texCoords.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    texCoords.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
    texCoords.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

    // Indices
    vector<GLuint> indices ={
        0, 1, 2,
        0, 2, 3
    };

    uint nrVertices = vertices.size();
    vector<RenderableObject::Vertex> attrData(nrVertices);

    for(uint i=0; i<nrVertices; ++i)
    {    
        glm::vec3 v = vertices[i];
        glm::vec3 n = normals[i];
        glm::vec3 t = texCoords[i];

        attrData[i].Position = v;
        attrData[i].Normal = n;
        attrData[i].Color = color;
        attrData[i].TexCoords = glm::vec4(t.x, t.y, 0.0f, 0.0f);
    }

    unique_ptr<RenderableObject> vbo(new RenderableObject);
    vbo->setData(attrData, indices, primitive);

    return vbo;
}

unique_ptr<RenderableObject> RenderableObject::box(const glm::vec3 &mi, 
                                                   const glm::vec3 &ma, 
                                                   const glm::vec4 &color, 
                                                   GLenum primitive)
{
    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;

    // Face 1
    vertices.push_back(glm::vec3(mi.x, ma.y, mi.z));
    vertices.push_back(glm::vec3(mi.x, ma.y, ma.z));
    vertices.push_back(glm::vec3(ma.x, ma.y, ma.z));
    vertices.push_back(glm::vec3(ma.x, ma.y, mi.z));

    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

    // Face 2
    vertices.push_back(glm::vec3(mi.x, mi.y, mi.z));
    vertices.push_back(glm::vec3(ma.x, mi.y, mi.z));
    vertices.push_back(glm::vec3(ma.x, mi.y, ma.z));
    vertices.push_back(glm::vec3(mi.x, mi.y, ma.z));

    normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
    normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));    

    // Face 3
    vertices.push_back(glm::vec3(mi.x, mi.y, mi.z));
    vertices.push_back(glm::vec3(mi.x, ma.y, mi.z));
    vertices.push_back(glm::vec3(ma.x, ma.y, mi.z));
    vertices.push_back(glm::vec3(ma.x, mi.y, mi.z));

    normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));

    // Face 4
    vertices.push_back(glm::vec3(mi.x, mi.y, ma.z));
    vertices.push_back(glm::vec3(ma.x, mi.y, ma.z));
    vertices.push_back(glm::vec3(ma.x, ma.y, ma.z));
    vertices.push_back(glm::vec3(mi.x, ma.y, ma.z));

    normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

    // Face 5
    vertices.push_back(glm::vec3(mi.x, mi.y, mi.z));
    vertices.push_back(glm::vec3(mi.x, mi.y, ma.z));
    vertices.push_back(glm::vec3(mi.x, ma.y, ma.z));
    vertices.push_back(glm::vec3(mi.x, ma.y, mi.z));

    normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
    normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
    normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
    normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));

    // Face 6
    vertices.push_back(glm::vec3(ma.x, mi.y, mi.z));
    vertices.push_back(glm::vec3(ma.x, ma.y, mi.z));
    vertices.push_back(glm::vec3(ma.x, ma.y, ma.z));
    vertices.push_back(glm::vec3(ma.x, mi.y, ma.z));

    normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

    // Indices
    vector<GLuint> indices ={
        0, 1, 2,
        0, 2, 3,
        4, 5, 6,
        4, 6, 7,
        8, 9, 10,
        8, 10, 11,
        12, 13, 14,
        12, 14, 15
    };

    uint nrVertices = vertices.size();
    vector<RenderableObject::Vertex> attrData(nrVertices);

    for(uint i=0; i<nrVertices; ++i)
    {    
        glm::vec3 v = vertices[i];
        glm::vec3 n = normals[i];

        attrData[i].Position = v;
        attrData[i].Normal = n;
        attrData[i].Color = color;
    }

    unique_ptr<RenderableObject> vbo(new RenderableObject);
    vbo->setData(attrData, indices, primitive);

    return vbo;
}

unique_ptr<RenderableObject> RenderableObject::sphere(float radius, 
                                                      int iterations, 
                                                      const glm::vec4 &color, 
                                                      GLenum primitive)
{
    vector<FACET3> f((int)pow(4.0, iterations));
    int n = CreateUnitSphere(f, iterations);

    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;

    for(int i=0; i<n; ++i)
    {
        FACET3& facet = f[i];
        vertices.push_back(facet.p1 * radius);
        vertices.push_back(facet.p2 * radius);
        vertices.push_back(facet.p3 * radius);

        facet.p1 = glm::normalize(facet.p1);
        facet.p2 = glm::normalize(facet.p2);
        facet.p3 = glm::normalize(facet.p3);
        normals.push_back(facet.p1);
        normals.push_back(facet.p2);
        normals.push_back(facet.p3);
    }

    uint nrVertices = vertices.size();
    vector<RenderableObject::Vertex> attrData(nrVertices);

    for(uint i=0; i<nrVertices; ++i)
    {    
        glm::vec3 v = vertices[i];
        glm::vec3 n = normals[i];

        attrData[i].Position = v;
        attrData[i].Normal = n;
        attrData[i].Color = color;
    }

    unique_ptr<RenderableObject> vbo(new RenderableObject);
    vbo->setData(attrData, primitive);

    return vbo;
}

int RenderableObject::createNSphere(vector<FACET3>& f, int iterations)
{
    int i,it;
    double a;
    glm::vec3 p[6];

    p[0] = glm::vec3(0, 0, 1);
    p[1] = glm::vec3(0, 0, -1);
    p[2] = glm::vec3(-1, -1, 0);
    p[3] = glm::vec3(1, -1, 0);
    p[4] = glm::vec3(1, 1, 0);
    p[5] = glm::vec3(-1, 1, 0);

    glm::vec3 pa, pb, pc;
    int nt = 0, ntold;

    /* Create the level 0 object */
    a = 1 / sqrt(2.0);
    for (i=0;i<6;i++) 
    {
        p[i].x *= a;
        p[i].y *= a;
    }

    f[0].p1 = p[0]; f[0].p2 = p[3]; f[0].p3 = p[4];
    f[1].p1 = p[0]; f[1].p2 = p[4]; f[1].p3 = p[5];
    f[2].p1 = p[0]; f[2].p2 = p[5]; f[2].p3 = p[2];
    f[3].p1 = p[0]; f[3].p2 = p[2]; f[3].p3 = p[3];
    f[4].p1 = p[1]; f[4].p2 = p[4]; f[4].p3 = p[3];
    f[5].p1 = p[1]; f[5].p2 = p[5]; f[5].p3 = p[4];
    f[6].p1 = p[1]; f[6].p2 = p[2]; f[6].p3 = p[5];
    f[7].p1 = p[1]; f[7].p2 = p[3]; f[7].p3 = p[2];
    nt = 8;

    if (iterations < 1)
        return(nt);

    /* Bisect each edge and move to the surface of a unit sphere */
    for (it=0;it<iterations;it++) 
    {
        ntold = nt;
        for (i=0;i<ntold;i++) 
        {
            pa.x = (f[i].p1.x + f[i].p2.x) / 2;
            pa.y = (f[i].p1.y + f[i].p2.y) / 2;
            pa.z = (f[i].p1.z + f[i].p2.z) / 2;
            pb.x = (f[i].p2.x + f[i].p3.x) / 2;
            pb.y = (f[i].p2.y + f[i].p3.y) / 2;
            pb.z = (f[i].p2.z + f[i].p3.z) / 2;
            pc.x = (f[i].p3.x + f[i].p1.x) / 2;
            pc.y = (f[i].p3.y + f[i].p1.y) / 2;
            pc.z = (f[i].p3.z + f[i].p1.z) / 2;
            pa = glm::normalize(pa);
            pb = glm::normalize(pb);
            pc = glm::normalize(pc);
            f[nt].p1 = f[i].p1; f[nt].p2 = pa; f[nt].p3 = pc; nt++;
            f[nt].p1 = pa; f[nt].p2 = f[i].p2; f[nt].p3 = pb; nt++;
            f[nt].p1 = pb; f[nt].p2 = f[i].p3; f[nt].p3 = pc; nt++;
            f[i].p1 = pa;
            f[i].p2 = pb;
            f[i].p3 = pc;
        }
    }

    return(nt);
}

int RenderableObject::CreateUnitSphere(vector<FACET3>& facets, int iterations)
{
    int i,j,n,nstart;
    glm::vec3 p1(1.0,1.0,1.0),  p2(-1.0,-1.0,1.0);
    glm::vec3 p3(1.0,-1.0,-1.0), p4(-1.0,1.0,-1.0); 

    p1 = glm::normalize(p1);
    p2 = glm::normalize(p2);
    p3 = glm::normalize(p3);
    p4 = glm::normalize(p4);

    facets[0].p1 = p1; facets[0].p2 = p2; facets[0].p3 = p3;
    facets[1].p1 = p2; facets[1].p2 = p1; facets[1].p3 = p4;
    facets[2].p1 = p2; facets[2].p2 = p4; facets[2].p3 = p3;
    facets[3].p1 = p1; facets[3].p2 = p3; facets[3].p3 = p4;

    n = 4;

    for (i=1;i<iterations;i++) 
    {
        nstart = n;

        for (j=0;j<nstart;j++) 
        {

            /* Create initially copies for the new facets */
            facets[n  ] = facets[j];
            facets[n+1] = facets[j];
            facets[n+2] = facets[j];

            /* Calculate the midpoints */
            p1 = MidPoint(facets[j].p1,facets[j].p2);
            p2 = MidPoint(facets[j].p2,facets[j].p3);
            p3 = MidPoint(facets[j].p3,facets[j].p1);

            /* Replace the current facet */
            facets[j].p2 = p1;
            facets[j].p3 = p3;

            /* Create the changed vertices in the new facets */
            facets[n  ].p1 = p1;
            facets[n  ].p3 = p2;
            facets[n+1].p1 = p3;
            facets[n+1].p2 = p2;
            facets[n+2].p1 = p1;
            facets[n+2].p2 = p2;
            facets[n+2].p3 = p3;
            n += 3;
        }
    }

    for (j=0;j<n;j++) 
    {
        facets[j].p1 = glm::normalize(facets[j].p1);
        facets[j].p2 = glm::normalize(facets[j].p2);
        facets[j].p3 = glm::normalize(facets[j].p3);
    }

    return(n);
}

glm::vec3 RenderableObject::MidPoint(glm::vec3 p1, glm::vec3 p2)
{
    glm::vec3 p;

    p.x = (p1.x + p2.x) / 2.0f;
    p.y = (p1.y + p2.y) / 2.0f;
    p.z = (p1.z + p2.z) / 2.0f;

    return(p);
}
