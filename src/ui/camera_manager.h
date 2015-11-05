/*=====================================================================================
                  camera_manager.h

Description:  Camera Manager Class
    - Two modes:
        1) Perspective: zoom by moving in the z-direction;
        2) Orthogonal: zoom by changing the projection matrix.

Created by Chen Chen on 09/28/2015
=====================================================================================*/

#ifndef CAMERA_MANAGER_H_
#define CAMERA_MANAGER_H_

#include "headers.h"

class Camera;
class Shader;

class CameraManager {
    public:
        CameraManager();
        ~CameraManager();

        void update();
        void resize(float width, float height);

        // Toggle Camera Mode
        void toggleCameraMode();

        // Camera Information
        void currentMatrices(Transform &trans);

        // Default view when not using camera
        void changeRotHeight(float delta);

        // Keyboard / Mouse response
        void onMouseMove(float dx, float dy, int button);
        void onMouseWheel(int dir,
                          double tx = 0.0f,
                          double ty = 0.0f); // tx and ty are only for ORTHO camera
        void onKeyPress(int keyId);
        void onKeyRelease(int keyId);

        // Cameras
        vector<Camera*> cameras();
        void toggleCam();
        void renderCameras(Shader* shader);

        void      increaseSpeed();      // Increase cameras' moving speed
        void      decreaseSpeed();      // Decrease cameras' moving speed
        Camera    *lodCamera();         // m_camera[0]
        glm::vec3 lodCamPos();          // m_camera[0]'s position
        Camera    *currentCam();        // current active camera
        glm::vec3 currentCamPos();      // current camera's position
        float     currentCamFov();
        float     currentCamNcp();
        float     currentCamFcp();
        void      lockCurCamera();

        void      toggleInterpolation();
        void      addFrame();
        void      clearFrameset();
        void      saveFrameset();
        void      toggleFrameset();
        QString   currentFramesetName();

    private:
        enum Config{
            PERSPECTIVE = 0,
            ORTHOGONAL = 1
        };

        // Compute current projection, view matrices, and store them in m_projection
        // and m_view
        void computeViewProjection();

        // Update global params Params' fov, ncp, fcp, camPos using current 
        // camera's info
        void  currentCamToParams(); 

        // CameraManager parameters
        Config                m_conf;
        float                 m_width;
        float                 m_height;
        float                 m_aspect;
        float                 m_fov;
        float                 m_ncp;
        float                 m_fcp;
        float                 m_rotHeight;
        glm::vec3             m_rotate; // x,y,z rotation angles
        glm::vec3             m_translate;
        float                 m_zoom;

        // Helper matrices, only used internally when m_useCam = false
        glm::mat4             m_projection;
        glm::mat4             m_view;

        // Ortho Zooming Helper to zoom in mouse wheel pos
        double                m_orthoZoomTransX;
        double                m_orthoZoomTransY;
        double                m_orthoZoomMax;

        float                 m_mouseSensitivity;
        float                 m_camSpeed;

        // Cameras
        bool                  m_useCam;
        int                   m_active;
        vector<Camera *>      m_cameras;
        
};

#endif /* end of include guard: CAMERA_MANAGER_H_ */
