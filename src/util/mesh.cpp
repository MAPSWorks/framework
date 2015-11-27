#include "mesh.h"
#include "renderable_object.h"
#include "model_loader_obj.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

vector<RenderableObject *> Mesh::obj(const QString &fileName, const glm::vec3 &rot, const glm::vec3 &scale, GLenum primitive)
{
    vector<RenderableObject *> vbos;

    ModelOBJ *model = new ModelOBJ();
    std::vector<glm::vec3> tempVertices;	

    std::cout << "\tOBSTACLEOBJ::load():" << fileName.toStdString() << std::endl;
    model->import(fileName.toStdString().c_str());

    uint nrMeshes = model->getNumberOfMeshes();

    const ModelOBJ::Vertex *vb = model->getVertexBuffer();
    const int *tempIdx = model->getIndexBuffer();

    glm::mat4 rotMatX, rotMatY, rotMatZ, scaleMat;
    rotMatX = glm::rotate(rotMatX, rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
    rotMatY = glm::rotate(rotMatY, rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
    rotMatZ = glm::rotate(rotMatZ, rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMat = glm::scale(scaleMat, glm::vec3(scale.x, scale.y, scale.z));

    for(uint i = 0; i < nrMeshes; ++i)	
    {
        std::vector<glm::vec3> tempVertices;	
        std::vector<glm::vec3> tmpNormals;
        std::vector<glm::vec3> tmpTexCoords;

        const ModelOBJ::Mesh &objMesh = model->getMesh(i);
        int startIndex = objMesh.startIndex;
        int vertexCount = objMesh.triangleCount * 3;

        for(int j=startIndex; j<startIndex + vertexCount; ++j)
        {
            uint idx = tempIdx[j];
            glm::vec3 v(vb[idx].position[0], vb[idx].position[1], vb[idx].position[2]);
            glm::vec3 n(vb[idx].normal[0], vb[idx].normal[1], vb[idx].normal[2]);
            glm::vec3 t(vb[idx].texCoord[0], vb[idx].texCoord[1], 0.0f);            

            tempVertices.push_back(v);
            tmpNormals.push_back(n);
            tmpTexCoords.push_back(t);
        } 

        glm::vec3 mi(math_maxfloat, math_maxfloat, math_maxfloat);
        glm::vec3 ma(math_minfloat, math_minfloat, math_minfloat);

        vector<RenderableObject::Vertex> data(tempVertices.size());

        for(uint i=0; i<tempVertices.size(); ++i)
        {
            RenderableObject::Vertex d;

            glm::vec3 v = tempVertices[i];
            glm::vec3 n = tmpNormals[i];
            glm::vec3 t = tmpTexCoords[i];

            d.Position = v;
            d.Color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            d.Normal = n;
            
            d.TexCoords = glm::vec2(t.x, t.y);
            data[i] = d;
        }

        RenderableObject* vboMesh = new RenderableObject();
        vboMesh->setData(data, primitive); 

        vbos.push_back(vboMesh);
    }

    return vbos;
}

vector<RenderableObject*> loadObj(QString fileName)
{
    vector <RenderableObject*> vbos;

    ModelOBJ *model = new ModelOBJ();
    std::vector<glm::vec3> tempVertices;

    std::cout << "\tOBSTACLEOBJ::load():" << fileName.toStdString() << std::endl;
    model->import(fileName.toStdString().c_str());

    uint nrMeshes = model->getNumberOfMeshes();

    const ModelOBJ::Vertex *vb = model->getVertexBuffer();
    const int *tempIdx = model->getIndexBuffer();


    glm::vec3 scale(10, 10, 10);
    glm::vec3 rot(0, 0, 0);

    glm::mat4 rotMatX, rotMatY, rotMatZ, scaleMat;
    rotMatX = glm::rotate(rotMatX, rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
    rotMatY = glm::rotate(rotMatY, rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
    rotMatZ = glm::rotate(rotMatZ, rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMat = glm::scale(scaleMat, glm::vec3(scale.x, scale.y, scale.z));

    for (uint i = 0; i < nrMeshes; ++i)
    {
        std::vector<glm::vec3> tempVertices;
        std::vector<glm::vec3> tmpNormals;
        std::vector<glm::vec3> tmpTexCoords;

        const ModelOBJ::Mesh &objMesh = model->getMesh(i);
        int startIndex = objMesh.startIndex;
        int vertexCount = objMesh.triangleCount * 3;

        for (int j = startIndex; j<startIndex + vertexCount; ++j)
        {
            uint idx = tempIdx[j];
            glm::vec3 v(vb[idx].position[0], vb[idx].position[1], vb[idx].position[2]);
            glm::vec3 n(vb[idx].normal[0], vb[idx].normal[1], vb[idx].normal[2]);
            glm::vec3 t(vb[idx].texCoord[0], vb[idx].texCoord[1], 0.0f);

            tempVertices.push_back(v);
            tmpNormals.push_back(n);
            tmpTexCoords.push_back(t);
        }

        vector<RenderableObject::Vertex> data(tempVertices.size());

        for (uint i = 0; i<tempVertices.size(); ++i)
        {
            RenderableObject::Vertex d;
            glm::vec3 pos = tempVertices[i];
            glm::vec3 n = tmpNormals[i];
            glm::vec3 t = tmpTexCoords[i];

            d.Position = pos;
            d.Color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            d.Normal = n;
            d.TexCoords = glm::vec2(t.x, t.y);

            data[i] = d;
        }

        RenderableObject* vboMesh = new RenderableObject();
        vboMesh->setData(data, GL_TRIANGLES);

        vbos.push_back(vboMesh);
    }

    return vbos;
}

RenderableObject *Mesh::quadLines(int startX, int startY, int width, int height, const glm::vec4 &color)
{
    glm::vec3 mi(startX, startY, 0.0f);
    glm::vec3 ma(startX + width, startY + height, 0.0f);

    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;
    vector<glm::vec3> texCoords;

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

    RenderableObject *vbo = new RenderableObject();
    vbo->setData(attrData, GL_LINES);

    return vbo;
}

RenderableObject *Mesh::quad(int startX, int startY, int width, int height, const glm::vec4 &color, GLenum primitive)
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
        attrData[i].TexCoords = glm::vec2(t.x, t.y);
    }

    RenderableObject *vbo = new RenderableObject();
    vbo->setData(attrData, indices, primitive);

    return vbo;
}

RenderableObject *Mesh::quad(int width, int height, const glm::vec4 &color, GLenum primitive)
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
        attrData[i].TexCoords = glm::vec2(t.x, t.y);
    }

    RenderableObject *vbo = new RenderableObject();
    vbo->setData(attrData, indices, primitive);

    return vbo;
}

RenderableObject *Mesh::box(const glm::vec3 &mi, const glm::vec3 &ma, const glm::vec4 &color, GLenum primitive)
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

    RenderableObject *vbo = new RenderableObject();
    vbo->setData(attrData, indices, primitive);

    return vbo;
}

RenderableObject *Mesh::sphere(float radius, int iterations, const glm::vec4 &color, GLenum primitive)
{
    FACET3 *f = new FACET3[(int)pow(4.0, iterations)];
    int n = CreateUnitSphere(f, iterations);

    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;

    for(int i=0; i<n; ++i)
    {
        FACET3 facet = f[i];
        vertices.push_back(facet.p1 * radius);
        vertices.push_back(facet.p2 * radius);
        vertices.push_back(facet.p3 * radius);
        //cout << "Face " << i << ": " << glm::to_string(facet.p1) << ", " << glm::to_string(facet.p2) << ", " << glm::to_string(facet.p3) << endl;

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

    RenderableObject *vbo = new RenderableObject();
    vbo->setData(attrData, primitive);

    return vbo;
}

int Mesh::createNSphere(FACET3 *f, int iterations)
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

int Mesh::CreateUnitSphere(FACET3 *facets, int iterations)
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

glm::vec3 Mesh::MidPoint(glm::vec3 p1, glm::vec3 p2)
{
    glm::vec3 p;

    p.x = (p1.x + p2.x) / 2.0f;
    p.y = (p1.y + p2.y) / 2.0f;
    p.z = (p1.z + p2.z) / 2.0f;

    return(p);
}
