/*=====================================================================================
                                scene_widget.h
    Description:  The main widget for displaying the OpenGL Context.
        
    Created by Chen Chen on 09/28/2015
=====================================================================================*/

#ifndef SCENE_WIDGET_H_
#define SCENE_WIDGET_H_

#include "headers.h"

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

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

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
    QTimer               *m_timer;
    QPoint               m_mouse;
    
    bool                 m_leftButton;
    bool                 m_rightButton;
	bool                 m_ctrlPressed;
	bool                 m_altPressed;
	bool                 m_shiftPressed;
    bool                 m_noOtherKey;
    bool                 m_renderOffline;

    int                  m_width;
    int                  m_height;
    int                  m_frameNr;

    // Camera transform
    Transform            m_trans;

    // m_lastFrameTime is used for counting time for animation
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastFrameTime;

    Scene                *m_scene;
    CameraManager        *m_cameraManager;
    Renderer             *m_renderer;

    unique_ptr<QMenu>    m_doubleClickMenu;
    bool                 m_openingFile; // Stop updating drawing while opening files

};

#endif //SCENE_WIDGET_H_
