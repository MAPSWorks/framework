#ifndef SCENE_H_
#define SCENE_H_

#include "headers.h"
#include "light.h"

class NiceGrid;
class Light;
class VertexBufferObjectAttribs;
class Shader;
class CameraManager;
class Object;
class TransformFeedback;

class Scene
{
public:
    Scene(CameraManager *camManager);
    ~Scene();

	void update(float delta);
    void initShaders();
    void init();

    void renderWorld(const Transform &trans);  
    void renderObjects(const Transform &trans);  
    void renderObjectsDepth(const Transform &trans);

    void select(const Transform &trans, int sw, int sh, int mx, int my);
    void move(const Transform &trans, int x, int y);
    void resetSelection();

public:
    Light                     *m_light;

private:

    // Shaders to draw various objects
    map<string, Shader*>       m_shaders;

    NiceGrid                  *m_niceGrid;
    CameraManager             *m_cameraManager;

    Shader                    *m_shaderNormal;
    Shader                    *m_shaderDepth;

    vector<Object*>           m_objects;

    int                       m_activeIdx;

	//TransformFeedback *m_tfb;
};


#endif /* end of include guard: SCENE_H_*/
