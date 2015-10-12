#include "frustum.h"

Frustum::Frustum()
{
    m_angle = 45.0f;
    m_ratio = 1.0f;
    m_ncp = 1.0f;
    m_fcp = 1000.0f;

    m_pointVBO = new VertexBufferObjectAttribs();
    m_lineVBO = new VertexBufferObjectAttribs();
    m_planeVBO = new VertexBufferObjectAttribs();
    m_normalVBO = new VertexBufferObjectAttribs();

    m_pointVBO->addAttrib(VERTEX_POSITION);
    m_pointVBO->addAttrib(VERTEX_NORMAL);
    m_pointVBO->addAttrib(VERTEX_COLOR);
    m_pointVBO->addAttrib(VERTEX_TEXTURE);

    m_lineVBO->addAttrib(VERTEX_POSITION);
    m_lineVBO->addAttrib(VERTEX_NORMAL);
    m_lineVBO->addAttrib(VERTEX_COLOR);
    m_lineVBO->addAttrib(VERTEX_TEXTURE);

    m_planeVBO->addAttrib(VERTEX_POSITION);
    m_planeVBO->addAttrib(VERTEX_NORMAL);
    m_planeVBO->addAttrib(VERTEX_COLOR);
    m_planeVBO->addAttrib(VERTEX_TEXTURE);

    m_vertices.resize(8);

    m_lineIdxs = {0, 1, 1, 5, 5, 4, 4, 0,
                  3, 2, 2, 6, 6, 7, 7, 3,
                  0, 3, 3, 7, 7, 4, 4, 0,
                  1, 2, 2, 6, 6, 5, 5, 1,
                  0, 3, 3, 2, 2, 1, 1, 0,
                  4, 7, 7, 6, 6, 5, 5, 4};

    m_planeIdxs = {0, 1, 4, 1, 5, 4,
                   3, 2, 7, 2, 6, 7,
                   0, 4, 7, 7, 3, 0,
                   1, 2, 6, 1, 6, 5,
                   0, 3, 2, 2, 1, 0,
                   4, 7, 6, 6, 5, 4};

    glm::vec3 frustumColor(1.0, 1.0f, 0.0f);
    m_vertices[0].cx = frustumColor.x;
    m_vertices[0].cy = frustumColor.y;
    m_vertices[0].cz = frustumColor.z;

    m_vertices[1].cx = frustumColor.x;
    m_vertices[1].cy = frustumColor.y;
    m_vertices[1].cz = frustumColor.z;

    m_vertices[2].cx = frustumColor.x;
    m_vertices[2].cy = frustumColor.y;
    m_vertices[2].cz = frustumColor.z;

    m_vertices[3].cx = frustumColor.x;
    m_vertices[3].cy = frustumColor.y;
    m_vertices[3].cz = frustumColor.z;

    m_vertices[4].cx = frustumColor.x;
    m_vertices[4].cy = frustumColor.y;
    m_vertices[4].cz = frustumColor.z;

    m_vertices[5].cx = frustumColor.x;
    m_vertices[5].cy = frustumColor.y;
    m_vertices[5].cz = frustumColor.z;

    m_vertices[6].cx = frustumColor.x;
    m_vertices[6].cy = frustumColor.y;
    m_vertices[6].cz = frustumColor.z;

    m_vertices[7].cx = frustumColor.x;
    m_vertices[7].cy = frustumColor.y;
    m_vertices[7].cz = frustumColor.z;

}

Frustum::~Frustum(){
    delete m_pointVBO;
    delete m_lineVBO;
    delete m_planeVBO;
    delete m_normalVBO;
}

void Frustum::setCamInternals(float angle, float ratio, float ncp, float fcp) {
	m_angle = angle;
	m_ratio = ratio;
    m_ncp = ncp;
    m_fcp = fcp;

	m_tang = (float)tan(angle* math_radians * 0.5) ;
	
    nh = m_ncp * m_tang;
	nw = nh * ratio; 

	fh = m_fcp * m_tang;
	fw = fh * ratio;
}

void Frustum::setCamDef(glm::vec3 &p, glm::vec3 &l, glm::vec3 &u) {
	glm::vec3 dir, nc, fc, X, Y, Z;

	Z = l; 
    Z = glm::normalize(Z);

    //cout << "Z=" << Z.x << ", " << Z.y << ", " << Z.z << endl;

    X = glm::cross(u, Z);
    X = glm::normalize(X);
    //cout << "X=" << X.x << ", " << X.y << ", " << X.z << endl;

	Y = glm::cross(Z, X);

	nc = p + Z * m_ncp;
	fc = p + Z * m_fcp;

	ntl = nc + Y * nh - X * nw;
	ntr = nc + Y * nh + X * nw;
	nbl = nc - Y * nh - X * nw;
	nbr = nc - Y * nh + X * nw;

	ftl = fc + Y * fh - X * fw;
	ftr = fc + Y * fh + X * fw;
	fbl = fc - Y * fh - X * fw;
	fbr = fc - Y * fh + X * fw;

    m_vertices[0].vx = ntl.x;
    m_vertices[0].vy = ntl.y;
    m_vertices[0].vz = ntl.z;

    m_vertices[1].vx = ntr.x;
    m_vertices[1].vy = ntr.y;
    m_vertices[1].vz = ntr.z;

    m_vertices[2].vx = nbr.x;
    m_vertices[2].vy = nbr.y;
    m_vertices[2].vz = nbr.z;

    m_vertices[3].vx = nbl.x;
    m_vertices[3].vy = nbl.y;
    m_vertices[3].vz = nbl.z;

    m_vertices[4].vx = ftl.x;
    m_vertices[4].vy = ftl.y;
    m_vertices[4].vz = ftl.z;

    m_vertices[5].vx = ftr.x;
    m_vertices[5].vy = ftr.y;
    m_vertices[5].vz = ftr.z;

    m_vertices[6].vx = fbr.x;
    m_vertices[6].vy = fbr.y;
    m_vertices[6].vz = fbr.z;

    m_vertices[7].vx = fbl.x;
    m_vertices[7].vy = fbl.y;
    m_vertices[7].vz = fbl.z;

    //cout << "ntl: " << ntl.x << ", " << ntl.y << ", " << ntl.z << endl;

    m_pointVBO->setData(&m_vertices[0], GL_STATIC_DRAW, m_vertices.size(), GL_POINTS);
    m_pointVBO->bindAttribs();

    m_lineVBO->setData(&m_vertices[0], GL_STATIC_DRAW, m_vertices.size(), GL_LINES);
    m_lineVBO->setIndexData(&m_lineIdxs[0], GL_STATIC_DRAW, m_lineIdxs.size());
    m_lineVBO->bindAttribs();

    m_planeVBO->setData(&m_vertices[0], GL_STATIC_DRAW, m_vertices.size(), GL_TRIANGLES);
    m_planeVBO->setIndexData(&m_planeIdxs[0], GL_STATIC_DRAW, m_planeIdxs.size());
    m_planeVBO->bindAttribs();
}

void Frustum::drawPoints() 
{
    m_pointVBO->render();
}


void Frustum::drawLines() 
{
    m_lineVBO->render();	
}


void Frustum::drawPlanes() 
{    
    m_planeVBO->render();	
}

void Frustum::drawNormals() 
{
}
