#ifndef MESH_H
#define MESH_H

#include "headers.h"

class RenderableObject;

class Mesh
{
public:
   Mesh();
   ~Mesh();

   static vector<RenderableObject *> obj(const QString &fileName, const glm::vec3 &rot = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3 &scale = glm::vec3(1.0f, 1.0f, 1.0f), GLenum primitive = GL_TRIANGLES);
   static vector<RenderableObject*> obj(QString fileName);
   static RenderableObject *quadLines(int startX, int startY, int width, int height, const glm::vec4 &color = glm::vec4());
   static RenderableObject *quad(int startX, int startY, int width, int height, const glm::vec4 &color = glm::vec4(), GLenum primitive = GL_TRIANGLES);
   static RenderableObject *quad(int width, int height, const glm::vec4 &color = glm::vec4(), GLenum primitive = GL_TRIANGLES);
   static RenderableObject *sphere(float radius, int iterations, const glm::vec4 &color = glm::vec4(), GLenum primitive = GL_TRIANGLES);
   static RenderableObject *box(const glm::vec3 &mi, const glm::vec3 &ma, const glm::vec4 &color = glm::vec4(), GLenum primitive = GL_TRIANGLES);

private:
    typedef struct 
    {
       double x,y,z;
    } XYZ;
    
    typedef struct 
    {
       glm::vec3 p1,p2,p3;
    } FACET3;

    //Paul Bourke Sphere http://paulbourke.net/miscellaneous/sphere_cylinder/
    static int createNSphere(FACET3 *f, int iterations);
    static int CreateUnitSphere(FACET3 *facets, int iterations);
    static glm::vec3 MidPoint(glm::vec3 p1, glm::vec3 p2);

};

#endif

