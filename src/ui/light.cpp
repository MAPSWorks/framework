#include "light.h"
#include "camera_manager.h"
#include "shader.h"
#include "scene.h"
#include "framebuffer_object.h"

Light::Light(Scene *scene, const glm::vec3 &pos)
:   m_scene(scene), 
    m_position(pos),
    m_record(false),
    m_first(true),
    m_saved(true),
    m_moveMode(0),
    m_angle(0.0f),
    m_radius(20.0f),
    m_oldTime(std::chrono::high_resolution_clock::now()),
    m_time(0),
    m_distance(1.0f),
    m_movement(0.0f),
    m_moved(true),
    m_bufferWidth(2048),
    m_bufferHeight(2048)
{
    m_ncpLight = 1.0f;
    m_fcpLight = 5.0f * params::inst().scale;
    m_fovLight = 90.0f;

    loadPaths();

    // Light Objects
    m_vbo = RenderableObject::sphere(1.0f, 
                                     4, 
                                     glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    m_fboLight.reset(new FrameBufferObject(m_bufferWidth, m_bufferHeight));
    m_fboLight->createDepthTexture();
    params::inst().shadowInfo.shadowMapID = m_fboLight->texAttachment(GL_DEPTH_ATTACHMENT);
}

Light::~Light(){
}

void Light::renderLightView(Transform &trans){
    //glm::mat4 projection = glm::perspective(glm::radians(m_fovLight), 
    //                                        (float)m_bufferWidth/(float)m_bufferHeight, 
    //                                        1.0f, 
    //                                        2.5f * params::inst().scale);
    glm::mat4 projection = glm::ortho(-params::inst().scale, 
                                      params::inst().scale, 
                                      -params::inst().scale, 
                                      params::inst().scale, 
                                      1.0f, 
                                      5.0f * params::inst().scale);
    m_direction = glm::normalize(-m_position);
    glm::mat4 view = glm::lookAt(m_position, 
                                 m_position + m_direction, 
                                 glm::vec3(0.0f, 1.0f, 0.0f));

    Transform transLight;
    transLight.matView           = view;
    transLight.matProjection     = projection;
    transLight.matViewProjection = projection * view;
    m_lightSpace                 = transLight.matViewProjection;

    // Render depth map
    glViewport(0, 0, m_bufferWidth, m_bufferHeight);
    m_fboLight->bind();
        glClear(GL_DEPTH_BUFFER_BIT);     
        m_scene->renderObjectsDepth(transLight);        
    m_fboLight->release();
    glViewport(0, 0, params::inst().windowSize.x, params::inst().windowSize.y);

    // Update trans
    trans.matLightSpace = m_lightSpace;
}

void Light::render(unique_ptr<Shader>& shader)
{
    glm::mat4 model(1.0);
    model = glm::translate(model, m_position);
    shader->setMatrix("matModel", model); 
    m_vbo->render();
}

glm::vec3 Light::position() const
{
    return m_position;
}

void Light::setPosition(const glm::vec3 &pos)
{
    m_position = pos;
    m_moved = true;
}

void Light::autoMove()
{
    if(m_moveMode == 1)
    {
        m_position.x = cosf(m_angle) * m_radius;
        m_position.z = sinf(m_angle) * m_radius;
        m_angle += m_movement;
    }

    if(m_moveMode == 2)
    {
        m_time += m_movement * 0.1;
        if(m_time > 1.0f)
            m_time = 0.0f;
        //glm::vec3 p = m_spline.interpolatedPoint(m_time);
        //m_position = p;
    }    
}

void Light::move(unique_ptr<CameraManager>& camManager, float diffX, float diffY)
{
    if(m_moveMode == 0)
    {
        Transform trans;	
        glm::vec3 dir;
        glm::vec3 right;
        glm::vec3 up;
        glm::vec3 pos;

        camManager->currentMatrices(trans);
        getCameraFrame(trans, dir, up, right, pos);

        glm::vec3 movZ;
        glm::vec3 movX;
        glm::vec3 movY(0, m_position.y, 0);

        glm::vec3 onPlane = glm::cross(right, glm::vec3(0, 1, 0));
        movZ = glm::vec3(0, 0, m_position.z) + onPlane * diffY;
        movX = glm::vec3(m_position.x, 0, 0) + right * diffX;

        glm::vec3 result = movX + movY + movZ;

        float clamped_x = clamp(result.x, -params::inst().scale, params::inst().scale);
        float clamped_z = clamp(result.z, -params::inst().scale, params::inst().scale);

        m_position = glm::vec3(clamped_x, result.y, clamped_z);

        if(m_record)
        {
            std::chrono::time_point<std::chrono::high_resolution_clock> cur_time = std::chrono::high_resolution_clock::now();

            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(cur_time - m_oldTime).count();

            if(diff >= 250)
            {
                m_oldTime = std::chrono::high_resolution_clock::now();
                m_curPath.positions.push_back(m_position);
            }
        }
        m_moved = true;
    }
}

void Light::loadPaths()
{
    QDir dir("../data/Light");

    QStringList filters;
    filters << "*.light";

    QFileInfoList list = dir.entryInfoList(filters);

    for(int i=0; i<list.size(); ++i)
    {
        QFile file(list[i].absoluteFilePath());

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        QTextStream in(&file);

        Path path;
        path.name = list[i].baseName();

        while (!in.atEnd()) 
        {
            QString line = in.readLine();
            QStringList strPos = line.split(" ");

            glm::vec3 p;
            p.x = strPos[0].toFloat();
            p.y = strPos[1].toFloat();
            p.z = strPos[2].toFloat();

            path.positions.push_back(p);
        }  

        file.close();
        m_curPath = path;
    }
}

void Light::savePaths()
{
    QString folder = "../data/Light/";

    for(uint i=0; i<m_paths.size(); ++i)
    {
        QString name = m_paths[i].name;

        QFile file(folder + name + ".light");

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream out(&file);
        for(uint j=0; j<m_paths[i].positions.size(); ++j)
        {
            glm::vec3 p = m_paths[i].positions[j];
            out << p.x << " " << p.y << " " << p.z << endl;
        }

        file.close();
    }
}

void Light::recordPath(bool record)
{
    m_record = record;

    if(m_record && m_first)
    {       
        QDate date = QDate::currentDate(); 
        QTime time = QTime::currentTime();

        int year = date.year();
        int month = date.month();
        int day = date.day();

        int hour = time.hour();
        int minute = time.minute();
        int second = time.second();

        QString sYear   = QString::number(year);
        QString sMonth  = QString::number(month);
        QString sDay    = QString::number(day);
        QString sHour   = QString::number(hour);
        QString sMinute = QString::number(minute);
        QString sSecond = QString::number(second);
        QString sNull   = QString::number(0);

        QString fMonth  = month  < 10 ? sNull + sMonth  : sMonth;
        QString fDay    = day    < 10 ? sNull + sDay    : sDay;
        QString fHour   = hour   < 10 ? sNull + sHour   : sHour;
        QString fMinute = minute < 10 ? sNull + sMinute : sMinute;
        QString fSecond = second < 10 ? sNull + sSecond : sSecond;

        QString name = sYear + fMonth + fDay + "_" + fHour + fMinute + fSecond;

        m_curPath.name = name;
        m_curPath.positions.clear();

        m_first = false;
        m_saved = false;
    }
    else if(record == false && m_saved == false)
    {
        m_paths.push_back(m_curPath);
        savePaths();

        m_saved = true;
        m_first = true;
    }
}

void Light::toggleMode()
{
    if(m_moveMode == 0)
    {
        m_oldPosition = m_position;
    }

    m_moveMode ++;
    if(m_moveMode > 2)
    {
        m_moveMode = 0;
    }

    if(m_moveMode == 0)
    {
        m_position = m_oldPosition;
    }

    if(m_moveMode == 2)
    {
        //m_spline.clear();
        //for(uint i=0; i<m_curPath.positions.size(); ++i)
        //{
        //    m_spline.addPoint(m_curPath.positions[i]);
        //}

        m_time = 0;
    }
}

void Light::update(float delta)
{
    m_movement = m_distance * delta;
    autoMove();
    //m_moved = true;
}

bool  Light::hasMoved()
{
    return m_moved;
}
