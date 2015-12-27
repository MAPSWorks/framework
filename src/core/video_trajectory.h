/*=====================================================================================
                                video_trajectory.h

    Description:  Video Trajectory Class

    Created by Chen Chen on 12/23/2015
=====================================================================================*/

#ifndef VIDEO_TRAJECTORY_H_4CQEOH1R
#define VIDEO_TRAJECTORY_H_4CQEOH1R

#include "headers.h"
#include "common.h"
#include "renderable_object.h"

#include <pcl/common/common.h>
#include <pcl/search/search.h>

#include "pcl_wrapper.h"
#include <Eigen/Dense>

class Shader;
class RenderableObject;

struct GPSPoint {
    string timestamp;
    double latitude;
    double longitude;
    double speed;
    double heading;
    double altitude;
    double hAccuracy;
    double vAccuracy;
    // Derived
    double easting;
    double northing;
};

class VideoTrajectory {
public:
    VideoTrajectory();
    virtual ~VideoTrajectory();

    // IO
    bool load(const string& filename);

    // Rendering
    void render(unique_ptr<Shader>& shader);
    void updateVBO();

    // Clear data
    void clear();

    bool isEmpty();

public:
    // Data that's accessible from outside
    vector<GPSPoint> m_gpsPoints;
    Eigen::Vector4f m_boundBox;  // [minX, maxX, minY, maxY]

private:
    string m_frameDir;
    glm::vec4 m_lineColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 m_triangleColor = glm::vec4(0.0f, 1.0f, 0.0f, 0.5f);
    vector<RenderableObject::Vertex> m_pointData;
    vector<RenderableObject::Vertex> m_lineData;
    vector<RenderableObject::Vertex> m_triangleData;
    vector<GLuint> m_triangleIndices;
    unique_ptr<RenderableObject> m_vboTriangles;
    unique_ptr<RenderableObject> m_vboLines;
    unique_ptr<RenderableObject> m_vboPoints;
};

#endif /* end of include guard: VIDEO_TRAJECTORY_H_4CQEOH1R */
