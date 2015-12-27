/*=====================================================================================
  headers.cpp
Description:  Global Header

Created by Chen Chen on 09/28/2015
=====================================================================================*/

#include "headers.h"

vector<string> split(string &str, char delimiter) {
    vector<string> internal;
    stringstream ss(str);  // Turn the string into a stream.
    string tok;

    while (getline(ss, tok, delimiter)) {
        internal.push_back(tok);
    }

    return internal;
}

void resetBBOX() {
    params::inst().boundBox.bottomLeft = glm::vec3(1e10, 1e10, 1e10);
    params::inst().boundBox.upperRight = glm::vec3(-1e10, -1e10, -1e10);
}

void updateBBOX(float minX, float maxX, float minY, float maxY, float minZ,
                float maxZ) {
    glm::vec3 &minBBOX = params::inst().boundBox.bottomLeft;
    glm::vec3 &maxBBOX = params::inst().boundBox.upperRight;

    params::inst().boundBox.updated = true;
    if (minBBOX.x > minX) {
        minBBOX.x = minX;
    }
    if (maxBBOX.x < maxX) {
        maxBBOX.x = maxX;
    }

    if (minBBOX.y > minY) {
        minBBOX.y = minY;
    }
    if (maxBBOX.y < maxY) {
        maxBBOX.y = maxY;
    }

    if (minBBOX.z > minZ) {
        minBBOX.z = minZ;
    }
    if (maxBBOX.z < maxZ) {
        maxBBOX.z = maxZ;
    }
}

glm::vec3 BBOXNormalize(double x, double y, double z) {
    glm::vec3 minBBOX = params::inst().boundBox.bottomLeft;
    glm::vec3 maxBBOX = params::inst().boundBox.upperRight;

    double lx = maxBBOX.x - minBBOX.x;
    double ly = maxBBOX.y - minBBOX.y;
    double lz = maxBBOX.z - minBBOX.z;
    double l = lx;
    if (l < ly) {
        l = ly;
    }
    if (l < lz) {
        l = lz;
    }

    double newX = (2.0 * x - minBBOX.x - maxBBOX.x) / l;
    double newY = (2.0 * y - minBBOX.y - maxBBOX.y) / l;
    double newZ = (2.0 * z - minBBOX.z - maxBBOX.z) / l;

    return glm::vec3(newX, newY, newZ);
}

float cosineInterpolation(float a, double b, double s) {
    float s2;

    s2 = (float)(1 - cos(s * math_pi)) / 2;

    return float((a * (1 - s2) + b * s2));
}

double hermiteInterpolation(double y0, double y1, double y2, double y3,
                            double mu, double tension, double bias) {
    double m0, m1, mu2, mu3;
    double a0, a1, a2, a3;

    mu2 = mu * mu;
    mu3 = mu2 * mu;
    m0 = (y1 - y0) * (1 + bias) * (1 - tension) / 2;
    m0 += (y2 - y1) * (1 - bias) * (1 - tension) / 2;
    m1 = (y2 - y1) * (1 + bias) * (1 - tension) / 2;
    m1 += (y3 - y2) * (1 - bias) * (1 - tension) / 2;
    a0 = 2 * mu3 - 3 * mu2 + 1;
    a1 = mu3 - 2 * mu2 + mu;
    a2 = mu3 - mu2;
    a3 = -2 * mu3 + 3 * mu2;

    return (a0 * y1 + a1 * m0 + a2 * m1 + a3 * y2);
}

void saveImage(QImage &img) {
    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();

    int year = date.year();
    int month = date.month();
    int day = date.day();

    int hour = time.hour();
    int minute = time.minute();
    int second = time.second();

    QString sYear = QString::number(year);
    QString sMonth = QString::number(month);
    QString sDay = QString::number(day);
    QString sHour = QString::number(hour);
    QString sMinute = QString::number(minute);
    QString sSecond = QString::number(second);
    QString sNull = QString::number(0);

    QString fMonth = month < 10 ? sNull + sMonth : sMonth;
    QString fDay = day < 10 ? sNull + sDay : sDay;
    QString fHour = hour < 10 ? sNull + sHour : sHour;
    QString fMinute = minute < 10 ? sNull + sMinute : sMinute;
    QString fSecond = second < 10 ? sNull + sSecond : sSecond;

    QString fileName =
        sYear + fMonth + fDay + "_" + fHour + fMinute + fSecond + ".png";

    img.save(fileName, "png", 100);
    cout << "\tframe buffer saved to " << fileName.toStdString() << endl;
}

void saveImage(QImage &img, int idx) {
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

    QString sYear = QString::number(year);
    QString sMonth = QString::number(month);
    QString sDay = QString::number(day);
    QString sHour = QString::number(hour);
    QString sMinute = QString::number(minute);
    QString sSecond = QString::number(second);
    QString sNull = QString::number(0);

    QString fMonth = month < 10 ? sNull + sMonth : sMonth;
    QString fDay = day < 10 ? sNull + sDay : sDay;
    QString fHour = hour < 10 ? sNull + sHour : sHour;
    QString fMinute = minute < 10 ? sNull + sMinute : sMinute;
    QString fSecond = second < 10 ? sNull + sSecond : sSecond;

    // QString fileName = "Output/" + sYear + fMonth + fDay + "_" + fHour +
    // fMinute + fSecond + "_" + number + ".jpg";
    QString fileName = "Output/" + number + ".png";

    img.save(fileName, "png", 100);
    cout << "\tframe buffer saved to " << fileName.toStdString() << endl;
}

void checkGLError() {
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
            std::cout << "GL_ERROR: GL_INVALID_FRAMEBUFFER_OPERATION"
                      << std::endl;
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

void checkGLVersion() {
    char *vendor = NULL;
    char *renderer = NULL;
    char *version = NULL;
    char *extentions = NULL;

    vendor = (char *)glGetString(GL_VENDOR);
    renderer = (char *)glGetString(GL_RENDERER);
    version = (char *)glGetString(GL_VERSION);
    extentions = (char *)glGetString(GL_EXTENSIONS);

    cout << vendor << endl;
    cout << renderer << endl;
    cout << version << endl;

    QString ext(extentions);
    QStringList extList = ext.split(" ");

    for (int i = 0; i < extList.size(); ++i) {
        cout << extList.at(i).toStdString() << endl;
    }

    cout << extList.size() << "Extentions listed!" << endl;
}

void getCameraFrame(const Transform &trans, glm::vec3 &dir, glm::vec3 &up,
                    glm::vec3 &right, glm::vec3 &pos) {
    glm::mat4 view = trans.matView;

    up = glm::vec3(view[1][0], view[1][1], view[1][2]);
    up = glm::normalize(up);

    right = glm::vec3(view[0][0], view[0][1], view[0][2]);
    right = glm::normalize(right);
    dir = glm::cross(up, right);

    pos.x = -(view[0][0] * view[3][0] + view[0][1] * view[3][1] +
              view[0][2] * view[3][2]);
    pos.y = -(view[1][0] * view[3][0] + view[1][1] * view[3][1] +
              view[0][2] * view[3][2]);
    pos.z = -(view[2][0] * view[3][0] + view[2][1] * view[3][1] +
              view[0][2] * view[3][2]);
}

// Return random float between -0.5 and 0.5
float jitter() { return ((float)rand() / RAND_MAX) - 0.5f; }

void buildOffsetTex(int texSize, int samplesU, int samplesV) {
    int size = texSize;
    int samples = samplesU * samplesV;
    int bufSize = size * size * samples * 2;
    vector<float> data(bufSize);
    float TWOPI = 2.0f * 3.1415926;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            for (int k = 0; k < samples; k += 2) {
                int x1, y1, x2, y2;
                x1 = k % (samplesU);
                y1 = (samples - 1 - k) / samplesU;
                x2 = (k + 1) % samplesU;
                y2 = (samples - 1 - k - 1) / samplesU;
                glm::vec4 v;
                // Center on grid and jitter
                v.x = (x1 + 0.5f) + jitter();
                v.y = (y1 + 0.5f) + jitter();
                v.z = (x2 + 0.5f) + jitter();
                v.w = (y2 + 0.5f) + jitter();
                // Scale between 0 and 1
                v.x /= samplesU;
                v.y /= samplesV;
                v.z /= samplesU;
                v.w /= samplesV;
                // Warp to disk
                int cell = ((k / 2) * size * size + j * size + i) * 4;
                data[cell + 0] = sqrtf(v.y) * cosf(TWOPI * v.x);
                data[cell + 1] = sqrtf(v.y) * sinf(TWOPI * v.x);
                data[cell + 2] = sqrtf(v.w) * cosf(TWOPI * v.z);
                data[cell + 3] = sqrtf(v.w) * sinf(TWOPI * v.z);
            }
        }
    }
    glActiveTexture(GL_TEXTURE1);
    GLuint texID;
    glGenTextures(1, &texID);
    params::inst().shadowInfo.offsetTexID = texID;
    glBindTexture(GL_TEXTURE_3D, texID);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, size, size, samples / 2, 0,
                 GL_RGBA, GL_FLOAT, &data[0]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}
