#include "video_trajectory.h"

#include "shader.h"
#include "resource_manager.h"

#include "latlon_converter.h"
#include "common.h"

VideoTrajectory::VideoTrajectory()
    : m_vboTriangles(new RenderableObject),
      m_vboLines(new RenderableObject),
      m_vboPoints(new RenderableObject) {
    m_boundBox = Eigen::Vector4f(POSITIVE_INFINITY, -POSITIVE_INFINITY,
                                 POSITIVE_INFINITY, -POSITIVE_INFINITY);
}

VideoTrajectory::~VideoTrajectory() {}

bool VideoTrajectory::load(const string& filename) {
    clear();

    QFileInfo fileInfo(filename.c_str());
    QDir fileDir = fileInfo.dir();
    m_frameDir = fileDir.path().toStdString() + string("/key_frames/");

    Converter& latlon_converter = Converter::getInstance();
    ifstream infile(filename.c_str());
    string line;
    while (std::getline(infile, line)) {
        vector<string> res = split(line, ',');
        if (res.size() != 8) {
            cout << "ERROR from VideoTrajectory::load: invalid trajectory file "
                 << filename << endl;
            return false;
        }

        GPSPoint new_pt;
        new_pt.timestamp = res[0];
        new_pt.latitude = std::stof(res[1]);
        new_pt.longitude = std::stof(res[2]);
        new_pt.speed = std::stof(res[3]);
        double adjusted_heading = 450.0f - std::stof(res[4]);
        adjusted_heading = fmod(adjusted_heading, 360.0f);
        new_pt.heading = adjusted_heading;
        new_pt.altitude = std::stof(res[5]);
        new_pt.hAccuracy = std::stof(res[6]);
        new_pt.vAccuracy = std::stof(res[7]);
        float easting, northing;
        latlon_converter.convertLatLonToXY(new_pt.latitude, new_pt.longitude,
                                           easting, northing);
        new_pt.easting = easting;
        new_pt.northing = northing;
        if (m_boundBox[0] > easting) {
            m_boundBox[0] = easting;
        }
        if (m_boundBox[1] < easting) {
            m_boundBox[1] = easting;
        }
        if (m_boundBox[2] > northing) {
            m_boundBox[2] = northing;
        }
        if (m_boundBox[3] < northing) {
            m_boundBox[3] = northing;
        }
        m_gpsPoints.push_back(new_pt);
    }
    printf(
        "Loading complete from file %s.\n\tThere are %lu points in "
        "trajectory.\n",
        filename.c_str(), m_gpsPoints.size());
    updateBBOX(m_boundBox[0], m_boundBox[1], m_boundBox[2], m_boundBox[3]);
    printf("traj updated bbox: %.2f, %.2f, %.2f, %.2f\n", m_boundBox[0],
           m_boundBox[1], m_boundBox[2], m_boundBox[3]);

    printf("Loading keyframes from\n\t %s\n", m_frameDir.c_str());
    QDir dir(m_frameDir.c_str());
    dir.setNameFilters(QStringList() << "*.png");
    QStringList files = dir.entryList();

    vector<string> filenames;
    for (const auto& v : files) {
        string fullname = m_frameDir + v.toStdString();
        filenames.push_back(fullname);
    }

    ResourceManager::inst().loadTexture(filenames, "key_frames");

    return true;
}

void VideoTrajectory::render(unique_ptr<Shader>& shader) {
    if (m_gpsPoints.empty()) {
        return;
    }

    if (params::inst().boundBox.updated) {
        updateVBO();
    }
    params::inst().glFuncs->glActiveTexture(GL_TEXTURE0);
    params::inst().glFuncs->glBindTexture(
        GL_TEXTURE_2D_ARRAY,
        ResourceManager::inst().getTexture("key_frames")->id());
    shader->seti("quadTex", 0);

    glm::mat4 model(1.0f);
    shader->setMatrix("matModel", model);
    m_vboLines->render();
    params::inst().glFuncs->glDisable(GL_CULL_FACE);
    m_vboTriangles->render();
    params::inst().glFuncs->glEnable(GL_CULL_FACE);
}

void VideoTrajectory::updateVBO() {
    m_pointData.clear();
    m_lineData.clear();
    m_triangleData.clear();
    m_triangleIndices.clear();
    // Point data
    double HALF_W = 10.0f;  // in meters
    glm::vec3 last_pos;
    for (size_t i = 0; i < m_gpsPoints.size(); ++i) {
        Eigen::Vector2d pt_dir = headingToVector2d(m_gpsPoints[i].heading);
        Eigen::Vector2d pt_perp(-pt_dir[1], pt_dir[0]);
        Eigen::Vector2d pt(m_gpsPoints[i].easting, m_gpsPoints[i].northing);
        Eigen::Vector2d left_pt = pt + HALF_W * pt_perp;
        Eigen::Vector2d right_pt = pt - HALF_W * pt_perp;

        RenderableObject::Vertex vpt, v_bottom_left, v_bottom_right,
            v_upper_left, v_upper_right;
        vpt.Position = convertToDisplayCoord(pt[0], pt[1], 1.0f);
        v_bottom_left.Position =
            convertToDisplayCoord(left_pt[0], left_pt[1], 0.1f);
        v_bottom_right.Position =
            convertToDisplayCoord(right_pt[0], right_pt[1], 0.1f);
        glm::vec3 w = v_bottom_right.Position - v_bottom_left.Position;
        float w_length = glm::length(w);
        float h_length = 0.6f * w_length;
        v_upper_left.Position =
            v_bottom_left.Position + glm::vec3(0.0f, h_length, 0.0f);
        v_upper_right.Position =
            v_bottom_right.Position + glm::vec3(0.0f, h_length, 0.0f);
        v_bottom_left.Color = m_triangleColor;
        v_bottom_right.Color = m_triangleColor;
        v_upper_left.Color = m_triangleColor;
        v_upper_right.Color = m_triangleColor;
        GLuint start_idx = m_triangleData.size();

        // Setup texture coordinates
        v_bottom_left.TexCoords = glm::vec4(0.0f, 1.0f, float(i), 0.0f);
        v_bottom_right.TexCoords = glm::vec4(1.0f, 1.0f, float(i), 0.0f);
        v_upper_left.TexCoords = glm::vec4(0.0f, 0.0f, float(i), 0.0f);
        v_upper_right.TexCoords = glm::vec4(1.0f, 0.0f, float(i), 0.0f);

        m_triangleData.push_back(v_bottom_left);
        m_triangleData.push_back(v_bottom_right);
        m_triangleData.push_back(v_upper_right);
        m_triangleData.push_back(v_upper_left);
        // First triangle
        m_triangleIndices.push_back(start_idx);
        m_triangleIndices.push_back(start_idx + 1);
        m_triangleIndices.push_back(start_idx + 2);
        // Second triangle
        m_triangleIndices.push_back(start_idx);
        m_triangleIndices.push_back(start_idx + 2);
        m_triangleIndices.push_back(start_idx + 3);

        if (i > 0) {
            RenderableObject::Vertex line_pt1, line_pt2;
            line_pt1.Position = last_pos;
            line_pt1.Color = m_lineColor;
            line_pt2.Position = vpt.Position;
            line_pt2.Color = m_lineColor;
            m_lineData.push_back(line_pt1);
            m_lineData.push_back(line_pt2);
        }
        last_pos = vpt.Position;
    }
    m_vboLines->setData(m_lineData, GL_LINES);
    m_vboTriangles->setData(m_triangleData, m_triangleIndices, GL_TRIANGLES);
}

void VideoTrajectory::clear() {
    m_boundBox = Eigen::Vector4f(POSITIVE_INFINITY, -POSITIVE_INFINITY,
                                 POSITIVE_INFINITY, -POSITIVE_INFINITY);
    m_frameDir.clear();
    m_gpsPoints.clear();
    m_pointData.clear();
    m_lineData.clear();
    m_triangleData.clear();
    m_triangleIndices.clear();
}

bool VideoTrajectory::isEmpty() { return m_gpsPoints.empty(); }
