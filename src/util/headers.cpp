/*=====================================================================================
  headers.cpp
Description:  Global Header

Created by Chen Chen on 09/28/2015
=====================================================================================*/

#include "headers.h"

void resetBBOX(){
    params::inst()->minBBOX = glm::vec3(1e10, 1e10, 1e10); 
    params::inst()->maxBBOX = glm::vec3(-1e10, -1e10, -1e10);
}

void updateBBOX(float minX, float maxX,
                float minY, float maxY,
                float minZ, float maxZ){
    glm::vec3& minBBOX = params::inst()->minBBOX;
    glm::vec3& maxBBOX = params::inst()->maxBBOX;

    params::inst()->bboxUpdated = true;
    if(minBBOX.x > minX) { 
        minBBOX.x = minX;
    } 
    if(maxBBOX.x  < maxX) { 
        maxBBOX.x = maxX; 
    } 

    if(minBBOX.y > minY) { 
        minBBOX.y = minY;
    } 
    if(maxBBOX.y < maxY) { 
        maxBBOX.y = maxY; 
    }

    if(minBBOX.z > minZ) { 
        minBBOX.z = minZ;
    } 
    if(maxBBOX.z < maxZ) { 
        maxBBOX.z = maxZ; 
    }
}

glm::vec3 BBOXNormalize(float x, float y, float z){
    glm::vec3 minBBOX = params::inst()->minBBOX;
    glm::vec3 maxBBOX = params::inst()->maxBBOX;

    float lx = maxBBOX.x - minBBOX.x; 
    float ly = maxBBOX.y - minBBOX.y; 
    float lz = maxBBOX.z - minBBOX.z; 
    float l = lx;
    if(l < ly) { 
        l = ly; 
    } 
    if(l < lz) { 
        l = lz; 
    } 

    float newX = (x - minBBOX.x) / l * 2.0f - 1.0f;
    float newY = (y - minBBOX.y) / l * 2.0f - 1.0f;
    float newZ = (y - minBBOX.z) / l * 2.0f - 1.0f;

    return glm::vec3(newX, newY, newZ);
}

float cosineInterpolation(float a, double b, double s)
{
   float s2;

   s2 = (float)( 1 - cos(s * math_pi) ) / 2;

   return float((a * (1 - s2) + b * s2));
}

double hermiteInterpolation(double y0, double y1, double y2, double y3, double mu, double tension, double bias)
{
    double m0,m1,mu2,mu3;
    double a0,a1,a2,a3;

    mu2 = mu * mu;
    mu3 = mu2 * mu;
    m0  = (y1-y0)*(1+bias)*(1-tension)/2;
    m0 += (y2-y1)*(1-bias)*(1-tension)/2;
    m1  = (y2-y1)*(1+bias)*(1-tension)/2;
    m1 += (y3-y2)*(1-bias)*(1-tension)/2;
    a0 =  2*mu3 - 3*mu2 + 1;
    a1 =    mu3 - 2*mu2 + mu;
    a2 =    mu3 -   mu2;
    a3 = -2*mu3 + 3*mu2;

    return(a0*y1+a1*m0+a2*m1+a3*y2);
}

void saveImage(QImage& img){
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

    QString fMonth  = month < 10 ? sNull + sMonth : sMonth;
    QString fDay    = day < 10 ? sNull + sDay : sDay;
    QString fHour   = hour < 10 ? sNull + sHour : sHour;
    QString fMinute = minute < 10 ? sNull + sMinute : sMinute;
    QString fSecond = second < 10 ? sNull + sSecond : sSecond;

    QString fileName = sYear + fMonth + fDay + "_" + fHour + fMinute + fSecond + ".jpg";

    img.save(fileName, "jpg", 100);
    cout << "\tframe buffer saved to" << fileName.toStdString() << endl;
}

void saveImage(QImage& img, int idx){
    cout << "frame idx: " << idx << endl;
    QDate date = QDate::currentDate(); 
    QTime time = QTime::currentTime();

    int year = date.year();
    int month = date.month();
    int day = date.day();

    int hour = time.hour();
    int minute = time.minute();
    int second = time.second();

    QString number = QString("%1").arg(idx, 5, 10, QChar('0'));

    QString sYear   = QString::number(year);
    QString sMonth  = QString::number(month);
    QString sDay    = QString::number(day);
    QString sHour   = QString::number(hour);
    QString sMinute = QString::number(minute);
    QString sSecond = QString::number(second);
    QString sNull   = QString::number(0);

    QString fMonth  = month < 10 ? sNull + sMonth : sMonth;
    QString fDay    = day < 10 ? sNull + sDay : sDay;
    QString fHour   = hour < 10 ? sNull + sHour : sHour;
    QString fMinute = minute < 10 ? sNull + sMinute : sMinute;
    QString fSecond = second < 10 ? sNull + sSecond : sSecond;

    //QString fileName = "Output/" + sYear + fMonth + fDay + "_" + fHour + fMinute + fSecond + "_" + number + ".jpg";
    QString fileName = "Output/" + number + ".png";

    img.save(fileName, "png", 100);
    cout << "\tframe buffer saved to " << fileName.toStdString() << endl;
}

void getCameraFrame(const Transform &trans, 
                    glm::vec3 &dir, 
                    glm::vec3 &up, 
                    glm::vec3 &right, 
                    glm::vec3 &pos)
{
    glm::mat4 view = trans.view;

    up = glm::vec3(view[1][0], view[1][1], view[1][2]);
    up = glm::normalize(up);

    right = glm::vec3(view[0][0], view[0][1], view[0][2]);
    right = glm::normalize(right);
    dir = glm::cross(up, right);

    pos.x = -(view[0][0] * view[3][0] + view[0][1] * view[3][1] + view[0][2] * view[3][2]);
    pos.y = -(view[1][0] * view[3][0] + view[1][1] * view[3][1] + view[0][2] * view[3][2]);
    pos.z = -(view[2][0] * view[3][0] + view[2][1] * view[3][1] + view[0][2] * view[3][2]);

}

glm::vec3 getCamPosFromModelView(const Transform &trans)
{
    glm::mat4 m = trans.view;
    glm::vec3 c;

    glm::mat4 inv = glm::inverse(m);

	/*c.x = -(m.a11 * m.a43 + m.a21 * m.a24 + m.a31 * m.a34);
	c.y = -(m.a12 * m.a43 + m.a22 * m.a24 + m.a32 * m.a34);
	c.z = -(m.a13 * m.a43 + m.a23 * m.a24 + m.a33 * m.a34);*/

	c.x = inv[0][3];
	c.y = inv[1][3];
	c.z = inv[2][3];

	return c;
}

glm::vec3 getViewDirFromModelView(const Transform &trans)
{
    glm::mat4 m = trans.view;
    glm::vec3 c;

	c.x = -m[2][0];
	c.y = -m[2][1];
	c.z = -m[2][2];

	return glm::normalize(c);
}

glm::vec3 getUpDirFromModelView(const Transform &trans)
{
    glm::mat4 m = trans.view;
    glm::vec3 c;

	c.x = m[1][0];
	c.y = m[1][1];
	c.z = m[1][2];

	return glm::normalize(c);
}

void checkGLError(){
    GLenum error = glGetError();

    switch (error) { 
        case GL_NO_ERROR:  
            cout << "GL_ERROR: NO_ERROR" << endl;
            break; 
        case GL_INVALID_ENUM:
            cout << "GL_ERROR: GL_INVALID_ENUM" << endl;
            break;
        case GL_INVALID_OPERATION:
            cout << "GL_ERROR: GL_INVALID_OPERATION" << endl;
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            std::cout << "GL_ERROR: GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
            break;
        case GL_OUT_OF_MEMORY:
            std::cout << "GL_ERROR: GL_OUT_OF_MEMORY" << std::endl;
            break;
        case GL_STACK_UNDERFLOW:
            std::cout << "GL_ERROR: GL_STACK_UNDERFLOW" << std::endl;
            break;
        case GL_STACK_OVERFLOW:
            std::cout << "GL_ERROR: GL_STACK_OVERFLOW" << std::endl;
            break;
        default:  
            break; 
    } 
}

void checkGLVersion()
{
    char *vendor = NULL;
    char *renderer = NULL;
    char *version = NULL;
    char *extentions = NULL;

    vendor = (char*)glGetString(GL_VENDOR);
    renderer = (char*)glGetString(GL_RENDERER);
    version = (char*)glGetString(GL_VERSION);
    extentions = (char*)glGetString(GL_EXTENSIONS);

    cout << vendor << endl;
    cout << renderer << endl;
    cout << version << endl;

    QString ext(extentions);
    QStringList extList = ext.split(" ");    

    for(int i=0; i<extList.size(); ++i)
    {
        cout << extList.at(i).toStdString() << endl;
    }

    cout << extList.size() << "Extentions listed!" << endl;
}

void colorMap(float x, vector<float>& out, vector<float>& cm){
    x = clamp(x*63, 0.f, 63.f - (float)1e-6);
    int idx = int(x);
    float r = fract(x);
    out[0] = cm[idx*3+0]*(1-r) + cm[(idx+1)*3+0]*r;
    out[1] = cm[idx*3+1]*(1-r) + cm[(idx+1)*3+1]*r;
    out[2] = cm[idx*3+2]*(1-r) + cm[(idx+1)*3+2]*r;
}

void colorMapBgr(float x, vector<float>& out, vector<float>& cm)
{
    x = clamp(x*63, 0.f, 63.f - (float)1e-6);
    int idx = int(x);
    float r = fract(x);
    out[2] = cm[idx*3+0]*(1-r) + cm[(idx+1)*3+0]*r;
    out[1] = cm[idx*3+1]*(1-r) + cm[(idx+1)*3+1]*r;
    out[0] = cm[idx*3+2]*(1-r) + cm[(idx+1)*3+2]*r;
}
