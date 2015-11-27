#include "frustum.h"

Frustum::Frustum()
{
    m_angle = 45.0f;
    m_ratio = 1.0f;
    m_ncp = 1.0f;
    m_fcp = 1000.0f;
    
    m_pointVBO = new RenderableObject();
    m_lineVBO = new RenderableObject();
    m_planeVBO = new RenderableObject();
    m_normalVBO = new RenderableObject();

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

    glm::vec4 frustumColor(1.0, 1.0f, 0.0f, 1.0f);
    m_vertices[0].Color = frustumColor;
    m_vertices[1].Color = frustumColor;
    m_vertices[2].Color = frustumColor;
    m_vertices[3].Color = frustumColor;
    m_vertices[4].Color = frustumColor;
    m_vertices[5].Color = frustumColor;
    m_vertices[6].Color = frustumColor;
    m_vertices[7].Color = frustumColor;
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

    m_vertices[0].Position = ntl;
    m_vertices[1].Position = ntr;
    m_vertices[2].Position = nbr;
    m_vertices[3].Position = nbl;
    m_vertices[4].Position = ftl;
    m_vertices[5].Position = ftr;
    m_vertices[6].Position = fbr;
    m_vertices[7].Position = fbl;

    m_pointVBO->setData(m_vertices, GL_POINTS);
    m_lineVBO->setData(m_vertices, m_lineIdxs, GL_LINES);
    m_planeVBO->setData(m_vertices, m_planeIdxs, GL_TRIANGLES);
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
