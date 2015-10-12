#include "scene_widget.h"

#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "scene.h"
#include "renderer.h"
#include "camera_manager.h"

SceneWidget::SceneWidget(QWidget* parent)
:QOpenGLWidget(parent),
    m_leftButton(false),
    m_rightButton(false),
    m_ctrlPressed(false),
    m_altPressed(false),
    m_shiftPressed(false),
    m_noOtherKey(true),
    m_renderOffline(false),
    m_frameNr(0),
    m_oldTime(std::chrono::high_resolution_clock::now())
{
    this->resize(this->width(), this->height());
    this->setMouseTracking(true);
}

SceneWidget::~SceneWidget(){
    if(m_scene) { 
        delete m_scene; 
    } 
    if(m_cameraManager) { 
        delete m_cameraManager; 
    } 
    if(m_renderer){
        delete m_renderer;
    }

}

void SceneWidget::initializeGL(){
    initParams();

    m_cameraManager = new CameraManager();
    m_scene         = new Scene(m_cameraManager);
    m_renderer      = new Renderer(m_scene, m_cameraManager);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_SAMPLE_SHADING);
    glMinSampleShading(0.0f);
    
}

void SceneWidget::initParams(){
    params::inst()->lightPos            = glm::vec3(0.0f, 0.0f, 0.0f);
    params::inst()->lightDir            = glm::vec3(0.0f, 0.0f, 0.0f);
    params::inst()->camPos              = glm::vec3(0.0f, 0.0f, 0.0f);
    params::inst()->attenuation         = glm::vec3(0.5f, 0.0f, 0.0f);
    params::inst()->lightCone           = glm::vec2(0.0f, 0.0f);
    params::inst()->blur                = glm::vec2(4.0f, 2.0f);
    params::inst()->windowSize          = glm::vec2(m_width, m_height);

    params::inst()->clipPlaneGround     = glm::vec4(0.0f, -1.0f, 0.0f, 4.0f);

    params::inst()->gridRenderMode      = 0;
    params::inst()->polygonMode         = 0;

    params::inst()->applyShadow         = true;
    params::inst()->renderMesh          = false;
    params::inst()->renderObjects       = true;

    params::inst()->shadowIntensity     = 0.4f;
    params::inst()->depthRangeMax       = 1.0f;
    params::inst()->depthRangeMin       = 0.0f;
    params::inst()->polygonOffsetFactor = 0.0f;
    params::inst()->polygonOffsetUnits  = 0.0f;
    params::inst()->ncp                 = 0.0f;
    params::inst()->fcp                 = 0.0f;
    params::inst()->fov                 = 0.0f;

    params::inst()->shadowMapID         = 0;
    params::inst()->shadowMapBlurredID  = 0;

}

void SceneWidget::paintGL(){
    const qreal retina_scale = devicePixelRatio();
    m_width = this->width() * retina_scale;
    m_height = this->height() * retina_scale;
    params::inst()->windowSize.x = m_width;
    params::inst()->windowSize.y = m_height;

    glViewport(0, 0, m_width, m_height);

    m_renderer->render(m_trans);
    
    if (m_renderOffline)
    {
        saveFrameBuffer(this, m_frameNr);
        m_frameNr++;
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> cur_time = std::chrono::high_resolution_clock::now();
    float delta = std::chrono::duration_cast<std::chrono::milliseconds>(cur_time - m_oldTime).count();

    if(delta > 25)
    {
        m_scene->update(1.0f/delta);
        m_oldTime = cur_time;

        m_cameraManager->currentMatrices(m_trans);
        m_cameraManager->currentCamToParams();
    }

    this->update();
}

void SceneWidget::resizeGL(int width, int height){
    const int retina_scale = devicePixelRatio();
    m_width = width * retina_scale;
    m_height = height * retina_scale;

    m_renderer->resize(m_width, m_height);
    m_cameraManager->resize(m_width, m_height);
}

/*
 *Keyboard and Mouse
 */
void SceneWidget::wheelEvent(QWheelEvent* event){
    if(!m_altPressed && !m_ctrlPressed && !m_rightButton)
    {
        m_cameraManager->onMouseWheel(event->pixelDelta().y());
    }

    if(m_altPressed)
    {
        glm::vec3 lPos = m_scene->m_light->position();
        float delta = lPos.y * 0.1;

        if (event->pixelDelta().y() > 0) {
            m_scene->m_light->setPosition(glm::vec3(lPos.x, lPos.y+delta, lPos.z));
        }
        else{
            m_scene->m_light->setPosition(glm::vec3(lPos.x, lPos.y-delta, lPos.z));
        } 
    }

    if (m_rightButton) 
    {
        if (event->pixelDelta().y() > 0){

            m_cameraManager->changeRotHeight(-1.0f);
        }
        else{
            m_cameraManager->changeRotHeight(1.0f);
        }
    }

    event->accept();
}

void SceneWidget::mouseMoveEvent(QMouseEvent* event){
    QPoint newMouse(event->x(), event->y());
    QPoint diff = newMouse - m_mouse;

    float MAX_DIFF = 30.0f;
    float dx = clamp(diff.x(), -MAX_DIFF, MAX_DIFF);
    float dy = clamp(diff.y(), -MAX_DIFF, MAX_DIFF);

    if(!m_altPressed && !m_ctrlPressed)
    {
        if(m_leftButton)
        {            		
            m_cameraManager->onMouseMove(diff.x(), diff.y(), 0);
        }
        else if(m_rightButton)
        {
            m_cameraManager->onMouseMove(diff.x(), diff.y(), 1);
        }
    }

    //if(m_leftButton && m_altPressed)
    //{
    //    if(m_ctrlPressed)
    //        m_scene->m_light->recordPath(true);
    //    else
    //        m_scene->m_light->recordPath(false);

    //    m_scene->m_light->move(m_cameraManager, diff.x()*0.1, diff.y()*0.1);
    //}

    if(m_leftButton && m_ctrlPressed)
    {
        m_scene->move(m_trans, diff.x(), diff.y());
    }

    m_mouse.setX(event->x());
    m_mouse.setY(event->y());
}

void SceneWidget::mousePressEvent(QMouseEvent* event){
    if(event->button() == Qt::LeftButton)
        m_leftButton = true; 

    if(event->button() == Qt::RightButton)
        m_rightButton = true;

    if(m_ctrlPressed)
    {
        m_scene->resetSelection();
        m_scene->select(m_trans, this->width(), this->height(), event->x(), event->y());
    }
}

void SceneWidget::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton)
        m_leftButton = false;   

    if(event->button() == Qt::RightButton)
        m_rightButton = false;
}

void SceneWidget::keyPressEvent(QKeyEvent* event){
    //m_cameraManager->onKeyPress(event->key());

    switch (event->key()) 
    {
        case Qt::Key_PageUp: 
            m_cameraManager->increaseSpeed();
            break;
        case Qt::Key_PageDown:
            m_cameraManager->decreaseSpeed();
            break;
        case Qt::Key_Left:
            break;
        case Qt::Key_Right:
            break;
        case Qt::Key_Space:
            loop(params::inst()->polygonMode, 0, 1, 1);
            break;
        case Qt::Key_Plus:
            break;
        case Qt::Key_Minus:
            break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
            break;

        case Qt::Key_A:
            //Camera Strafe Left
            break;
        case Qt::Key_B:
            break;
        case Qt::Key_C:
            m_cameraManager->lockCurCamera();
            break;
        case Qt::Key_D:
            //Camera Strafe Right
            break;
        case Qt::Key_E:
            break;
        case Qt::Key_F:
            break;
        case Qt::Key_G:
            break;
        case Qt::Key_H:
            break;
        case Qt::Key_I:   
            m_cameraManager->toggleInterpolation();
            break;
        case Qt::Key_J:
            break;
        case Qt::Key_K:
            break;
        case Qt::Key_L:
            //m_scene->m_light->toggleMode();
            break;
        case Qt::Key_M:
            break;
        case Qt::Key_N:
            break;
        case Qt::Key_O:
            break;
        case Qt::Key_P:
            loop(params::inst()->gridRenderMode, 0, 3);
            break;
        case Qt::Key_Q:
            break;
        case Qt::Key_U:
            params::inst()->applyShadow = !params::inst()->applyShadow;
            break;
        case Qt::Key_R:
            break;
        case Qt::Key_S:
            //Camera Back
            break;
        case Qt::Key_T:
            break;
        case Qt::Key_V:
            break;
        case Qt::Key_W:
            //Camera Forward
            break;
        case Qt::Key_X:
            break;
        case Qt::Key_Y:
            break;
        case Qt::Key_Z:
            break;

        case Qt::Key_F1:
            this->setWindowState(this->windowState() ^ Qt::WindowFullScreen); 
            break;
        case Qt::Key_F2:
            m_cameraManager->toggleCam();
            break;        
        case Qt::Key_F3:            
            saveFrameBuffer(this);
            break;        
        case Qt::Key_F4:
            m_renderer->toggleBGColor();
            break;
        case Qt::Key_F5:
            //if(m_noOtherKey)
            //    m_gui->toggleMode();

            if(m_ctrlPressed)
                m_cameraManager->toggleFrameset();
            break;
        case Qt::Key_F6:			
            if(m_ctrlPressed)
                m_cameraManager->addFrame();
            break;
        case Qt::Key_F7:			
            if(m_ctrlPressed)
                m_cameraManager->clearFrameset();
            break;
        case Qt::Key_F8:			
            if(m_ctrlPressed)
                m_cameraManager->saveFrameset();
            break;
        case Qt::Key_F9:
            m_renderOffline = !m_renderOffline;
            m_frameNr = 0;
            break;
        case Qt::Key_F10:			
            break;
        case Qt::Key_F11:			
            break;
        case Qt::Key_F12:			
            break;

        case Qt::Key_Control:
            m_ctrlPressed = true;
            m_noOtherKey = false;
            break;
        case Qt::Key_Alt:
            m_altPressed = true;
            m_noOtherKey = false;
            break;
        case Qt::Key_Shift:
            m_shiftPressed = true;
            m_noOtherKey = false;
            break;
        case Qt::Key_Escape:            
            exit(0);
            break;
    }
}

void SceneWidget::keyReleaseEvent(QKeyEvent* event){
    m_cameraManager->onKeyRelease(event->key());

    switch (event->key()) 
    {
        case Qt::Key_Control:
            m_ctrlPressed = false;
            m_noOtherKey = true;
            break;
        case Qt::Key_Alt:
            m_altPressed = false;
            m_noOtherKey = true;
            break;
        case Qt::Key_Shift:
            m_shiftPressed = false;
            m_noOtherKey = true;
            break;
        default:
            break;
    }
}
