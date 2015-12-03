#include "camera_manager.h"
#include "camera.h"
#include "shader.h"

CameraManager::CameraManager()
    : m_conf(PERSPECTIVE),
      m_camSpeed(10.0f),
      m_useCam(false),
      m_active(0)
{
    m_aspect           = params::inst().windowSize.x / params::inst().windowSize.y;
    m_fov              = 45.0f;
    m_ncp              = 1.0f;
    m_fcp              = 10.0f * params::inst().scale;
    m_rotHeight        = -1.0f;
    m_rotate           = glm::vec3(20.0f, 0.0f, 0.0f);
    m_translate        = glm::vec3(0.0f, 0.0f, -1.5f * params::inst().scale);
    m_zoomTrans        = glm::vec3(0.0f);
    m_mouseSensitivity = 0.1f;

    m_camSpeed         = 1.0f;
    m_useCam           = false;
    m_active           = 0;
    unique_ptr<Camera> cam0(new Camera(glm::vec3(0.0f, 4.0f, 2.0f),
                                       180.0f,
                                       10.0f,
                                       0.0f,
                                       m_fov,
                                       m_ncp,
                                       m_fcp));
    cam0->setSpeed(m_camSpeed);
    cam0->update();
    cam0->loadFrameDirectory("../data/Camera");
    m_cameras.push_back(std::move(cam0));
}

CameraManager::~CameraManager(){
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
        cout << "Now using orthogonal camera." << endl;
    }  
    else{
        m_conf = PERSPECTIVE;
        cout << "Now using perspective camera." << endl;
    }
}

void CameraManager::computeViewProjection(){
    if (m_conf == PERSPECTIVE) { 
        // Perspective Camera
        m_projection = glm::perspective(glm::radians(m_fov), 
                                        m_aspect, 
                                        m_ncp, 
                                        m_fcp);    

        m_view = glm::mat4(1.0f);
            // Translation
        m_view = glm::translate(m_view, glm::vec3(0.0f, m_rotHeight, 0.0f));
        m_view = glm::translate(m_view, m_translate);
        m_view = glm::translate(m_view, m_zoomTrans);
            // Rotation
        m_view = glm::rotate(m_view, 
                             glm::radians(m_rotate.x), 
                             glm::vec3(1.0f, 0.0f, 0.0f));
        m_view = glm::rotate(m_view, 
                             glm::radians(m_rotate.y), 
                             glm::vec3(0.0f, 1.0f, 0.0f));
    } 
    else{
        // Orthogonal Camera
        float orthoWidth, orthoHeight;
        orthoWidth = abs(m_translate.z);
        orthoHeight = abs(m_translate.z);

        float orthoLeft   =  - orthoWidth / 2.0f * m_aspect;
        float orthoRight  = orthoWidth /2.0f * m_aspect;
        float orthoBottom = - orthoHeight/2.0f;
        float orthoTop    = orthoHeight/2.0f;
        
        m_projection = glm::ortho(orthoLeft,
                                  orthoRight,
                                  orthoBottom,
                                  orthoTop,
                                  -m_fcp,
                                  m_fcp);

        m_view = glm::mat4(1.0f);
        
        m_view = glm::translate(m_view, glm::vec3(0.0f, m_rotHeight, 0.0f));
        m_view = glm::translate(m_view, m_translate);
        m_view = glm::translate(m_view, m_zoomTrans);
       
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
        trans.matView = m_view;
        trans.matProjection = m_projection;
        trans.matViewProjection = m_projection * m_view;
    }
}

void CameraManager::changeRotHeight(float delta) {
	m_rotHeight += delta;
}

// Keyboard / Mouse Response
// button = 0: left button
// button = 1: right button
void CameraManager::onMouseMove(float dx, float dy, int button){
    if (button == 1) { 
        // Right button
        if(m_useCam) { 
                
        } 
        else{
            m_translate.x += (m_mouseSensitivity * dx); 
            m_translate.y -= (m_mouseSensitivity * dy); 
        } 
    } 
    else{
        // Left button
        if (m_useCam) { 
            m_cameras[m_active]->changeHeading(m_mouseSensitivity * dx);
            m_cameras[m_active]->changePitch(m_mouseSensitivity * dy);
        } 
        else{
            m_rotate.y -= (m_mouseSensitivity * dx);
            m_rotate.x -= (m_mouseSensitivity * dy);
        }
    }
}

void CameraManager::onMouseWheel(int yoffset,
                                 double tx,
                                 double ty){
    float delta = abs(m_translate.z) * 0.01 * abs(yoffset);
    if(delta < 0.01) { 
        delta = 0.01; 
    } 

    float old_trans_z = m_translate.z;
    float new_trans_z = m_translate.z;

    float max_zoom = 5.0f * params::inst().scale;
    if (yoffset > 0) { 
        new_trans_z += delta;
        if(new_trans_z > 1.0) { 
            new_trans_z = 1.0; 
        } 
    }
    else{
        new_trans_z -= delta;
        if(new_trans_z < -max_zoom) { 
            new_trans_z = -max_zoom; 
        } 
    }
    m_translate.z = new_trans_z;

    float delta_x = -(new_trans_z - old_trans_z) * tx * m_aspect;
    float delta_y = -(new_trans_z - old_trans_z) * ty;
    m_zoomTrans.x += delta_x;
    m_zoomTrans.y += delta_y;      
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

vector<unique_ptr<Camera>>& CameraManager::cameras(){
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

void CameraManager::renderCameras(unique_ptr<Shader>& shader){
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

unique_ptr<Camera>& CameraManager::lodCamera(){
    return m_cameras[0];
}

glm::vec3 CameraManager::lodCamPos(){
    return m_cameras[0]->position();
}

unique_ptr<Camera>& CameraManager::currentCam(){
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
