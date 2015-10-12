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

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void initParams();    

private:
    QPoint m_mouse;
    
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

    Transform            m_trans;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_oldTime;

    Scene                *m_scene;
    CameraManager        *m_cameraManager;
    Renderer             *m_renderer;
    
};

#endif //SCENE_WIDGET_H_
