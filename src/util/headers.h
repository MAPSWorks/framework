/*=====================================================================================
                                headers.h
    Description:  Global Header
        
    Created by Chen Chen on 09/28/2015
=====================================================================================*/

#ifndef HEADERS_H_
#define HEADERS_H_

#include "global.h"

#include <QFileInfoList> 
#include <QFileInfo>
#include <QMessageBox> 
#include <QKeyEvent> 
#include <QDebug> 
#include <QTimer> 
#include <QColor> 
#include <QDate> 
#include <QTime> 
#include <QDir> 

#include <QtOpenGL>
#include <QOpenGLWidget> 

#include <vector> 
#include <map> 
#include <list> 
#include <limits> 
#include <iostream> 
#include <stdlib.h> 
#include <stdio.h> 
#include <math.h> 
#include <chrono> 

#include <glm/glm.hpp> 
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/matrix_inverse.hpp> 
#include <glm/gtc/type_ptr.hpp> 

using namespace std;

struct Transform{
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 lightView;
    glm::mat3 normal;
    glm::mat4 viewProjection;
    glm::mat4 modelViewProjection;
};

struct GlobalObjectParams{
    glm::vec3 lightPos;
    glm::vec3 lightDir;
    glm::vec3 camPos;
    glm::vec3 attenuation;
    glm::vec2 lightCone;
	glm::vec2 blur;
    glm::vec2 windowSize;
    glm::vec4 clipPlaneGround;
    
    int   gridRenderMode;
    int   polygonMode;

    bool  applyShadow;
    bool  renderMesh;
    bool  renderObjects;    

    float shadowIntensity;
    float depthRangeMax;
    float depthRangeMin;
    float polygonOffsetFactor;
    float polygonOffsetUnits; 
    float ncp;
    float fcp;
    float fov;

    GLuint shadowMapID;
    GLuint shadowMapBlurredID;
};

struct GlobalBoundBox{
    float minX = 1e10;
    float maxX = -1e10;
    float minY = 1e10;
    float maxY = -1e10;
    float minZ = 0.0f;
    float maxZ = 0.0f;
};

//Singleton Template
template<class T>
class Singleton {
public:
    static T* inst()
    {
        if (!m_pInstance)
        {
            m_pInstance = new T;
        }
        return m_pInstance;
    }

private:
    Singleton(){};
    Singleton(const Singleton&){};
    Singleton& operator=(const Singleton&){};

    static T* m_pInstance;

};
template<class T> T* Singleton<T>::m_pInstance = NULL;
typedef Singleton<GlobalObjectParams> params;

typedef Singleton<GlobalBoundBox>     BBOX;
void updateBBOX(float minX, float maxX,
                float minY, float maxY,
                float minZ = 0.0f, float maxZ = 0.0f);

glm::vec2 BBOXNormalize(float x, float y);

float cosineInterpolation(float a, double b, double s);
double hermiteInterpolation(double y0, double y1, double y2, double y3, double mu, double tension, double bias);

void saveImage(QImage& img);
void saveImage(QImage& img, int idx);

void getCameraFrame(const Transform &trans, glm::vec3 &dir, glm::vec3 &up, glm::vec3 &right, glm::vec3 &pos);
glm::vec3 getCamPosFromModelView(const Transform &trans);
glm::vec3 getViewDirFromModelView(const Transform &trans);
glm::vec3 getUpDirFromModelView(const Transform &trans);

void checkGLError();
void checkGLVersion();

static float colorJet[] = { 0.000000f, 0.000000f, 0.562500f, 0.000000f, 0.000000f, 0.625000f, 0.000000f, 0.000000f, 0.687500f, 0.000000f, 0.000000f, 0.750000f, 0.000000f, 0.000000f, 0.812500f, 0.000000f, 0.000000f, 0.875000f, 0.000000f, 0.000000f, 0.937500f, 0.000000f, 0.000000f, 1.000000f, 0.000000f, 0.062500f, 1.000000f, 0.000000f, 0.125000f, 1.000000f, 0.000000f, 0.187500f, 1.000000f, 0.000000f, 0.250000f, 1.000000f, 0.000000f, 0.312500f, 1.000000f, 0.000000f, 0.375000f, 1.000000f, 0.000000f, 0.437500f, 1.000000f, 0.000000f, 0.500000f, 1.000000f, 0.000000f, 0.562500f, 1.000000f, 0.000000f, 0.625000f, 1.000000f, 0.000000f, 0.687500f, 1.000000f, 0.000000f, 0.750000f, 1.000000f, 0.000000f, 0.812500f, 1.000000f, 0.000000f, 0.875000f, 1.000000f, 0.000000f, 0.937500f, 1.000000f, 0.000000f, 1.000000f, 1.000000f, 0.062500f, 1.000000f, 0.937500f, 0.125000f, 1.000000f, 0.875000f, 0.187500f, 1.000000f, 0.812500f, 0.250000f, 1.000000f, 0.750000f, 0.312500f, 1.000000f, 0.687500f, 0.375000f, 1.000000f, 0.625000f, 0.437500f, 1.000000f, 0.562500f, 0.500000f, 1.000000f, 0.500000f, 0.562500f, 1.000000f, 0.437500f, 0.625000f, 1.000000f, 0.375000f, 0.687500f, 1.000000f, 0.312500f, 0.750000f, 1.000000f, 0.250000f, 0.812500f, 1.000000f, 0.187500f, 0.875000f, 1.000000f, 0.125000f, 0.937500f, 1.000000f, 0.062500f, 1.000000f, 1.000000f, 0.000000f, 1.000000f, 0.937500f, 0.000000f, 1.000000f, 0.875000f, 0.000000f, 1.000000f, 0.812500f, 0.000000f, 1.000000f, 0.750000f, 0.000000f, 1.000000f, 0.687500f, 0.000000f, 1.000000f, 0.625000f, 0.000000f, 1.000000f, 0.562500f, 0.000000f, 1.000000f, 0.500000f, 0.000000f, 1.000000f, 0.437500f, 0.000000f, 1.000000f, 0.375000f, 0.000000f, 1.000000f, 0.312500f, 0.000000f, 1.000000f, 0.250000f, 0.000000f, 1.000000f, 0.187500f, 0.000000f, 1.000000f, 0.125000f, 0.000000f, 1.000000f, 0.062500f, 0.000000f, 1.000000f, 0.000000f, 0.000000f, 0.937500f, 0.000000f, 0.000000f, 0.875000f, 0.000000f, 0.000000f, 0.812500f, 0.000000f, 0.000000f, 0.750000f, 0.000000f, 0.000000f, 0.687500f, 0.000000f, 0.000000f, 0.625000f, 0.000000f, 0.000000f, 0.562500f, 0.000000f, 0.000000f, 0.500000f, 0.000000f, 0.000000f };
static float colorHot[] = { 0.041667f, 0.000000f, 0.000000f, 0.083333f, 0.000000f, 0.000000f, 0.125000f, 0.000000f, 0.000000f, 0.166667f, 0.000000f, 0.000000f, 0.208333f, 0.000000f, 0.000000f, 0.250000f, 0.000000f, 0.000000f, 0.291667f, 0.000000f, 0.000000f, 0.333333f, 0.000000f, 0.000000f, 0.375000f, 0.000000f, 0.000000f, 0.416667f, 0.000000f, 0.000000f, 0.458333f, 0.000000f, 0.000000f, 0.500000f, 0.000000f, 0.000000f, 0.541667f, 0.000000f, 0.000000f, 0.583333f, 0.000000f, 0.000000f, 0.625000f, 0.000000f, 0.000000f, 0.666667f, 0.000000f, 0.000000f, 0.708333f, 0.000000f, 0.000000f, 0.750000f, 0.000000f, 0.000000f, 0.791667f, 0.000000f, 0.000000f, 0.833333f, 0.000000f, 0.000000f, 0.875000f, 0.000000f, 0.000000f, 0.916667f, 0.000000f, 0.000000f, 0.958333f, 0.000000f, 0.000000f, 1.000000f, 0.000000f, 0.000000f, 1.000000f, 0.041667f, 0.000000f, 1.000000f, 0.083333f, 0.000000f, 1.000000f, 0.125000f, 0.000000f, 1.000000f, 0.166667f, 0.000000f, 1.000000f, 0.208333f, 0.000000f, 1.000000f, 0.250000f, 0.000000f, 1.000000f, 0.291667f, 0.000000f, 1.000000f, 0.333333f, 0.000000f, 1.000000f, 0.375000f, 0.000000f, 1.000000f, 0.416667f, 0.000000f, 1.000000f, 0.458333f, 0.000000f, 1.000000f, 0.500000f, 0.000000f, 1.000000f, 0.541667f, 0.000000f, 1.000000f, 0.583333f, 0.000000f, 1.000000f, 0.625000f, 0.000000f, 1.000000f, 0.666667f, 0.000000f, 1.000000f, 0.708333f, 0.000000f, 1.000000f, 0.750000f, 0.000000f, 1.000000f, 0.791667f, 0.000000f, 1.000000f, 0.833333f, 0.000000f, 1.000000f, 0.875000f, 0.000000f, 1.000000f, 0.916667f, 0.000000f, 1.000000f, 0.958333f, 0.000000f, 1.000000f, 1.000000f, 0.000000f, 1.000000f, 1.000000f, 0.062500f, 1.000000f, 1.000000f, 0.125000f, 1.000000f, 1.000000f, 0.187500f, 1.000000f, 1.000000f, 0.250000f, 1.000000f, 1.000000f, 0.312500f, 1.000000f, 1.000000f, 0.375000f, 1.000000f, 1.000000f, 0.437500f, 1.000000f, 1.000000f, 0.500000f, 1.000000f, 1.000000f, 0.562500f, 1.000000f, 1.000000f, 0.625000f, 1.000000f, 1.000000f, 0.687500f, 1.000000f, 1.000000f, 0.750000f, 1.000000f, 1.000000f, 0.812500f, 1.000000f, 1.000000f, 0.875000f, 1.000000f, 1.000000f, 0.937500f, 1.000000f, 1.000000f, 1.000000f };

void colorMap(float x, vector<float>& out, vector<float>& cm);
void colorMapBgr(float x, vector<float>& out, vector<float>& cm);

#endif /* end of include guard: HEADERS_H_ */
