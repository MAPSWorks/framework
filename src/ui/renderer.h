#ifndef RENDERER_H
#define RENDERER_H

#include "headers.h"

class CameraManager;
class FrameBufferObjectMultisample;
class FrameBufferObject;
class Shader;
class Scene;
class VertexBufferObjectAttribs;
class Texture;

class Renderer
{
public:
    Renderer(Scene *scene, 
             CameraManager *camManager);

    ~Renderer();

    void init();
    void render(Transform &trans);
    
    void resize(int width, int height);

    void toggleBGColor();
    void togglePolygonMode();

private:
    void renderScene(const Transform &trans);

private:
	Scene *m_scene;
    CameraManager *m_cameraManager;

    int m_width;
    int m_height;

    glm::vec4 m_bgColor; 

    const GLuint m_samples;
    GLuint m_bgMode;
};

#endif
