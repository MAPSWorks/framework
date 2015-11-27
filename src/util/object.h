#ifndef OBJECT_H
#define OBJECT_H

#include "headers.h"

class Shader;
class RenderableObject;

class Object
{
public:
    Object(const QString &fileName, 
           const glm::vec3 &pos = glm::vec3(0), 
           const glm::vec3 &scale = glm::vec3(1, 1, 1), 
           const glm::vec4 &rot = glm::vec4(), 
           const glm::vec4 &color = glm::vec4(1, 1, 1, 1));
    ~Object();

    void render(Shader* shader);

    bool             m_isSelected;
    glm::vec3        m_min;
    glm::vec3        m_max;
    glm::vec3        m_position;
    glm::vec4        m_rotation;
    glm::vec3        m_scale;
    glm::vec4        m_color;
    glm::vec4        m_center;
    glm::vec4        m_up;
    glm::vec4        m_right;
    glm::vec4        m_front;

private:
    void init();
    void buildVBOs(const QString &fileName, const glm::vec3 &rot, const glm::vec3 &scale);

public:
    QString                             m_fileName;
    
    vector<RenderableObject *>          m_vbosTriangles;
    vector<RenderableObject *>          m_vbosLines;

    int                                 m_nrTriangles;
    int                                 m_nrVertices;
};

#endif
