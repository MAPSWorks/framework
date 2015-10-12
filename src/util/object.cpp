#include "object.h"
#include "shader.h"
#include "vertexbuffer_object_attribs.h"
#include "model_loader_obj.h"

Object::Object(const QString &fileName, const glm::vec3 &pos, const glm::vec3 &scale, const glm::vec4 &rot, const glm::vec4 &color)
: m_fileName(fileName),
  m_position(pos),
  m_scale(scale),
  m_rotation(rot),
  m_color(color),
  m_isSelected(false),
  m_max(math_minfloat, math_minfloat, math_minfloat),
  m_min(math_maxfloat, math_maxfloat, math_maxfloat),
  m_nrTriangles(0),
  m_nrVertices(0),
  m_up(0, 1, 0, 1)
{
    init();
}

Object::~Object()
{

    for(int i=m_vbosTriangles.size()-1; i>=0; --i)
    {
        VertexBufferObjectAttribs *vbo = m_vbosTriangles[i];
        delete vbo;
    }

    for(int i=m_vbosLines.size()-1; i>=0; --i)
    {
        VertexBufferObjectAttribs *vbo = m_vbosLines[i];
        delete vbo;
    }
}

void Object::init()
{
    buildVBOs(m_fileName, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
}

void Object::render(Shader* shader)
{
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

    glEnable(GL_CLIP_DISTANCE0);    

	shader->bind();  

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, params::inst()->shadowMapID);    
        shader->seti("shadowMap", 1);   

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, params::inst()->shadowMapBlurredID);    
        shader->seti("shadowMapBlurred", 2);  

        shader->set3f("lightPos", params::inst()->lightPos);        
        shader->set3f("camPos", params::inst()->camPos);      
        shader->seti("applyShadow", params::inst()->applyShadow);
        shader->setf("shadowIntensity", params::inst()->shadowIntensity);
        shader->seti("isSelected", m_isSelected);

        shader->set4f("clipPlane", params::inst()->clipPlaneGround);

        for(uint i=0; i<m_vbosTriangles.size(); ++i)
        {
            m_vbosTriangles[i]->render();
        }

        if (params::inst()->renderMesh)
        {

            for(uint i=0; i<m_vbosLines.size(); ++i)
            {
                m_vbosLines[i]->render();
            }

        }

	shader->release();

	glDisable(GL_CULL_FACE);    
}

void Object::buildVBOs(const QString &fileName, const glm::vec3 &rot, const glm::vec3 &scale)
{
    ModelOBJ *model = new ModelOBJ();

    std::cout << "\tOBSTACLEOBJ::load():" << fileName.toStdString() << std::endl;;
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
		vector<glm::vec3> tmpVertices;	
		vector<glm::vec3> tmpNormals;
		vector<glm::vec3> tmpTexCoords;

		const ModelOBJ::Mesh &objMesh = model->getMesh(i);
		int startIndex = objMesh.startIndex;
		m_nrTriangles = objMesh.triangleCount;
		m_nrVertices = objMesh.triangleCount * 3;

		for(int j=startIndex; j<startIndex + m_nrVertices; ++j)
		{
			uint idx = tempIdx[j];

            glm::vec3 v(vb[idx].position[0], vb[idx].position[1], vb[idx].position[2]);
            glm::vec3 n(vb[idx].normal[0], vb[idx].normal[1], vb[idx].normal[2]);
            glm::vec3 t(vb[idx].texCoord[0], vb[idx].texCoord[1], 0.0f);            

			tmpVertices.push_back(v);
			tmpNormals.push_back(n);
			tmpTexCoords.push_back(t);
		} 

        glm::vec3 mi(math_maxfloat, math_maxfloat, math_maxfloat);
        glm::vec3 ma(math_minfloat, math_minfloat, math_minfloat);

        for(uint i=0; i<tmpVertices.size(); ++i)
        {
            glm::vec3 v = tmpVertices[i];
            
            if(v.x > m_max.x)
                m_max.x = v.x;

            if(v.y > m_max.y)
                m_max.y = v.y;

            if(v.z > m_max.z)
                m_max.z = v.z;

            if(v.x < m_min.x)
                m_min.x = v.x;

            if(v.y < m_min.y)
                m_min.y = v.y;

            if(v.z < m_min.z)
                m_min.z = v.z;
        }        

        m_center =  (glm::vec4(m_min, 1.0f) + glm::vec4(m_max, 1.0f)) * 0.5f;

        VertexBufferObjectAttribs::DATA *data = new VertexBufferObjectAttribs::DATA[tmpVertices.size()];

		for(uint i=0; i<tmpVertices.size(); ++i)
		{
            glm::vec3 v = tmpVertices[i];
            glm::vec3 n = tmpNormals[i];
            glm::vec3 t = tmpTexCoords[i];

			data[i].vx = v.x;
			data[i].vy = v.y;
			data[i].vz = v.z;
			data[i].vw = 1.0f;

            data[i].cx = m_color.x;
            data[i].cy = m_color.y;
            data[i].cz = m_color.z;
			data[i].cw = m_color.w;
			
            data[i].nx = n.x;
			data[i].ny = n.y;
			data[i].nz = n.z;
			data[i].nw = 1.0f;
            
			data[i].tx = t.x;
			data[i].ty = t.y;
            data[i].tz = 0.0f;
            data[i].tw = 0.0f;
		}

		VertexBufferObjectAttribs* vboMesh = new VertexBufferObjectAttribs();
		vboMesh->setData(data, GL_STATIC_DRAW, tmpVertices.size(), GL_TRIANGLES); 

		vboMesh->addAttrib(VERTEX_POSITION);
		vboMesh->addAttrib(VERTEX_NORMAL);
		vboMesh->addAttrib(VERTEX_COLOR);
		vboMesh->addAttrib(VERTEX_TEXTURE);
		vboMesh->bindAttribs();

		VertexBufferObjectAttribs* vboLines = new VertexBufferObjectAttribs();
		vboLines->setData(data, GL_STATIC_DRAW, tmpVertices.size(), GL_LINES); 

		vboLines->addAttrib(VERTEX_POSITION);
		vboLines->addAttrib(VERTEX_NORMAL);
		vboLines->addAttrib(VERTEX_COLOR);
		vboLines->addAttrib(VERTEX_TEXTURE);
		vboLines->bindAttribs();

		delete[] data;

        m_vbosTriangles.push_back(vboMesh);
        m_vbosLines.push_back(vboLines);
    }
}
