#include "renderer.h"
#include "camera_manager.h"
#include "shader.h"
#include "vertexbuffer_object_attribs.h"
#include "scene.h"
#include "mesh.h"
#include "light.h"
#include "texture.h"

Renderer::Renderer(Scene *scene, CameraManager *camManager)
: m_scene(scene),
  m_cameraManager(camManager),
  m_bgColor(0.1f, 0.1f, 0.1f, 1.0f),
  m_width(0),
  m_height(0),
  m_samples(16),
  m_bgMode(0)
{
    init();
}

Renderer::~Renderer()
{
}

void Renderer::init()
{
}

void Renderer::render(Transform &trans)
{
    if(params::inst()->applyShadow)
    {
        m_scene->m_light->renderLightView(trans.lightView, 
                                          params::inst()->shadowMapID, 
                                          params::inst()->shadowMapBlurredID); 
    }    

    renderScene(trans);
}

void Renderer::renderScene(const Transform &trans)
{
    glClearColor(m_bgColor.x, m_bgColor.y, m_bgColor.z, m_bgColor.w);    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);     

    if (params::inst()->polygonMode == 1)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    m_scene->renderObjects(trans);
    m_scene->renderWorld(trans);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::resize(int width, int height)
{
    m_width = width;
    m_height = height;    
}

void Renderer::toggleBGColor()
{
    m_bgMode ++;
    if(m_bgMode > 2)
        m_bgMode = 0;

    if(m_bgMode == 0)
	{
        m_bgColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	}
   
    if(m_bgMode == 1)
	{
        m_bgColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	}

    if(m_bgMode == 2)
	{
        m_bgColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

