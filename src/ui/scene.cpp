#include "scene.h"
#include "nice_grid.h"
#include "shader.h"
#include "vertexbuffer_object_attribs.h"
#include "mesh.h"
#include "camera_manager.h"
#include "object.h"

Scene::Scene(CameraManager *camManager)
: m_cameraManager(camManager),
  m_activeIdx(-1)
{
    init();   
}

Scene::~Scene()
{
    // Delete Shaders
    for(auto& m : m_shaders) { 
        delete m.second;
    } 

    // Delete Objects
    if(m_light) { 
        delete m_light;
    } 
    if(m_niceGrid) { 
        delete m_niceGrid;
    } 

    for (size_t i = 0; i < m_objects.size(); ++i) { 
        delete m_objects[i];
    } 

    if(m_trajectories) { 
        delete m_trajectories; 
    } 

    if(m_osmMap) { 
        delete m_osmMap; 
    } 
}

void Scene::initShaders(){
    m_shaders["trajectory"]      = new Shader("../shader/Trajectory.vert.glsl", 
                                           "../shader/Trajectory.frag.glsl");

    m_shaders["default"]      = new Shader("../shader/Default.vert.glsl", 
                                           "../shader/Default.frag.glsl");

    m_shaders["defaultDepth"] = new Shader("../shader/DefaultDepth.vert.glsl", 
                                           "../shader/DefaultDepth.frag.glsl");

    m_shaders["defaultLight"] = new Shader("../shader/DefaultLight.vert.glsl", 
                                           "../shader/DefaultLight.frag.glsl");

    m_shaders["niceGrid"]     = new Shader("../shader/NiceGrid.vert.glsl", 
                                           "../shader/NiceGrid.frag.glsl");

    m_shaders["object"]       = new Shader("../shader/Object.vert.glsl", 
                                           "../shader/Object.frag.glsl");

}

void Scene::init()
{
    initShaders(); 

    m_light = new Light(this, glm::vec3(10.0f, 30.0f, 10.0f));
    params::inst()->lightPos = m_light->position();
	m_niceGrid = new NiceGrid(100.0f, 40.0f);  

    Object *sphere = new Object("../data/Objs/sphere.obj", 
                                glm::vec3(0.0f, 1.0f, 0.0f), 
                                glm::vec3(1.0f, 1.0f, 1.0f), 
                                glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), 
                                glm::vec4(0.2f, 0.596f, 0.859f, 1.0f));
    m_objects.push_back(sphere);

    m_trajectories = new Trajectories();
    m_osmMap       = new OpenStreetMap();
}

void Scene::renderWorld(const Transform &trans)
{
    m_shaders["niceGrid"]->bind();
        m_shaders["niceGrid"]->setMatrix("matView", trans.view);
        m_shaders["niceGrid"]->setMatrix("matProjection", trans.projection);
        m_shaders["niceGrid"]->setMatrix("matLightView", trans.lightView);
        m_shaders["niceGrid"]->setMatrix("matViewProjection", trans.viewProjection);
        m_niceGrid->render(m_shaders["niceGrid"]); 
        
    m_shaders["default"]->bind();

        m_shaders["default"]->setMatrix("matView", trans.view);
        m_shaders["default"]->setMatrix("matProjection", trans.projection);

        m_light->render(m_shaders["default"]);
        m_cameraManager->renderCameras(m_shaders["default"]);
        m_trajectories->render(m_shaders["default"]);

    m_shaders["default"]->release();
}

void Scene::renderObjects(const Transform &trans)
{
    // Render Trajectories
    //m_shaders["trajectory"]->bind();
    //    m_shaders["trajectory"]->setMatrix("matView", trans.view);
    //    m_shaders["trajectory"]->setMatrix("matProjection", trans.projection);

    //    m_trajectories->render(m_shaders["trajectory"]);
 
    //m_shaders["trajectory"]->release();

    // Render OpenStreetMap
    //m_shaders["default"]->bind();
    //    m_shaders["default"]->setMatrix("matView", trans.view);
    //    m_shaders["default"]->setMatrix("matProjection", trans.projection);

    //    m_osmMap->render(m_shaders["default"]);
    //m_shaders["default"]->release();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));

    m_shaders["object"]->bind();
        m_shaders["object"]->setMatrix("matModel", model);
        m_shaders["object"]->setMatrix("matView", trans.view);
        m_shaders["object"]->setMatrix("matProjection", trans.projection);
        m_shaders["object"]->setMatrix("matLightView", trans.lightView);
        m_shaders["object"]->set3fv("lightPos", glm::value_ptr(params::inst()->lightPos));
        m_shaders["object"]->set3fv("lightDir", glm::value_ptr(params::inst()->lightDir));

        if (params::inst()->renderObjects)
        {
            for(uint i=0; i<m_objects.size(); ++i)
            {
                m_objects[i]->render(m_shaders["object"]);
            }
        }

    m_shaders["object"]->release();

}

void Scene::renderObjectsDepth(const Transform &trans)
{
    m_shaders["defaultDepth"]->bind();
        m_shaders["defaultDepth"]->setMatrix("matView", trans.view);
        m_shaders["defaultDepth"]->setMatrix("matProjection", trans.projection);
        m_shaders["defaultDepth"]->setMatrix("matLightView", trans.lightView);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
        m_shaders["defaultDepth"]->setMatrix("matModel", model);

        if (params::inst()->renderObjects)
        {
            for(uint i=0; i<m_objects.size(); ++i)
            {
               m_objects[i]->render(m_shaders["defaultDepth"]);
            }
        }

    m_shaders["defaultDepth"]->release();
}
 
void Scene::update(float delta)
{
    //m_light->update(delta);
    m_trajectories->update(delta);
}

void Scene::updateDrawingData(){
    m_trajectories->prepareForVisualization();
    m_osmMap->prepareForVisualization();
}

void Scene::select(const Transform &trans, int sw, int sh, int mx, int my)
{
    //float minDist = math_maxfloat;
    //int idx = -1;
    
    //Picking pick;

    //for(int i=0; i<m_objects.size(); ++i)
    //{
    //    Object *o = m_objects[i];
        
    //    glm::vec3 mi = o->m_min;
    //    glm::vec3 ma = o->m_max;

    //    glm::mat4 model(1.0);
    //    model = glm::scale(model, o->m_scale);
    //    model = glm::rotate(model, glm::radians(o->m_rotation.y), glm::vec3(0, 1, 0));
    //    model = glm::translate(model, o->m_position);
        
    //    float t = pick.select(trans, model, mi, ma, sw, sh, mx, my);

    //    if( t > 0.0f && t<minDist)
    //    {
    //        minDist = t;
    //        idx = i;
    //    }
    //}

    //if(idx >= 0)
    //{
    //    m_activeIdx = idx;
    //    m_objects[m_activeIdx]->m_isSelected = true;
    //}
}

void Scene::move(const Transform &trans, int x, int y)
{
	glm::vec3 dir;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 pos;

    getCameraFrame(trans, dir, up, right, pos);

	glm::vec3 movZ;
	glm::vec3 movX;
	glm::vec3 movY(0, 0, 0);
	
	glm::vec3 onPlane = glm::cross(right, glm::vec3(0, 1, 0));

    if(m_activeIdx >= 0)
    {
        glm::vec3 p = m_objects[m_activeIdx]->m_position;

        movY = glm::vec3(0, p.y, 0);
        float v = 0.1f * x;
        glm::vec3 dx = v * right;
        movX = glm::vec3(p.x, 0, 0) + dx;
        glm::vec3 dy = v * onPlane;
        movZ = glm::vec3(0, 0, p.z) + dy; 
        	
        glm::vec3 result = movX + movY + movZ ;            
        
        m_objects[m_activeIdx]->m_position = result;
    }
}

void Scene::resetSelection()
{
    for(int i=0; i<m_objects.size(); ++i)
    {
        m_objects[i]->m_isSelected = false;
    }

    m_activeIdx = -1;
}
