#include "camera_manager.h"
#include "camera.h"
#include "shader.h"

CameraManager::CameraManager()
    : m_conf(PERSPECTIVE),
      m_width(1.0f),
      m_height(1.0f),
      m_aspect(1.0f),
      m_fov(45.0f),
      m_ncp(1.0f),
      m_fcp(1000.0f),
      m_rotHeight(-1.0f),
      m_rotate(10.0f, 40.0f, 0.0f),
      m_translate(0.0f, 0.0f, 0.0f),
      m_zoom(-20.0f),
      m_mouseSensivity(0.2f),
      m_camSpeed(10.0f),
      m_useCam(false),
      m_active(0)
{
    Camera* cam0 = new Camera(glm::vec3(0.0f, 4.0f, 8.0f),
                              0.0f,
                              -10.0f,
                              0.0f,
                              m_fov,
                              m_ncp,
                              m_fcp);
    cam0->setSpeed(m_camSpeed);
    cam0->update();
    cam0->loadFrameDirectory("../data/Camera");
    m_cameras.push_back(cam0);
}

CameraManager::~CameraManager(){
    for (size_t i = 0; i < m_cameras.size(); ++i) { 
        Camera* cam = m_cameras[i];
        delete cam;
    } 
}

void CameraManager::update(){
    m_cameras[m_active]->determineMovement();
    m_cameras[m_active]->update();
}

void CameraManager::resize(float width, float height){
    m_aspect = width / height;

    for (size_t i = 0; i < m_cameras.size(); ++i) { 
        m_cameras[i]->updateCamAspectRatio(m_aspect);
        m_cameras[i]->update();
    } 
}

void CameraManager::currentMatrices(Transform& trans){
    if (m_useCam) { 
        m_cameras[m_active]->determineMovement();
        m_cameras[m_active]->update();

        // Update trans
        m_cameras[m_active]->getMatrices(trans);
    } 
    else{
        if (m_conf == PERSPECTIVE) { 
            glm::mat4 projection = glm::perspective(m_fov, m_aspect, m_ncp, m_fcp);    
            glm::mat4 view(1.0f);
            view = glm::translate(view, glm::vec3(0.0f, m_rotHeight, 0.0f));
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, m_zoom));
            view = glm::rotate(view, glm::radians(m_rotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
            view = glm::rotate(view, glm::radians(m_rotate.y), glm::vec3(0.0f, 1.0f, 0.0f));

            // Update trans
            trans.projection = projection;
            trans.view = view;
            trans.viewProjection = projection * view;
        } 
        else{
        }
    }
}

void CameraManager::changeRotHeight(float delta) {
	m_rotHeight += delta;
}

// Keyboard / Mouse Response
void CameraManager::onMouseMove(float dx, float dy, int button){
    if (button == 0) { 
        if (m_useCam) { 
            m_cameras[m_active]->changeHeading(m_mouseSensivity * dx);
            m_cameras[m_active]->changePitch(m_mouseSensivity * dy);
        } 
        else{
            m_rotate.y -= (0.1 * dx);
            m_rotate.x -= (0.1 * dy);
        }
    } 
    else{
        m_cameras[m_active]->changeHeading(m_mouseSensivity * dx);
        m_cameras[m_active]->changePitch(m_mouseSensivity * dy);
    }
}

void CameraManager::onMouseWheel(int dir,
                                 double tx,
                                 double ty){
    float delta = m_zoom * -0.01 * abs(dir);

    if (dir > 0) { 
        m_zoom += delta;
    }
    else{
        m_zoom -= delta;
    }
}

void CameraManager::onKeyPress(int keyId){
    switch(keyId)
	{
        case Qt::Key_W:
			m_cameras[m_active]->moveForward(true);
            break;
        case Qt::Key_S:
			m_cameras[m_active]->moveBackward(true);
            break;
        case Qt::Key_A:
			m_cameras[m_active]->strafeLeft(true);
            break;
        case Qt::Key_D:
			m_cameras[m_active]->strafeRight(true);
            break;
	}
}

void CameraManager::onKeyRelease(int keyId) {
	switch(keyId)
	{
		case Qt::Key_W:	            
			m_cameras[m_active]->moveForward(false);
            break;
        case Qt::Key_S:
			m_cameras[m_active]->moveBackward(false);
            break;
        case Qt::Key_A:
			m_cameras[m_active]->strafeLeft(false);
            break;
        case Qt::Key_D:
			m_cameras[m_active]->strafeRight(false);
            break;
	}
}

void CameraManager::toggleCam(){
    if (!m_useCam) { 
        m_useCam = true; 
    } 
    else{
        m_active++;
        if(m_active > (int)m_cameras.size() - 1){
            m_active = 0;
            m_useCam = false;
        }
    }
}

void CameraManager::renderCameras(Shader* shader){
    if(!m_useCam) { 
        for(size_t i = 0; i < m_cameras.size(); ++i) { 
            m_cameras[i]->update(); 
            m_cameras[i]->render(shader);
        }  
    } 
}

void CameraManager::increaseSpeed(){
    m_camSpeed *= 2;

    for (size_t i = 0; i < m_cameras.size(); ++i) { 
        m_cameras[i]->setDistPerSec(m_camSpeed);
    } 
}

void CameraManager::decreaseSpeed(){
    m_camSpeed /= 2;

    for(size_t i = 0; i < m_cameras.size(); ++i) { 
        m_cameras[i]->setDistPerSec(m_camSpeed);
    }
}

Camera* CameraManager::lodCamera(){
    return m_cameras[0];
}

glm::vec3 CameraManager::lodCamPos(){
    return m_cameras[0]->position();
}

Camera* CameraManager::currentCam(){
    return m_cameras[m_active];
}

glm::vec3 CameraManager::currentCamPos(){
    if(m_useCam) { 
        return lodCamPos(); 
    } 
    else{
        glm::mat4 view(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, m_rotHeight, 0.0f));
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, m_zoom));
        view = glm::rotate(view, glm::radians(m_rotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::rotate(view, glm::radians(m_rotate.y), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::vec4 camPos = glm::inverse(view) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        return glm::vec3(camPos.x, camPos.y, camPos.z);
    }
}

float CameraManager::currentCamFov()
{
	if(m_useCam)
	{
        return m_cameras[m_active]->fov();
    }
    else
    {
        return m_fov;
    }
}

float CameraManager::currentCamNcp()
{
    if(m_useCam)
	{
        return m_cameras[m_active]->ncp();
    }
    else
    {
        return m_ncp;
    }
}

float CameraManager::currentCamFcp()
{
    if(m_useCam)
	{
        return m_cameras[m_active]->fcp();
    }
    else
    {
        return m_fcp;
    }
    
}

void CameraManager::currentCamToParams(){
    if(m_useCam) { 
        params::inst()->fov = m_cameras[m_active]->fov();     
        params::inst()->ncp = m_cameras[m_active]->ncp();     
        params::inst()->fcp = m_cameras[m_active]->fcp();     
        params::inst()->camPos = lodCamPos();     
    } 
    else{
        params::inst()->fov = m_fov;
        params::inst()->ncp = m_ncp;
        params::inst()->fcp = m_fcp;

        glm::mat4 view(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, m_rotHeight, 0.0f));
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, m_zoom));
        view = glm::rotate(view, glm::radians(m_rotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::rotate(view, glm::radians(m_rotate.y), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::vec4 camPos = glm::inverse(view) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        params::inst()->camPos = glm::vec3(camPos.x, camPos.y, camPos.z);
    }
}

void CameraManager::lockCurCamera(){
    if(m_useCam) { 
        m_cameras[m_active]->lock(); 
    } 
}

void CameraManager::toggleInterpolation(){

}

void CameraManager::addFrame(){
}

void CameraManager::clearFrameset(){

}

void CameraManager::saveFrameset(){

}

void CameraManager::toggleFrameset(){

}

QString CameraManager::currentFramesetName(){
    //return m_cameras[m_active]->frameSetName();
    return "hello";
}
