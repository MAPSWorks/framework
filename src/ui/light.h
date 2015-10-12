#ifndef LIGHT_H
#define LIGHT_H

#include "headers.h"
#include "spline.h"

class CameraManager;
class VertexBufferObjectAttribs;
class Shader;
class FrameBufferObject;
class Scene;
class Spline;

class Light
{
    public:
        struct Path
        {
            QString name;
            vector<glm::vec3> positions;
        };

        Light(Scene *scene, const glm::vec3 &pos);
        ~Light();

        void render(Shader* shader); // Draw the actual light as a sphere

        // Shadow mapping
        void blurShadowMap();
        void renderLightView(glm::mat4 &lightView, GLuint &shadowMapID, GLuint &shadowMapBlurredID);

        // Light info
        glm::vec3 position() const;
        void setPosition(const glm::vec3 &pos);

        void loadPaths();
        void savePaths();
        void recordPath(bool record);
        void move(CameraManager *camManager, float diffX, float diffY);
        void autoMove();
        void toggleMode();
        bool hasMoved();

        void update(float delta);

    private:
        Scene                     *m_scene;
        glm::vec3                 m_position;

        bool                      m_record;
        bool                      m_first;
        bool                      m_saved;
        int                       m_moveMode;
        float                     m_angle;  // circle path
        float                     m_radius; // circle path

        // Light paths and movements
        std::chrono::time_point<std::chrono::high_resolution_clock> m_oldTime;
        float                     m_time; 
        float                     m_distance;
        float                     m_movement;
        bool                      m_moved;

        vector<Path>              m_paths;
        Path                      m_curPath;
        Spline                    m_spline;

        glm::vec3                 m_oldPosition;

        VertexBufferObjectAttribs *m_vbo;       // Draw light as a sphere
        VertexBufferObjectAttribs *m_vboBlur;   // Quad for shadow mapping

    public:
        FrameBufferObject         *m_fboLight;

        int                       m_bufferWidth;
        int                       m_bufferHeight;
        float                     m_fcpLight;
        float                     m_ncpLight;
        float                     m_fovLight;

        glm::mat4                 m_lightView;
};

#endif

