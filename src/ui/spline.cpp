#include "spline.h"
#include "color.h"
#include "shader.h"
#include "renderable_object.h"

Spline::Spline(Config conf)
    : m_config(conf),
      m_deltaT(0.0f),
      m_phantomStart(),
      m_phantomEnd(),
      m_vboLines(new RenderableObject()),
      m_vboPoints(new RenderableObject()),
      m_ptSize(8.0f) {}

Spline::~Spline() {}

void Spline::clear() { m_points.clear(); }

void Spline::addPoint(const glm::vec3 &v) {
    m_points.push_back(v);
    if (m_points.size() <= 1) {
        return;
    }

    m_deltaT = 1.0f / ((float)m_points.size() - 1);

    // Update m_phantomStart and m_phantomEnd
    if (m_points.size() >= 3) {
        glm::vec3 sStart = m_points[0];
        glm::vec3 tStart = m_points[1];

        glm::vec3 nStart = glm::vec3(sStart - tStart);
        float lenStart = glm::length(nStart);
        if (lenStart < 1e-5) {
            m_phantomStart = sStart;
        } else {
            nStart = glm::normalize(nStart);
            glm::vec3 tmp = nStart * lenStart * 0.5f;
            m_phantomStart = sStart + tmp;
        }

        unsigned int size = m_points.size() - 1;
        glm::vec3 sEnd = m_points[size];
        glm::vec3 tEnd = m_points[size - 1];

        glm::vec3 nEnd = glm::vec3(sEnd - tEnd);
        float lenEnd = glm::length(nEnd);
        if (lenEnd < 1e-5) {
            m_phantomEnd = sEnd;
        } else {
            nEnd = glm::normalize(nEnd);
            glm::vec3 tmp1 = nEnd * lenEnd * 0.5f;
            m_phantomEnd = sEnd + tmp1;
        }
    }
}

void Spline::bounds(int &p) {
    if (p < 0)
        p = 0;

    else if (p >= (int)m_points.size() - 1)
        p = m_points.size() - 1;
}

glm::vec3 Spline::interpolatedPoint(float t, Config conf) {
    // Find out in which interval we are on the spline
    int p = (int)(t / m_deltaT);

    int p0 = p - 1;
    int p1 = p;
    int p2 = p + 1;
    int p3 = p + 2;

    glm::vec3 c1;
    if (p0 < 0)
        c1 = m_phantomStart;
    else
        c1 = m_points[p0];

    glm::vec3 v1 = m_points[p1];
    glm::vec3 v2;
    if ((uint)p2 >= m_points.size())
        v2 = m_points[m_points.size() - 1];
    else
        v2 = m_points[p2];

    glm::vec3 c2;
    if ((uint)p3 >= m_points.size() - 1)
        c2 = m_phantomEnd;
    else
        c2 = m_points[p3];

    // Relative (local) time
    float lt = (t - m_deltaT * (float)p) / m_deltaT;

    if (conf == CATMULL_ROM)
        return Spline::catmullRomInterpolation(c1, v1, v2, c2, lt);
    if (conf == CUBIC) return Spline::bSplineInterpolation(c1, v1, v2, c2, lt);
    if (conf == BSPLINE) return Spline::cubicInterpolation(c1, v1, v2, c2, lt);
    if (conf == HERMITE)
        return Spline::hermiteInterpolation(c1, v1, v2, c2, lt);
    if (conf == KOCHANEK_BARTEL)
        return Spline::kochanekBartelInterpolation(c1, v1, v2, c2, lt);
    if (conf == ROUNDED_CATMULL_ROM)
        return Spline::roundedCatmullRomInterpolation(c1, v1, v2, c2, lt);

    // return LINEAR in default case
    return Spline::linearInterpolation(v1, v2, lt);
}

glm::vec3 Spline::point(int n) const { return m_points[n]; }

int Spline::numPoints() const { return m_points.size(); }

void Spline::render(unique_ptr<Shader> &shader, bool showPoints, Config conf) {
    if (m_points.size() <= 1) return;

    vector<RenderableObject::Vertex> pointData;
    vector<RenderableObject::Vertex> lineData;

    glm::vec4 pColor(1.0f, 1.0f, 0.0f, 0.5f);
    // Interpolated Points
    for (float f = 0; f <= 1.0f; f += 0.01f) {
        glm::vec3 v = interpolatedPoint(f, conf);
        RenderableObject::Vertex newPt;
        newPt.Position = v;
        newPt.Color = pColor;
        lineData.push_back(newPt);
    }

    // Base Points
    for (uint i = 0; i < m_points.size(); ++i) {
        glm::vec3 v = m_points[i];
        RenderableObject::Vertex newPt;
        newPt.Position = v;
        newPt.Color = pColor;
        pointData.push_back(newPt);
    }

    // Phantom Points
    std::vector<RenderableObject::Vertex> phantomPoints;
    float phantomPtSize = 3.0f;

    glm::vec3 p1 = m_phantomStart;
    glm::vec3 p2 = m_phantomEnd;

    RenderableObject::Vertex newPt1, newPt2;
    newPt1.Position = p1;
    newPt1.Color = glm::vec4(1.0f, 0.0f, 1.0f, 0.5f);
    newPt2.Position = p2;
    newPt2.Color = glm::vec4(0.0f, 1.0f, 1.0f, 0.5f);
    pointData.push_back(newPt1);
    pointData.push_back(newPt2);

    m_vboLines->setData(lineData, GL_LINE_STRIP);
    m_vboPoints->setData(pointData, GL_POINTS);

    if(showPoints) { 
        m_vboLines->render();
        glPointSize(m_ptSize);
        m_vboPoints->render();
        glPointSize(1.0f);      
    } 
}

glm::vec3 Spline::linearInterpolation(const glm::vec3 &p0, const glm::vec3 &p1,
                                      float t) {
    return p0 * t + (p1 * (1 - t));
}

glm::vec3 Spline::catmullRomInterpolation(const glm::vec3 &p0,
                                          const glm::vec3 &p1,
                                          const glm::vec3 &p2,
                                          const glm::vec3 &p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;

    float b1 = 0.5f * (-t3 + 2 * t2 - t);
    float b2 = 0.5f * (3 * t3 - 5 * t2 + 2);
    float b3 = 0.5f * (-3 * t3 + 4 * t2 + t);
    float b4 = 0.5f * (t3 - t2);

    return (p0 * b1 + p1 * b2 + p2 * b3 + p3 * b4);
}

// not working right
glm::vec3 Spline::roundedCatmullRomInterpolation(const glm::vec3 &p0,
                                                 const glm::vec3 &p1,
                                                 const glm::vec3 &p2,
                                                 const glm::vec3 &p3, float t) {
    glm::vec3 ab = p0 - p1;
    glm::vec3 cb = p2 - p3;

    ab = glm::normalize(ab);
    cb = glm::normalize(cb);
    glm::vec3 bVelo = cb - ab;
    bVelo = glm::normalize(bVelo);

    glm::vec3 dc = p3 - p2;
    glm::vec3 bc = -cb;

    dc = glm::normalize(dc);
    bc = glm::normalize(bc);

    glm::vec3 cVelo = dc - bc;
    cVelo = glm::normalize(cVelo);

    float cbDist = glm::length(cb);

    return Spline::catmullRomInterpolation(bVelo * cbDist, p1, p2,
                                           cVelo * cbDist, t);
}

glm::vec3 Spline::cubicInterpolation(const glm::vec3 &p0, const glm::vec3 &p1,
                                     const glm::vec3 &p2, const glm::vec3 &p3,
                                     float t) {
    float t2;
    glm::vec3 a0, a1, a2, a3;

    t2 = t * t;
    a0 = p3 - p2 - p0 + p1;
    a1 = p0 - p1 - a0;
    a2 = p2 - p0;
    a3 = p1;

    float tmp1 = t * t2;
    glm::vec3 res = a0 * tmp1;

    return (a0 * tmp1 + a1 * t2 + a2 * t + a3);
}

// not working right
glm::vec3 Spline::bSplineInterpolation(const glm::vec3 &p1, const glm::vec3 &p2,
                                       const glm::vec3 &p3, const glm::vec3 &p4,
                                       float t) {
    glm::vec4 a, b, c;

    a.x = (-p1.x + 3 * p2.x - 3 * p3.x + p4.x) / 6.0;
    a.y = (3 * p1.x - 6 * p2.x + 3 * p3.x) / 6.0;
    a.z = (-3 * p1.x + 3 * p3.x) / 6.0;
    a.w = (p1.x + 4 * p2.x + p3.x) / 6.0;

    b.x = (-p1.y + 3 * p2.y - 3 * p3.y + p4.y) / 6.0;
    b.y = (3 * p1.y - 6 * p2.y + 3 * p3.y) / 6.0;
    b.z = (-3 * p1.y + 3 * p3.y) / 6.0;
    b.w = (p1.y + 4 * p2.y + p3.y) / 6.0;

    c.x = (-p1.y + 3 * p2.y - 3 * p3.y + p4.y) / 6.0;
    c.y = (3 * p1.y - 6 * p2.y + 3 * p3.y) / 6.0;
    c.z = (-3 * p1.y + 3 * p3.y) / 6.0;
    c.w = (p1.y + 4 * p2.y + p3.y) / 6.0;

    glm::vec3 p = glm::vec3(((a.z + t * (a.y + t * a.x)) * t + a.w),
                            ((b.z + t * (b.y + t * b.x)) * t + b.w),
                            ((c.z + t * (c.y + t * c.x)) * t + c.w));

    return p;
}

glm::vec3 Spline::hermiteInterpolation(const glm::vec3 &p0, const glm::vec3 &p1,
                                       const glm::vec3 &p2, const glm::vec3 &p3,
                                       float t, float tension, float bias) {
    glm::vec3 m0, m1;
    float t2, t3;
    float a0, a1, a2, a3;

    t2 = t * t;
    t3 = t2 * t;

    m0 = (p1 - p0) * (1.0f + bias) * (1.0f - tension) / 2.0f;
    m0 += (p2 - p1) * (1.0f - bias) * (1.0f - tension) / 2.0f;
    m1 = (p2 - p1) * (1.0f + bias) * (1.0f - tension) / 2.0f;
    m1 += (p3 - p2) * (1.0f - bias) * (1.0f - tension) / 2.0f;

    a0 = 2 * t3 - 3 * t2 + 1;
    a1 = t3 - 2 * t2 + t;
    a2 = t3 - t2;
    a3 = -2 * t3 + 3 * t2;

    return (a0 * p1 + a1 * m0 + a2 * m1 + a3 * p2);
}

glm::vec3 Spline::picewiseHermiteInterpolation(const glm::vec3 &a,
                                               const glm::vec3 &b,
                                               const glm::vec3 &startTangent,
                                               const glm::vec3 &endTangent,
                                               float t) {
    float t2, t3;
    float a0, a1, b0, b1;

    t2 = t * t;
    t3 = t2 * t;

    a0 = (t3 * 2.0f) - (3.0f * t2) + 1.0f;
    a1 = (-2.0f * t3) + (3.0f * t2);
    b0 = t3 - (2.0f * t2) + t;
    b1 = t3 - t2;

    return (a0 * a + a1 * b + b0 * startTangent + b1 * endTangent);
}

// not working right
glm::vec3 Spline::kochanekBartelInterpolation(
    const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c,
    const glm::vec3 &d, float t, float tension, float bias, float continuity) {
    glm::vec3 ab = b - a;
    ab = glm::normalize(ab);
    glm::vec3 cd = d - c;
    cd = glm::normalize(cd);

    glm::vec3 inTangent =
        ((1.0f - tension) * (1.0f - continuity) * (1.0f + bias)) * 0.5f * ab +
        ((1.0f - tension) * (1.0f + continuity) * (1.0f - bias)) * 0.5f * cd;

    glm::vec3 outTangent =
        ((1.0f - tension) * (1.0f + continuity) * (1.0f + bias)) * 0.5f * ab +
        ((1.0f - tension) * (1.0f - continuity) * (1.0f - bias)) * 0.5f * cd;

    // return picewiseHermiteInterpolation(b, c, inTangent, outTangent, t);
    return hermiteInterpolation(inTangent, b, c, outTangent, t);
}
