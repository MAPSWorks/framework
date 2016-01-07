/*=====================================================================================
                                primitive_shapes.h

    Description:  primitive shapes, such as line segments, line strips, quads,
etc. to help visualization.

    Created by Chen Chen on 01/07/2016
=====================================================================================*/

#ifndef PRIMITIVE_SHAPES_H_9CWLYH4U
#define PRIMITIVE_SHAPES_H_9CWLYH4U

#include "headers.h"
#include "shader.h"
#include "renderable_object.h"

// 2D line in X-Z plane
class Line2D {
public:
    Line2D(vector<glm::vec3>& points, vector<glm::vec4>& colors,
           float width = 1.0f);
    Line2D(vector<glm::vec3>& points, glm::vec4 color, float width = 1.0f);
    virtual ~Line2D() {}

    void appendTo(vector<RenderableObject::Vertex>& data,
                  vector<GLuint>& indices);

private:
    vector<RenderableObject::Vertex> m_triangleData;
};

// 2D line stripe in X-Z plane

#endif /* end of include guard: PRIMITIVE_SHAPES_H_9CWLYH4U */
