#include "primitive_shapes.h"

Line2D::Line2D(vector<glm::vec3>& points, vector<glm::vec4>& colors,
               float width) {
    if (points.size() != 2 || points.size() != colors.size()) {
        return;
    }
    glm::vec2 v1 = glm::vec2(points[0].x, points[0].z);
    glm::vec2 v2 = glm::vec2(points[1].x, points[1].z);
    float length = glm::length(v2 - v1);
    if (length < 1e-6) {
        return;
    }

    glm::vec2 dir = glm::normalize(v2 - v1);
    glm::vec2 perp(-dir[1], dir[0]);

    glm::vec2 vv1, vv2, vv3, vv4;
    vv1 = v1 + 0.5f * width * perp;
    vv2 = v1 - 0.5f * width * perp;
    vv3 = v2 - 0.5f * width * perp;
    vv4 = v2 + 0.5f * width * perp;

    RenderableObject::Vertex vp1, vp2, vp3, vp4;
    vp1.Position = glm::vec3(vv1.x, v1.y, vv1.y);
    vp1.Color = colors[0];
    vp2.Position = glm::vec3(vv2.x, v1.y, vv2.y);
    vp2.Color = colors[0];
    vp3.Position = glm::vec3(vv3.x, v2.y, vv3.y);
    vp3.Color = colors[1];
    vp4.Position = glm::vec3(vv4.x, v2.y, vv4.y);
    vp4.Color = colors[1];
    m_triangleData.push_back(vp1);
    m_triangleData.push_back(vp2);
    m_triangleData.push_back(vp3);
    m_triangleData.push_back(vp4);
}

Line2D::Line2D(vector<glm::vec3>& points, glm::vec4 color, float width) {
    if (points.size() != 2) {
        return;
    }
    glm::vec2 v1 = glm::vec2(points[0].x, points[0].z);
    glm::vec2 v2 = glm::vec2(points[1].x, points[1].z);
    float length = glm::length(v2 - v1);
    if (length < 1e-6) {
        return;
    }

    glm::vec2 dir = glm::normalize(v2 - v1);
    glm::vec2 perp(-dir[1], dir[0]);

    glm::vec2 vv1, vv2, vv3, vv4;
    vv1 = v1 + 0.5f * width * perp;
    vv2 = v1 - 0.5f * width * perp;
    vv3 = v2 - 0.5f * width * perp;
    vv4 = v2 + 0.5f * width * perp;

    RenderableObject::Vertex vp1, vp2, vp3, vp4;
    vp1.Position = glm::vec3(vv1.x, v1.y, vv1.y);
    vp1.Color = color;
    vp2.Position = glm::vec3(vv2.x, v1.y, vv2.y);
    vp2.Color = color;
    vp3.Position = glm::vec3(vv3.x, v2.y, vv3.y);
    vp3.Color = color;
    vp4.Position = glm::vec3(vv4.x, v2.y, vv4.y);
    vp4.Color = color;
    m_triangleData.push_back(vp1);
    m_triangleData.push_back(vp2);
    m_triangleData.push_back(vp3);
    m_triangleData.push_back(vp4);
}

void Line2D::appendTo(vector<RenderableObject::Vertex>& data,
        vector<GLuint>& indices) {
    size_t steart_idx = data.size();
    for(const auto& v : m_triangleData) { 
        data.push_back(v);
    } 
    indices.push_back(steart_idx);
    indices.push_back(steart_idx + 1);
    indices.push_back(steart_idx + 2);

    indices.push_back(steart_idx);
    indices.push_back(steart_idx + 2);
    indices.push_back(steart_idx + 3);
}
