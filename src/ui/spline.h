/*=====================================================================================
  spline.h
Description:  Spline

Created by Chen Chen on 09/28/2015
=====================================================================================*/

#ifndef SPLINE_H_
#define SPLINE_H_

#include "headers.h"

class Shader;
class RenderableObject;

class Spline {
public:
    enum Config {
        CATMULL_ROM = 0,
        CUBIC,
        HERMITE,
        PICEWISE_HERMITE,
        COSINE,
        LINEAR,
        KOCHANEK_BARTEL,
        ROUNDED_CATMULL_ROM,
        BSPLINE
    };

    Spline(Config conf = CATMULL_ROM);
    ~Spline();

    void clear();

    void addPoint(const glm::vec3 &v);
    void bounds(int &p);

    glm::vec3 interpolatedPoint(float t, Config conf = CATMULL_ROM);
    glm::vec3 point(int n) const;
    int numPoints() const;
    void render(unique_ptr<Shader> &shader, bool showPoints = true,
                Config conf = CATMULL_ROM);

    static glm::vec3 linearInterpolation(const glm::vec3 &p0,
                                         const glm::vec3 &p1, float t);
    static glm::vec3 catmullRomInterpolation(const glm::vec3 &p0,
                                             const glm::vec3 &p1,
                                             const glm::vec3 &p2,
                                             const glm::vec3 &p3, float t);
    static glm::vec3 roundedCatmullRomInterpolation(const glm::vec3 &p0,
                                                    const glm::vec3 &p1,
                                                    const glm::vec3 &p2,
                                                    const glm::vec3 &p3,
                                                    float t);
    static glm::vec3 cubicInterpolation(const glm::vec3 &p0,
                                        const glm::vec3 &p1,
                                        const glm::vec3 &p2,
                                        const glm::vec3 &p3, float t);
    static glm::vec3 bSplineInterpolation(const glm::vec3 &p1,
                                          const glm::vec3 &p2,
                                          const glm::vec3 &p3,
                                          const glm::vec3 &p4, float t);
    static glm::vec3 hermiteInterpolation(
        const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2,
        const glm::vec3 &p3, float t, float tension = 0.0, float bias = 0.0);
    static glm::vec3 picewiseHermiteInterpolation(const glm::vec3 &a,
                                                  const glm::vec3 &b,
                                                  const glm::vec3 &startTangent,
                                                  const glm::vec3 &endTangent,
                                                  float t);
    static glm::vec3 kochanekBartelInterpolation(
        const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c,
        const glm::vec3 &d, float t, float tension = 0.0, float bias = 0.0,
        float continuity = 0.0f);

    vector<glm::vec3> m_points;
    glm::vec3 m_phantomStart;
    glm::vec3 m_phantomEnd;

private:
    Config m_config;

    float m_deltaT;  // = 1.0 / (m_points.size() - 1)
    float m_ptSize;

    unique_ptr<RenderableObject> m_vboPoints;
    unique_ptr<RenderableObject> m_vboLines;
};

#endif /* end of include guard: SPLINE_H_ */
