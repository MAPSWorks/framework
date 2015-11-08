#include "scene_widget.h"

#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "scene.h"
#include "renderer.h"
#include "camera_manager.h"

#include "main_window.h"

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
    m_lastFrameTime(std::chrono::high_resolution_clock::now()),
    m_doubleClickMenu(new QMenu),
    m_openingFile(false)
{
    // update drawing at least every 25ms
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer->start(25); 

    this->resize(this->width(), this->height());
    this->setFocus();
    this->setMouseTracking(true);

    // Double click menu
    m_doubleClickMenu->addAction("Load Trajectories", this, SLOT(slotLoadTrajectories()));

    m_doubleClickMenu->addSeparator();
    m_doubleClickMenu->addAction("Open OpenStreetMap", this, SLOT(slotLoadOpenStreetMap()));

    m_doubleClickMenu->addSeparator();
    m_doubleClickMenu->addAction("Clear", this, SLOT(slotClear()));
}

SceneWidget::~SceneWidget(){
    m_timer->stop();
    if(m_timer)
        delete m_timer;

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

/*=====================================================================================
        Public Slots
=====================================================================================*/

void SceneWidget::slotLoadTrajectories(){
    m_openingFile = true;
    QString filename = QFileDialog::getOpenFileName(&MainWindow::getInstance(),
                                                    "Load trajectories from file",
                                                    "./",
                                                    tr("Trajecotries (*.pbf)"));
    if(filename.isEmpty())
        return;
    
    if(m_scene->m_trajectories->load(filename.toStdString())){
        m_scene->updateDrawingData();
        update();
    }
    m_openingFile = false;
}

void SceneWidget::slotSaveTrajectories(){
    m_openingFile = true;
    MainWindow& main_window = MainWindow::getInstance();
    
    QString filename = QFileDialog::getSaveFileName(&main_window, "Save Trajectories",
                                                    main_window.getWorkspace().c_str(), "Trajectories (*.pbf)");
    if (filename.isEmpty())
        return;
    
    m_scene->m_trajectories->save(filename.toStdString());
    
    m_openingFile = false;
}

void SceneWidget::slotLoadOpenStreetMap(){
    m_openingFile = true;
    QString filename = QFileDialog::getOpenFileName(&MainWindow::getInstance(),
                                                    "Load OpenStreetMap from file",
                                                    "./",
                                                    tr("OpenStreetMap (*.osm)"));
    if(filename.isEmpty())
        return;
    
    if(m_scene->m_osmMap->load(filename.toStdString())){
        m_scene->updateDrawingData();
        update();
    }
    m_openingFile = false;
}

void SceneWidget::slotExtractTrajectories(){
    m_openingFile = true; // Turn off display updating

    MainWindow& main_window = MainWindow::getInstance();

    if (m_scene->m_osmMap->isEmpty()){
        QMessageBox msgBox;
        msgBox.setText("Please load an openstreetmap first.");
        msgBox.exec();
        return;
    }
    
    QStringList filenames = QFileDialog::getOpenFileNames(&main_window, 
                                            "Open Trajectories",
                                            main_window.getWorkspace().c_str(),    
                                            "Trajectories (*.pbf)");
    if (filenames.isEmpty())
        return;
    
    m_scene->m_trajectories->extractFromMultipleFiles(filenames,
                                                      m_scene->m_osmMap->m_boundBox);
    
    m_openingFile = false;
    update();
}

void SceneWidget::slotClear(){
    if(m_scene->m_trajectories) { 
        m_scene->m_trajectories->clear();
    } 
    if(m_scene->m_osmMap) { 
        m_scene->m_osmMap->clear();
    } 
    params::inst()->bboxUpdated = true;
}

/*=====================================================================================
        End of Public Slots
=====================================================================================*/

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
    // Update viewport
    const qreal retina_scale = devicePixelRatio();
    m_width = this->width() * retina_scale;
    m_height = this->height() * retina_scale;
    params::inst()->windowSize.x = m_width;
    params::inst()->windowSize.y = m_height;
    glViewport(0, 0, m_width, m_height);

    // Update scene in case of animation
    std::chrono::time_point<std::chrono::high_resolution_clock> cur_time = std::chrono::high_resolution_clock::now();
    float delta = std::chrono::duration_cast<std::chrono::milliseconds>(cur_time - m_lastFrameTime).count();
    m_lastFrameTime = cur_time;

    if(!m_openingFile) { 
        m_scene->update(delta);
        m_cameraManager->currentMatrices(m_trans);
    } 

    // This will render the scene
    m_renderer->render(m_trans);
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
        // For Orthogonal Camera Mode
        double tx = static_cast<double>(event->x()) / width() - 0.5f;
        double ty = -static_cast<double>(event->y()) / height() + 0.5f;

        m_cameraManager->onMouseWheel(event->pixelDelta().y(),
                                      tx,
                                      ty);
    }

    // Alt + wheel for moving the light
    //if(m_altPressed)
    //{
    //    glm::vec3 lPos = m_scene->m_light->position();
    //    float delta = lPos.y * 0.1;

    //    if (event->pixelDelta().y() > 0) {
    //        m_scene->m_light->setPosition(glm::vec3(lPos.x, lPos.y+delta, lPos.z));
    //        update();
    //    }
    //    else{
    //        m_scene->m_light->setPosition(glm::vec3(lPos.x, lPos.y-delta, lPos.z));
    //        update();
    //    } 
    //}

    event->accept();
}

void SceneWidget::mouseMoveEvent(QMouseEvent* event){
    QPoint newMouse(event->x(), event->y());
    QPoint diff = newMouse - m_mouse;

    float MAX_DIFF = 30.0f;
    float dx = clamp(diff.x(), -MAX_DIFF, MAX_DIFF);
    float dy = clamp(diff.y(), -MAX_DIFF, MAX_DIFF);

    // Left button for translation, right button for rotation
    if(!m_shiftPressed) { 
        if(m_leftButton)
        {            		
            m_cameraManager->onMouseMove(diff.x(), diff.y(), 0);
        }
        else if(m_rightButton)
        {
            m_cameraManager->onMouseMove(diff.x(), diff.y(), 1);
        }      
    } 
   
    if(m_leftButton && m_shiftPressed) { 
        m_scene->m_light->move(m_cameraManager, diff.x()*0.1, diff.y()*0.1); 
    }  

    m_mouse.setX(event->x());
    m_mouse.setY(event->y());
}

void SceneWidget::mousePressEvent(QMouseEvent* event){
    if(event->button() == Qt::LeftButton)
        m_leftButton = true; 

    if(event->button() == Qt::RightButton)
        m_rightButton = true;

    // Scene selection
    //if(m_ctrlPressed)
    //{
    //    m_scene->resetSelection();
    //    m_scene->select(m_trans, this->width(), this->height(), event->x(), event->y());
    //}
}

void SceneWidget::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton)
        m_leftButton = false;   

    if(event->button() == Qt::RightButton)
        m_rightButton = false;
}

void SceneWidget::mouseDoubleClickEvent(QMouseEvent* event){
    m_openingFile = true;
    m_doubleClickMenu->exec(event->globalPos());
    m_openingFile = false;
}

void SceneWidget::keyPressEvent(QKeyEvent* event){
    m_cameraManager->onKeyPress(event->key());

    switch (event->key()) 
    {
        case Qt::Key_F1:
            m_cameraManager->toggleCam();
            break;
        case Qt::Key_F5:
            m_cameraManager->toggleFrameset();
            break;
        case Qt::Key_F6:
            m_cameraManager->addFrame();
            break;
        case Qt::Key_F7:
            m_cameraManager->clearFrameset();
            break;
        case Qt::Key_F8:
            m_cameraManager->saveFrameset();
            break;

        case Qt::Key_I:
            m_cameraManager->toggleInterpolation();
            break;
        case Qt::Key_P:
            loop(params::inst()->gridRenderMode, 0, 3);
            break;
        case Qt::Key_U:
            params::inst()->applyShadow = !params::inst()->applyShadow;
            break;
        case Qt::Key_Space:
            loop(params::inst()->polygonMode, 0, 1, 1);
            break;
        //case Qt::Key_PageUp: 
        //    m_cameraManager->increaseSpeed();
        //    break;
        //case Qt::Key_PageDown:
        //    m_cameraManager->decreaseSpeed();
        //    break;
        //case Qt::Key_P: // Toggle Camera Mode
        //    m_cameraManager->toggleCameraMode();
        //    break;

        //case Qt::Key_Control:
        //    m_ctrlPressed = true;
        //    m_noOtherKey = false;
        //    break;
        //case Qt::Key_Alt:
        //    m_altPressed = true;
        //    m_noOtherKey = false;
        //    break;
        case Qt::Key_Shift:
            m_shiftPressed = true;
            break;
        default:
            break;
    }
}

void SceneWidget::keyReleaseEvent(QKeyEvent* event){
    m_cameraManager->onKeyRelease(event->key());

    switch (event->key()) 
    {
        //case Qt::Key_Control:
        //    m_ctrlPressed = false;
        //    m_noOtherKey = true;
        //    break;
        //case Qt::Key_Alt:
        //    m_altPressed = false;
        //    m_noOtherKey = true;
        //    break;
        case Qt::Key_Shift:
            m_shiftPressed = false;
            break;
        default:
            break;
    }
}
