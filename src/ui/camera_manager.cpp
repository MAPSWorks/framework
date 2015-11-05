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
      m_rotate(20.0f, 0.0f, 0.0f),
      m_translate(0.0f, 0.0f, 0.0f),
      m_zoom(-50.0f),
      m_orthoZoomTransX(0.0f),
      m_orthoZoomTransY(0.0f),
      m_orthoZoomMax(100.0f),
      m_mouseSensitivity(0.1f),
      m_camSpeed(10.0f),
      m_useCam(false),
      m_active(0)
{
    Camera* cam0 = new Camera(glm::vec3(0.0f, 4.0f, 20.0f),
                              180.0f,
                              10.0f,
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

void CameraManager::toggleCameraMode(){
    if(m_conf == PERSPECTIVE) { 
        m_conf = ORTHOGONAL; 
    }  
    else{
        m_conf = PERSPECTIVE;
    }
}

void CameraManager::computeViewProjection(){
    if (m_conf == PERSPECTIVE) { 
        // Perspective Camera
        m_projection = glm::perspective(m_fov, m_aspect, m_ncp, m_fcp);    
        m_view = glm::mat4(1.0f);

        // Translation
        m_view = glm::translate(m_view, glm::vec3(0.0f, m_rotHeight, 0.0f));
        m_view = glm::translate(m_view, glm::vec3(0.0f, 0.0f, m_zoom));
        m_view = glm::translate(m_view, m_translate);

        // Rotation
        m_view = glm::rotate(m_view, glm::radians(m_rotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
        m_view = glm::rotate(m_view, glm::radians(m_rotate.y), glm::vec3(0.0f, 1.0f, 0.0f));
    } 
    else{
        // Orthogonal Camera
        float orthoWidth, orthoHeight;
        if(m_zoom < 1.0f) { 
           orthoWidth = m_width * (2.0f - m_zoom); 
           orthoHeight = m_height * (2.0f - m_zoom); 
        } 
        else{
           orthoWidth = m_width / m_zoom; 
           orthoHeight = m_height / m_zoom; 
        }

        float orthoLeft   = (m_orthoZoomTransX - orthoWidth/2.0f) * m_aspect;
        float orthoRight  = (m_orthoZoomTransX + orthoWidth/2.0f) * m_aspect;
        float orthoBottom = m_orthoZoomTransY - orthoHeight/2.0f;
        float orthoTop    = m_orthoZoomTransY + orthoHeight/2.0f;
        
        m_projection = glm::ortho(orthoLeft,
                                          orthoRight,
                                          orthoBottom,
                                          orthoTop,
                                          -m_fcp,
                                          m_fcp);

        m_view = glm::mat4(1.0f);
        
        m_view = glm::translate(m_view, glm::vec3(0.0f, m_rotHeight, 0.0f));
        m_view = glm::translate(m_view, glm::vec3(0.0f, 0.0f, m_zoom));
       
        m_view = glm::rotate(m_view, glm::radians(m_rotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
        m_view = glm::rotate(m_view, glm::radians(m_rotate.y), glm::vec3(0.0f, 1.0f, 0.0f)); 
    }
}

void CameraManager::currentMatrices(Transform& trans){
    if (m_useCam) { 
        m_cameras[m_active]->determineMovement();
        m_cameras[m_active]->update();
        m_cameras[m_active]->interpolate();

        // Update trans
        m_cameras[m_active]->getMatrices(trans);
    } 
    else{
        computeViewProjection(); 
        // Update trans
        trans.projection = m_projection;
        trans.view = m_view;
        trans.viewProjection = m_projection * m_view;
    }

    // Update global params' fov, ncp, fcp, camPos
    currentCamToParams();
}

void CameraManager::changeRotHeight(float delta) {
	m_rotHeight += delta;
}

// Keyboard / Mouse Response
// button = 0: left button
// button = 1: right button
void CameraManager::onMouseMove(float dx, float dy, int button){
    if (button == 0) { 
        if (m_useCam) { 
            m_cameras[m_active]->changeHeading(m_mouseSensitivity * dx);
            m_cameras[m_active]->changePitch(m_mouseSensitivity * dy);
        } 
        else{
            m_rotate.y -= (m_mouseSensitivity * dx);
            m_rotate.x -= (m_mouseSensitivity * dy);
        }
    } 
    else{
        if(m_useCam) { 
                
        } 
        else{
            m_translate.x += (m_mouseSensitivity * dx); 
            m_translate.y += (m_mouseSensitivity * dy); 
        }
    }
}

void CameraManager::onMouseWheel(int dir,
                                 double tx,
                                 double ty){
    float delta = abs(m_zoom) * 0.01 * abs(dir);
    if(delta < 0.1) { 
        delta = 0.1; 
    } 

    float prev_zoom = m_zoom;

    float max_zoom = 800.0f;
    if (dir > 0) { 
        m_zoom += delta;
        if(m_zoom > 1.0) { 
            m_zoom = 1.0; 
        } 
    }
    else{
        m_zoom -= delta;
        if(m_zoom < -max_zoom) { 
            m_zoom = -max_zoom; 
        } 
    }

    if(m_conf == ORTHOGONAL) { 
        float prevOrthoWidth, prevOrthoHeight;
        if(m_zoom < 1.0f) { 
           prevOrthoWidth = m_width * (2.0f - prev_zoom); 
           prevOrthoHeight = m_height * (2.0f - prev_zoom); 
        } 
        else{
           prevOrthoWidth = m_width / prev_zoom; 
           prevOrthoHeight = m_width / prev_zoom; 
        }
        double prex = tx * prevOrthoWidth;
        double prey = ty * prevOrthoHeight;

        float orthoWidth, orthoHeight;
        if(m_zoom < 1.0f) { 
           orthoWidth = m_width * (2.0f - m_zoom); 
           orthoHeight = m_height * (2.0f - m_zoom); 
        } 
        else{
           orthoWidth = m_width / m_zoom; 
           orthoHeight = m_height / m_zoom; 
        } 
        double postx = tx * orthoWidth;
        double posty = ty * orthoHeight;

        m_orthoZoomTransX += (prex - postx);
        m_orthoZoomTransY += (prey - posty);
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

vector<Camera*> CameraManager::cameras(){
    return m_cameras;
}

void CameraManager::toggleCam(){
    if (!m_useCam) { 
        m_useCam = true; 
        cout << "CameraManager: using camera." << endl;
    } 
    else{
        m_active++;
        if(m_active > (int)m_cameras.size() - 1){
            m_active = 0;
            m_useCam = false;
            cout << "CameraManager: NOT using camera." << endl;
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
        glm::vec4 camPos = glm::inverse(m_view) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
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

        glm::vec4 camPos = glm::inverse(m_view) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        params::inst()->camPos = glm::vec3(camPos.x, camPos.y, camPos.z);
    }
}

void CameraManager::lockCurCamera(){
    if(m_useCam) { 
        m_cameras[m_active]->lock(); 
    } 
}

void CameraManager::toggleInterpolation(){
    m_cameras[m_active]->toggleInterpolation();
}

void CameraManager::addFrame(){
    m_cameras[m_active]->autoAddFrame();
}

void CameraManager::clearFrameset(){
    m_cameras[m_active]->clearFrames();
}

void CameraManager::saveFrameset(){
    m_cameras[m_active]->saveFrames();
}

void CameraManager::toggleFrameset(){
    m_cameras[m_active]->changeFrameSet();
}

QString CameraManager::currentFramesetName(){
    return m_cameras[m_active]->frameSetName();
}
