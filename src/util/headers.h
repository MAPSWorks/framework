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
#include <QMenu>
#include <QFileDialog>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QDir>

#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLWidget>

#include <vector>
#include <map>
#include <list>
#include <limits>
#include <fstream>
#include <iostream>
#include <sstream>
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

struct Transform {
    glm::mat4 matView;
    glm::mat4 matProjection;
    glm::mat4 matViewProjection;
    glm::mat4 matLightSpace;  // Light's (Projection * View)
};

struct BoundingBox {
    bool updated = false;
    glm::vec3 bottomLeft = glm::vec3(1e10, 1e10, 1e10);
    glm::vec3 upperRight = glm::vec3(-1e10, -1e10, -1e10);
};

struct ShadowInfo {
    bool applyShadow = true;
    GLuint shadowMapID = 0;

    // Random offset texture
    int texSize = 8;
    int texSamplesU = 4;
    int texSamplesV = 8;
    GLuint offsetTexID;  // 3D texture for random offsets
    glm::vec3 offsetTexSize() {
        return glm::vec3(texSize, texSize, texSamplesU * texSamplesV / 2);
    }
};

struct GlobalObjectParams {
    glm::vec2 windowSize;
    float scale = 100.0f;    // all x, y, z coordinate scale by this factor
    int polygonMode = 0;     // 0: normal; 1: wire frame
    int gridRenderMode = 0;  // 0: texture; 1: white color

    float gamma = 2.2f;  // Gamma correction

    BoundingBox boundBox;
    ShadowInfo shadowInfo;

    // Global opengl core function pointer
    QOpenGLFunctions_4_1_Core *glFuncs;
};

// Singleton Template
template <class T>
class Singleton {
public:
    static T &inst() {
        static T singleton;
        return singleton;
    }

private:
    Singleton(){};
    Singleton(const Singleton &){};
    Singleton &operator=(const Singleton &){};
};

typedef Singleton<GlobalObjectParams> params;

// Split string by delimiter
vector<string> split(string &str, char delimiter);

void resetBBOX();
void updateBBOX(float minX, float maxX, float minY, float maxY,
                float minZ = 0.0f, float maxZ = 0.0f);

glm::vec3 BBOXNormalize(double x, double y, double z);

float cosineInterpolation(float a, double b, double s);
double hermiteInterpolation(double y0, double y1, double y2, double y3,
                            double mu, double tension, double bias);

void saveImage(QImage &img);
void saveImage(QImage &img, int idx);

void checkGLError();
void checkGLVersion();

void getCameraFrame(const Transform &trans, glm::vec3 &dir, glm::vec3 &up,
                    glm::vec3 &right, glm::vec3 &pos);

// Return random float between -0.5 and 0.5
float jitter();
void buildOffsetTex(int texSize, int samplesU, int samplesV);

#endif /* end of include guard: HEADERS_H_ */
