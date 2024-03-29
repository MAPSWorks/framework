#ifndef SCENE_H_
#define SCENE_H_

#include "headers.h"
#include "light.h"
#include "trajectories.h"
#include "openstreetmap.h"
#include "video_trajectory.h"
#include "model.h"

class NiceGrid;
class RenderableObject;
class Shader;
class CameraManager;
class TransformFeedback;

class Scene
{
public:
    Scene(unique_ptr<CameraManager>& camManager);
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
    unique_ptr<Light>         m_light;
    unique_ptr<Trajectories>  m_trajectories;
    unique_ptr<OpenStreetMap> m_osmMap;
    unique_ptr<VideoTrajectory> m_videoTrajectory;

private:
    unique_ptr<CameraManager>&      m_cameraManager;

    unique_ptr<NiceGrid>            m_niceGrid;

    unique_ptr<Shader>              m_shaderNormal;
    unique_ptr<Shader>              m_shaderDepth;

    unique_ptr<Model>               m_model;

    int                             m_activeIdx;

    unique_ptr<RenderableObject>    m_vboTest;
	//TransformFeedback *m_tfb;
};


#endif /* end of include guard: SCENE_H_*/
