/*=====================================================================================
                          frustum.h

            Description:  Frustum

            Created by Chen Chen on 09/28/2015
=====================================================================================*/

#ifndef FRUSTUM_H_
#define FRUSTUM_H_

#include "headers.h"
#include "renderable_object.h"

class RenderableObject;

class Frustum 
{
    public:
        enum {OUTSIDE, INTERSECT, INSIDE};

        Frustum();
        ~Frustum();

        void setCamInternals(float angle, float ratio, float ncp, float fcp);
        void setCamDef(glm::vec3 &p, glm::vec3 &l, glm::vec3 &u);

        void drawPoints();
        void drawLines();
        void drawPlanes();
        void drawNormals();

    private:

        float m_angle;
        float m_ratio;
        float m_ncp;
        float m_fcp;
        float m_tang;

        // 8 Frustum vertices
        glm::vec3 ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr;
        float nw,nh,fw,fh;

        RenderableObject*                       m_pointVBO;
        RenderableObject*                       m_lineVBO;
        RenderableObject*                       m_planeVBO;
        RenderableObject*                       m_normalVBO;
        vector<RenderableObject::Vertex>        m_vertices;
        vector<GLuint>                          m_lineIdxs;
        vector<GLuint>                          m_planeIdxs;
};

#endif /* end of include guard: FRUSTUM_H_ */
