/*=====================================================================================
                                  camera.h
    Description:  Camera class

    Created by Chen Chen on 09/28/2015
=====================================================================================*/

#ifndef CAMERA_H_
#define CAMERA_H_

#include "headers.h"

class Shader;
class Frustum;
class Spline;
class RenderableObject;

class Camera {
    public:
        enum Config{
            PERSPECTIVE = 0,
            ORTHOGONAL = 1
        };

        Camera(glm::vec3 pos, float heading, float pitch, float roll, float fov, float ncp, float fcp);
        virtual ~Camera();

        void update();
        void lock();
        void unlock();

        void changePitch(float degrees);
        void changeHeading(float degrees);
        void changeRoll(float degrees);

        // Access camera info
        void getMatrices(Transform &trans);
        glm::vec3 position() const;
        glm::vec3 direction() const;
        glm::vec3 up() const;
        glm::vec3 right() const;
        float pitch() const;
        float heading() const;
        float roll() const;
        float fov() const;
        float ncp() const;
        float fcp() const;

        // Set camera info
        void setColor(float r, float g, float b);
        void setPosition(glm::vec3 pos);
        void setHeading(float degrees);
        void setPitch(float degrees);
        void setRoll(float degrees);

        void setSpeed(float s);
        void increaseDistPerSec(float delta);
        void decreaseDistPerSec(float delta);
        void setDistPerSec(float value);

        void updateCamAspectRatio(float ratio);

        // Movement
        void moveForward(bool move);
        void moveBackward(bool move);
        void strafeLeft(bool move);
        void strafeRight(bool move);

        // Frames
        bool m_interpolate;
        size_t m_idxInterpolOld;
        size_t m_idxInterpolNew;
        void addFrame(glm::vec3 pos, float heading, float pitch, float desiredDistance); 
        void autoAddFrame();
        void clearFrames();
        void saveFrames(QString filePath);
        void saveFrames();
        void loadFrames(QString filePath);
        void loadFrameDirectory(QString folderPath);
        void changeFrameSet();
        QString frameSetName() const;

        void toggleInterpolation();
        void interpolate();
        void interpolate(float speed);
        void splineInterpolation();

        void determineMovement();

        // Show camera
        void render(unique_ptr<Shader>& shader);
        void renderFrames();
        void renderSpline();

    private:
        Config                    m_conf;
        float                     m_heading;
        float                     m_pitch;
        float                     m_roll;

        float                     m_ncp;
        float                     m_fcp;
        float                     m_fov;
        float                     m_aspect;
        float                     m_lerpFactor;

        bool                      m_locked;

        glm::mat4                 m_view;
        glm::mat4                 m_projection;

        glm::vec3                 m_pos;
        glm::vec3                 m_dir;
        glm::vec3                 m_up;
        glm::vec3                 m_right;
        glm::vec3                 m_strafe;
        
        glm::vec3                 m_cameraColor;

        // Camera speed
        HPTimer                   m_hpTimer;
        float                     m_forwardSpeed;
        float                     m_strafeSpeed;
        float                     m_moveSpeed;
        float                     m_time;
        float                     m_timer;
        float                     m_timerDiff;
        float                     m_secsPerFrame;
        float                     m_desiredDistance;
        float                     m_movementValue;

        // Camera spline
        Spline                    *m_spline;
        Spline                    *m_splineView;
        Spline                    *m_splineSpeed;
        RenderableObject          *m_splineVBO;

        // Quaternion helper
        glm::quat                 m_qHeading;
        glm::quat                 m_qPitch;
        glm::quat                 m_qRoll;

        // Frames
        struct CameraFrame{
            glm::vec3             pos;
            float                 heading;    // in degrees
            float                 pitch;
            float                 desiredDistance;
        };
        vector<CameraFrame>       m_camFrames;

        QFileInfoList             m_frameFileNames;
        int                       m_activeFrameSet;
        QString                   m_activeFrameSetName;
        QString                   m_frameSetFolder;
        RenderableObject          *m_frameVBO;

        // Visualize camera
        bool                      m_renderFrustum;
        unique_ptr<Frustum>       m_frustum;
};

#endif /* end of include guard: CAMERA_H_ */
