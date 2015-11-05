#include "camera.h"
#include "frustum.h"
#include "shader.h"
#include "spline.h"
#include "vertexbuffer_object_attribs.h"

Camera::Camera(glm::vec3 pos,
        float     heading,
        float     pitch,
        float     roll,
        float     fov,
        float     ncp,
        float     fcp)
    : m_heading(heading),
    m_pitch(pitch),
    m_roll(roll),
    m_fov(fov),
    m_ncp(ncp),
    m_fcp(fcp),
    m_aspect(1.3f),
    m_lerpFactor(0.0f),
    m_locked(false),
    m_pos(pos),
    m_dir(0.0f, 0.0f, -1.0f),
    m_up(0.0f, 1.0f, 0.0f),
    m_right(1.0f, 0.0f, 0.0f),
    m_strafe(0.0f, 0.0f, 0.0f),
    m_cameraColor(1.0f, 1.0f, 0.0f),
    m_moveSpeed(0.01f),
    m_forwardSpeed(0.0f),
    m_strafeSpeed(0.0f),
    m_timer(0.0f),
    m_time(0.0f),
    m_timerDiff(0.0f),
    m_secsPerFrame(0.0f),
    m_desiredDistance(4.0f),
    m_movementValue(0.0f),
    m_interpolate(false),
    m_idxInterpolOld(0),
    m_idxInterpolNew(1),
    m_activeFrameSet(0),
    m_activeFrameSetName("No Set Available"),
    m_renderFrustum(true),
    m_frustum(nullptr)
{
    m_conf = PERSPECTIVE;

    // Draw camera frustum
    m_frustum = new Frustum();
    m_frustum->setCamInternals(m_fov, m_aspect, 0.2f, 1.2f);

    // Camera spline
    m_spline = new Spline();
    m_splineView = new Spline();
    m_splineSpeed = new Spline();
    m_splineVBO = new VertexBufferObjectAttribs();

    m_splineVBO->addAttrib(VERTEX_POSITION);
    m_splineVBO->addAttrib(VERTEX_NORMAL);
    m_splineVBO->addAttrib(VERTEX_COLOR);
    m_splineVBO->addAttrib(VERTEX_TEXTURE);

    // Frame
    m_frameVBO = new VertexBufferObjectAttribs(); 
    m_frameVBO->addAttrib(VERTEX_POSITION);
    m_frameVBO->addAttrib(VERTEX_NORMAL);
    m_frameVBO->addAttrib(VERTEX_COLOR);
    m_frameVBO->addAttrib(VERTEX_TEXTURE);

    update();

    m_hpTimer.reset();
    m_timer = m_hpTimer.time();
}

Camera::~Camera(){
    if(m_frustum) { 
        delete m_frustum;
    } 

    if(m_spline) { 
        delete m_spline; 
    } 

    if(m_splineView){
        delete m_splineView;
    }

    if(m_splineSpeed) { 
        delete m_splineSpeed; 
    } 

    if(m_splineVBO){
        delete m_splineVBO;
    }

    if(m_frameVBO){
        delete m_frameVBO;
    }
}

void Camera::update(){
    m_view = glm::mat4(1.0f);
    m_projection = glm::mat4(1.0f);

    if (m_conf == PERSPECTIVE) { 
        m_projection = glm::perspective(m_fov, m_aspect, m_ncp, m_fcp);
    } 
    else{
    }

    // Make the Quaternions that will represent our rotations
    m_qPitch = glm::angleAxis(glm::radians(m_pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    m_qHeading = glm::angleAxis(glm::radians(m_heading), glm::vec3(0.0f, 1.0f, 0.0f));
    m_qRoll = glm::angleAxis(glm::radians(m_roll), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::quat q = m_qPitch * m_qHeading * m_qRoll;
    m_up = glm::rotate(q, glm::vec3(0.0f, 1.0f, 0.0f));
    m_dir = glm::rotate(q, glm::vec3(0.0f, 0.0f, 1.0f));

    m_view = glm::lookAt(m_pos, m_pos + m_dir, m_up);

    m_right = glm::cross(m_dir, m_up);

    // Update camera frustum
    m_frustum->setCamDef(m_pos, m_dir, m_up);

    m_strafe = m_right * m_strafeSpeed;
    m_dir = m_dir * m_forwardSpeed;

    m_pos += m_dir;
    m_pos += m_strafe;

}

void Camera::lock(){
    m_locked = true;
}

void Camera::unlock(){
    m_locked = false;
}

void Camera::changePitch(float degrees){
    if (!m_locked) { 
        m_pitch -= degrees; 
    } 
}

void Camera::changeHeading(float degrees){
    if (!m_locked) { 
        m_heading += degrees; 
    } 
}

void Camera::changeRoll(float degrees){
    if (!m_locked) { 
        m_roll += degrees;
    } 
}

// Access camera info
void Camera::getMatrices(Transform &trans){
    update();

    trans.view = m_view;
    trans.projection = m_projection;
    trans.viewProjection = m_projection * m_view;
}

glm::vec3 Camera::position() const
{
    return m_pos;
}

glm::vec3 Camera::direction() const{
    return m_dir;
}

glm::vec3 Camera::up() const{
    return m_up;
}

glm::vec3 Camera::right() const{
    return m_right;
}

float Camera::pitch() const{
    return m_pitch;
}

float Camera::heading() const{
    return m_heading;
}

float Camera::roll() const{
    return m_roll;
}

float Camera::fov() const{
    return m_fov;
}

float Camera::ncp() const{
    return m_ncp;
}

float Camera::fcp() const{
    return m_fcp;
}

// Set camera info
void Camera::setColor(float r, float g, float b){
    m_cameraColor.x = r;
    m_cameraColor.y = g;
    m_cameraColor.z = b;
}

void Camera::setPosition(glm::vec3 pos){
    m_pos = pos;
}

void Camera::setHeading(float degrees){
    m_heading = degrees;
}

void Camera::setPitch(float degrees){
    m_pitch = degrees;
}

void Camera::setRoll(float degrees){
    m_roll = degrees;
}

void Camera::setSpeed(float s){
    m_desiredDistance = s;
}

void Camera::increaseDistPerSec(float delta){
    m_desiredDistance += delta;
}

void Camera::decreaseDistPerSec(float delta){
    m_desiredDistance -= delta;
    if(m_desiredDistance < 0.1f)
        m_desiredDistance = 0.1f;
}

void Camera::setDistPerSec(float value){
    m_desiredDistance = value;
}

void Camera::updateCamAspectRatio(float ratio){
    m_aspect = ratio;
}

// Camera movement
void Camera::moveForward(bool move)
{
    if (!m_locked) {
        if (move)
            m_forwardSpeed = m_movementValue;
        else
            m_forwardSpeed = 0.0f;
    }
}

void Camera::moveBackward(bool move)
{
    if (!m_locked) {
        if (move)
            m_forwardSpeed = -m_movementValue;
        else
            m_forwardSpeed = 0.0f;
    }
}

void Camera::strafeLeft(bool move)
{
    if (!m_locked) {
        if (move)
            m_strafeSpeed = -m_movementValue;
        else
            m_strafeSpeed = 0.0f;
    }
}

void Camera::strafeRight(bool move)
{
    if (!m_locked) {
        if (move)
            m_strafeSpeed = m_movementValue;
        else
            m_strafeSpeed = 0.0f;
    }
}

// Camera Frames
void Camera::addFrame(glm::vec3 pos, float heading, float pitch, float desiredDistance){
    m_spline->addPoint(pos);

    glm::vec3 viewDir(heading, pitch, 0.0f);
    m_splineView->addPoint(viewDir);

    glm::vec3 speed(desiredDistance, 0.0f, 0.0f);
    m_splineSpeed->addPoint(speed);

    CameraFrame frame;
    frame.pos = pos;
    frame.pos.z *= -1.0f;
    frame.heading = heading;
    frame.pitch = pitch;
    frame.desiredDistance = desiredDistance;

    m_camFrames.push_back(frame);
}

void Camera::autoAddFrame(){
    m_spline->addPoint(m_pos);

    glm::vec3 viewDir(m_heading, m_pitch, 0.0f);
    m_splineView->addPoint(viewDir);

    glm::vec3 speed(m_desiredDistance, 0.0f, 0.0f);
    m_splineSpeed->addPoint(speed);

    CameraFrame frame;
    frame.pos = m_pos;
    frame.pos.z *= -1.0f;
    frame.heading = m_heading;
    frame.pitch = m_pitch;
    frame.desiredDistance = speed.x;
    m_camFrames.push_back(frame);
}

void Camera::clearFrames(){
    m_camFrames.clear();
    m_spline->clear();
    m_splineView->clear();
    m_splineSpeed->clear();
}

void Camera::saveFrames(QString filePath){
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) { 
        return; 
    } 

    QTextStream out(&file);

    for(size_t i = 0; i < m_camFrames.size(); ++i) { 
        CameraFrame cf = m_camFrames.at(i);

        if(i > 0) 
            out << "\n";

        out << cf.pos.x << " " << cf.pos.y << " " << cf.pos.z << " " << cf.heading << " " << cf.pitch << " " << cf.desiredDistance;
    } 

    file.close();
    cout << "Camera frames saved to " << filePath.toStdString() << endl;
}

void Camera::saveFrames(){
    // Save frame using current system timestamp.
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

    QString fileName = sYear + fMonth + fDay + "_" + fHour + fMinute + fSecond + ".cam";

    QString filePath = m_frameSetFolder + "/" + fileName;
    saveFrames(filePath);
}

void Camera::loadFrames(QString filePath){
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) { 
        cout << "Camera::loadFrames() -> No Camera File Available"  << endl;
        return;
    } 

    delete m_spline;
    delete m_splineView;
    delete m_splineSpeed;

    m_spline = new Spline();
    m_splineView = new Spline();
    m_splineSpeed = new Spline();

    m_camFrames.clear();
    m_time = 0;

    QTextStream in(&file);

    while(!in.atEnd()) { 
        glm::vec3 v;
        float heading = 0.0f;
        float pitch = 0.0f;
        float desiredDistance = 0.0f;

        in >> v.x >> v.y >> v.z >> heading >> pitch >> desiredDistance;
        v.z *= -1.0f;
        addFrame(v, heading, pitch, desiredDistance);
    } 

    file.close();
}

void Camera::loadFrameDirectory(QString folderPath){
    m_frameSetFolder = folderPath;

    QDir dir(folderPath);
    m_frameFileNames = dir.entryInfoList(QDir::Files);

    if(m_frameFileNames.size()) { 
        QFileInfo fi = m_frameFileNames.at(0);

        if(fi.exists()) { 
            loadFrames(fi.absoluteFilePath());
			m_activeFrameSetName = fi.baseName() + "." + fi.suffix();
        }  
    } 
}

void Camera::changeFrameSet(){
    m_activeFrameSet++;

    if(m_activeFrameSet >= m_frameFileNames.size()) { 
        m_activeFrameSet = 0; 
    } 

    if(m_frameFileNames.size() > 0) { 
        QFileInfo fi = m_frameFileNames.at(m_activeFrameSet);

        if(fi.exists()){
            loadFrames(fi.absoluteFilePath());
            m_activeFrameSetName = fi.baseName() + "." + fi.suffix();
        } 
    } 
}

QString Camera::frameSetName() const{
    return m_activeFrameSetName;
}

void Camera::toggleInterpolation(){
    m_interpolate = !m_interpolate;
}

void Camera::interpolate(){
    if(m_interpolate) { 
        splineInterpolation();
    } 
}

void Camera::interpolate(float speed){
    if(m_camFrames.size() == 0) { 
        return; 
    } 

    if(m_idxInterpolOld >= m_camFrames.size()) { 
        m_idxInterpolOld = 0; 
    } 

    if(m_idxInterpolNew >= m_camFrames.size()) { 
        m_idxInterpolNew = 0; 
    } 

    if(m_lerpFactor > 1.0f) { 
        m_lerpFactor = 1.0f; 
    } 

    glm::vec3 oldPos = m_camFrames.at(m_idxInterpolOld).pos;
    glm::vec3 newPos = m_camFrames.at(m_idxInterpolNew).pos;
    glm::vec3 curPos = glm::vec3(0.0f, 0.0f, 0.0f);

    curPos.x = hermiteInterpolation(oldPos.x/4.0f, oldPos.x, newPos.x, newPos.x/2.0f, m_lerpFactor, 1, 1);
    curPos.y = hermiteInterpolation(oldPos.y/4.0f, oldPos.y, newPos.y, newPos.y/2.0f, m_lerpFactor, 1, 1);
    curPos.z = hermiteInterpolation(oldPos.z/4.0f, oldPos.z, newPos.z, newPos.z/2.0f, m_lerpFactor, 1, 1);

    m_pos = curPos;

    float oldHeading = m_camFrames.at(m_idxInterpolOld).heading;
    float newHeading = m_camFrames.at(m_idxInterpolNew).heading;
    float curHeading = 0.0f;

    curHeading = hermiteInterpolation(oldHeading/4.0f, oldHeading, newHeading, newHeading/2.0f, m_lerpFactor, 1, 1);

    m_heading = curHeading;

    float oldPitch = m_camFrames.at(m_idxInterpolOld).pitch;
    float newPitch = m_camFrames.at(m_idxInterpolNew).pitch;
    float curPitch = 0.0f;

    curPitch = hermiteInterpolation(oldPitch/4.0f, oldPitch, newPitch, newPitch/2.0f, m_lerpFactor, 1, 1);
    m_pitch = curPitch;

    m_lerpFactor += speed;

    if( glm::length(curPos - newPos) < 1e-6 ){ 
        m_idxInterpolOld = m_idxInterpolNew;
        m_idxInterpolNew++;
        m_lerpFactor = 0.0f; 
    } 
}

void Camera::splineInterpolation(){
    float speed = m_movementValue;

    if(m_spline->numPoints() == 0) { 
        return; 
    } 

    if(speed < 0.0001f) { 
        speed = 0.0001f; 
    } 

    if(m_time > 1.0f) { 
        m_time = 0.0f; 
    } 

    m_time += speed * 0.01f;

    glm::vec3 v = m_spline->interpolatedPoint(m_time);
    glm::vec3 d = m_splineView->interpolatedPoint(m_time);
    glm::vec3 s = m_splineSpeed->interpolatedPoint(m_time);

    v.z               *= -1.0f;
    m_pos              = v;
    m_heading          = d.x;
    m_pitch            = d.y;
    m_desiredDistance  = s.x;
}

void Camera::determineMovement(){
    m_timerDiff = m_hpTimer.time() - m_timer;
    m_secsPerFrame = (float)(m_timerDiff) / 1000.0f;

    m_movementValue = (float)(m_desiredDistance * m_secsPerFrame);
    m_timer = m_hpTimer.time();
}

void Camera::render(Shader* shader){
        glm::mat4 model(1.0f);
        shader->setMatrix("matModel", model);

        if(m_renderFrustum){
            m_frustum->drawLines();
        }

        renderFrames();
        renderSpline();
}

void Camera::renderFrames(){
    if(m_camFrames.size() == 0) { 
        return; 
    } 

    glm::vec4 frameColor(m_cameraColor.x, m_cameraColor.y, m_cameraColor.z, 0.7f);
    // Add vertex data
    vector<VertexBufferObjectAttribs::DATA> data;
    float alpha_ratio = 1.0f / m_camFrames.size();
    for(size_t i = 0; i < m_camFrames.size(); ++i) { 
        float color_ratio = 1.0 - 0.5f * i * alpha_ratio;
        glm::vec3 p = m_camFrames.at(i).pos;

        VertexBufferObjectAttribs::DATA pv;
        pv.vx = p.x;
        pv.vy = p.y;
        pv.vz = -p.z;
        pv.cx = frameColor.x * color_ratio;
        pv.cy = frameColor.y * color_ratio;
        pv.cz = frameColor.z * color_ratio;
        pv.cw = frameColor.w;
        data.push_back(pv);
    }

    m_frameVBO->setData(&data[0], GL_STATIC_DRAW, data.size(), GL_POINTS);
    m_frameVBO->bindAttribs();
    glPointSize(10.0f); 
        m_frameVBO->render();
    glPointSize(1.0f);
}

void Camera::renderSpline(){
    if(!m_spline || m_spline->numPoints() == 0) 
        return;

    glm::vec4 splineColor(0.0f, 0.807f, 0.819f, 0.7f);
    // Add vertex data
    vector<VertexBufferObjectAttribs::DATA> data;
    for(float f = 0.0f;  f<1.0f; f += 0.01f) { 
        float color_ratio = 1.0f - 0.5f * f;
        glm::vec3 v = m_spline->interpolatedPoint(f);

        VertexBufferObjectAttribs::DATA pv;
        pv.vx = v.x;
        pv.vy = v.y;
        pv.vz = v.z;
        pv.cx = splineColor.x;
        pv.cy = splineColor.y;
        pv.cz = splineColor.z;
        pv.cw = splineColor.w;
        data.push_back(pv);
    }

    m_splineVBO->setData(&data[0], GL_STATIC_DRAW, data.size(), GL_LINE_STRIP);
    m_splineVBO->bindAttribs();
    m_splineVBO->render();
}
