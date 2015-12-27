/*=====================================================================================
                                scene_widget.h
    Description:  The main widget for displaying the OpenGL Context.
        
    Created by Chen Chen on 09/28/2015
=====================================================================================*/

#ifndef SCENE_WIDGET_H_
#define SCENE_WIDGET_H_

#include <QOpenGLFramebufferObject>
#include "headers.h"
#include "common.h"

using namespace std;

class Shader;
class Texture;
class Camera;
class Scene;
class Renderer;
class CameraManager;

/*
 *The main widget in the window
 */
class SceneWidget : public QOpenGLWidget
{
    Q_OBJECT
    
public:
    SceneWidget(QWidget *parent=0);
    ~SceneWidget();

public slots:
    void slotLoadTrajectories(void);
    void slotSaveTrajectories(void);
    void slotLoadOpenStreetMap(void);
    void slotLoadVideoTrajectory(void);
    void slotExtractTrajectories(void);
    void slotClear(void);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    // Save framebuffer
    void saveWindow();

    // Mouse event
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    // Keyboard event
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    
    // Initialize display related parameters. params is a singleton
    void initParams();    

private:
    unique_ptr<QTimer>          m_timer;
    QPoint                      m_mouse;

    bool                        m_renderOffline;
    unique_ptr<QOpenGLFramebufferObject>    m_fbo;
    
    bool                        m_leftButton;
    bool                        m_rightButton;
	bool                        m_ctrlPressed;
	bool                        m_altPressed;
	bool                        m_shiftPressed;
    bool                        m_noOtherKey;

    int                         m_width;
    int                         m_height;
    int                         m_frameNr;

    // Camera transform
    Transform                   m_trans;

    unique_ptr<Scene>           m_scene;
    unique_ptr<CameraManager>   m_cameraManager;
    unique_ptr<Renderer>        m_renderer;

    unique_ptr<QMenu>           m_doubleClickMenu;
    bool                        m_openingFile; // Stop updating drawing while opening files
};

#endif //SCENE_WIDGET_H_
