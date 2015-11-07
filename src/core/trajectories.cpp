#include "trajectories.h"

#include "shader.h"
#include "vertexbuffer_object_attribs.h"

#include <fcntl.h>
#include "gps_trajectory.pb.h"
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <pcl/common/centroid.h>
#include <pcl/search/impl/flann_search.hpp>

#include "latlon_converter.h"
#include "common.h"

Trajectories::Trajectories()
    : m_gpsPoints(new pcl::PointCloud<GpsPointType>),
      m_searchTree(new pcl::search::FlannSearch<GpsPointType>(false)),
      m_pointVBO(new VertexBufferObjectAttribs),
      m_animateVBO(new VertexBufferObjectAttribs),
      m_renderMode(POINTS),
      m_dataUpdated(false),
      m_animationTime(0.0f)
{
    m_pointVBO->addAttrib(VERTEX_POSITION);
    m_pointVBO->addAttrib(VERTEX_NORMAL);
    m_pointVBO->addAttrib(VERTEX_COLOR);
    m_pointVBO->addAttrib(VERTEX_TEXTURE);

    m_animateVBO->addAttrib(VERTEX_POSITION);
    m_animateVBO->addAttrib(VERTEX_NORMAL);
    m_animateVBO->addAttrib(VERTEX_COLOR);
    m_animateVBO->addAttrib(VERTEX_TEXTURE);

}

Trajectories::~Trajectories(){

}

bool Trajectories::load(const string& filename){
    return loadPBF(filename);
}

bool Trajectories::save(const string& filename){
    return true;
}

void Trajectories::clear(){
    m_dataUpdated = false;
    if(m_gpsPoints->size() > 0)
        m_gpsPoints->clear();

    m_boundBox = Eigen::Vector4f(POSITIVE_INFINITY,
                                 -POSITIVE_INFINITY,
                                 POSITIVE_INFINITY,
                                 -POSITIVE_INFINITY);

    m_carIdx.clear();
    m_timestamp.clear();
    m_lon.clear();
    m_lat.clear();
    m_heading.clear();
    m_speed.clear();
    m_heavy.clear();

    m_indexedTraj.clear();
    m_trajIdx.clear();
    m_sampleIdxInTraj.clear();
    m_easting.clear();
    m_northing.clear();

    m_sortedPointIdx.clear();
}

void Trajectories::update(float delta){
    if(m_renderMode == ANIMATE) { 
        m_dataUpdated = false;
        m_animationTime += delta;
    } 
}

void Trajectories::render(Shader* shader){
    glm::mat4 model(1.0f);

    shader->setMatrix("matModel", model);

    if(!m_dataUpdated){
        prepareForVisualization();
    }

    switch(m_renderMode) { 
        case POINTS: 
            glDisable(GL_DEPTH_TEST);
            glPointSize(20.0f); 
            m_pointVBO->render();
            glPointSize(1.0f); 
            glEnable(GL_DEPTH_TEST);
            break; 
        case LINES:
            break; 
        case ANIMATE:
            glDisable(GL_DEPTH_TEST);
            glPointSize(20.0f); 
            m_animateVBO->render();
            glPointSize(1.0f); 
            glEnable(GL_DEPTH_TEST);
            break;
        default: 
            break; 
    } 
}

/*=====================================================================================
        Update VBOs according to different drawing mode
=====================================================================================*/
void Trajectories::prepareForVisualization(){
    switch(m_renderMode) { 
        case POINTS: 
            updatePointVBO();
            break; 
        case ANIMATE: 
            updateAnimateVBO();
            break;
        default: 
            break; 
    }  
}

void Trajectories::updatePointVBO(){
    vector<VertexBufferObjectAttribs::DATA> vertexData;
    glm::vec4 color(1.0f, 1.0f, 0.0f, 0.3f);

    for(size_t i = 0; i < m_easting.size(); ++i) { 
        VertexBufferObjectAttribs::DATA newPt;
        glm::vec3 normalizedV = BBOXNormalize(m_easting[i], m_northing[i], 0.0);
        newPt.vx = normalizedV.x * 100; 
        newPt.vy = m_speed[i] / 500.0f;
        //newPt.vy = 1.0f;
        newPt.vz = -normalizedV.y * 100;

        float ratio = (m_speed[i] * m_speed[i]) / 1e6;
        if(ratio > 1.0f)
            ratio = 1.0f;
        if(ratio < 0.1f)
            ratio = 0.1f;

        newPt.cx = ratio;
        newPt.cy = 1.0f;
        newPt.cz = 1.0f - ratio;
        newPt.cw = color.w;
        vertexData.push_back(newPt);
    } 
    m_pointVBO->setData(&vertexData[0],
                        GL_STATIC_DRAW,
                        vertexData.size(),
                        GL_POINTS);
    m_pointVBO->bindAttribs();
    m_dataUpdated = true;
}

void Trajectories::updateAnimateVBO(){
    vector<VertexBufferObjectAttribs::DATA> vertexData;
    glm::vec4 color(1.0f, 1.0f, 0.0f, 0.3f);

    float max_deltaT = 0.0f;
    float t_window = 100.0f;

    float cur_t = m_animationTime / 10.0f;
    for(size_t i = 0; i < m_indexedTraj.size(); ++i) { 
        vector<size_t>& traj = m_indexedTraj[i]; 

        float traj_t0 = m_timestamp[traj[0]];
        float dt = 0.0f;
        for(size_t j = 0; j < traj.size(); ++j) { 
            dt = static_cast<float>(m_timestamp[traj[j]]) - traj_t0;
            if(dt < cur_t - t_window)
                continue;

            if(dt > cur_t) 
                break;

            VertexBufferObjectAttribs::DATA newPt;
            glm::vec3 normalizedV = BBOXNormalize(m_easting[traj[j]], m_northing[traj[j]], 0.0);
            newPt.vx = normalizedV.x * 100; 
            //newPt.vy = m_speed[traj[j]] / 500.0f;
            newPt.vy = 1.0f;
            newPt.vz = -normalizedV.y * 100;

            float ratio = (dt - cur_t + t_window) / t_window;
            if(ratio > 1.0f)
                ratio = 1.0f;
            if(ratio < 0)
                ratio = 0.0f;

            newPt.cx = ratio;
            newPt.cy = 1.0f;
            newPt.cz = 1.0f - ratio;
            newPt.cw = color.w;

            vertexData.push_back(newPt);
        } 
        if(dt > max_deltaT) { 
            max_deltaT = dt; 
        } 
    } 

    if(cur_t > max_deltaT) { 
        m_animationTime = 0; 
    } 

    m_animateVBO->setData(&vertexData[0],
                          GL_STATIC_DRAW,
                          vertexData.size(),
                          GL_POINTS);
    m_animateVBO->bindAttribs();
    m_dataUpdated = true;
}
/*=====================================================================================
        End of Updating VBOs
=====================================================================================*/

bool Trajectories::loadPBF(const string& filename){
    clear();

    m_dataUpdated = false;
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    int fid = open(filename.c_str(), O_RDONLY);
    if(fid == -1){
        fprintf(stderr, "ERROR! Cannot open trajectory file!%s\n", filename.c_str());
        return false;
    }

    google::protobuf::io::ZeroCopyInputStream* raw_input = new google::protobuf::io::FileInputStream(fid);
    google::protobuf::io::CodedInputStream* coded_input = new google::protobuf::io::CodedInputStream(raw_input);
    coded_input->SetTotalBytesLimit(1e9, 9e8);

    uint32_t num_trajectory;
    if(!coded_input->ReadLittleEndian32(&num_trajectory)){
        printf("Ooops, something bad happened when reading the trajectory file.\n");
        return false;
    }

    if(num_trajectory > 1e9){
        printf("ERROR: more than 1e9 trajectories in the trajectory file? We cannot handle that...\n");
        return false;
    }

    printf("Start loading trajectories: %u trajectories detected...\n", num_trajectory);

    // Clear existing data
    clock_t t_begin = clock();
    Converter& latlon_converter = Converter::getInstance();
    GpsPointType point;
    // int32_t has min value of -2,147,483,647 to 2,147,483,647
    vector<pair<size_t, uint32_t>> tmp_timestamp;
    for(size_t id_traj = 0; id_traj < num_trajectory; ++id_traj){
        uint32_t msg_length;
        if(!coded_input->ReadLittleEndian32(&msg_length)){
            break; // end of the file
        }
        auto limit = coded_input->PushLimit(msg_length);
        GpsTraj new_traj;
        if(!new_traj.MergePartialFromCodedStream(coded_input)){
            fprintf(stderr, "ERROR: Protobuf trajectory file possibly contaminated!\n");
            return false;
        }
        coded_input->PopLimit(limit);

        // Remove trajectory that has less than 2 points
        if(new_traj.point_size() < 2)
            continue;

        // Process data
        vector<size_t> a_traj;
        for(size_t pt_idx = 0; pt_idx < new_traj.point_size(); ++pt_idx){
            m_carIdx.push_back(new_traj.point(pt_idx).car_id());
            m_timestamp.push_back(new_traj.point(pt_idx).timestamp());
            tmp_timestamp.push_back(pair<size_t, uint32_t>(m_gpsPoints->size(),
                                                         new_traj.point(pt_idx).timestamp()));
            m_lon.push_back(new_traj.point(pt_idx).lon());
            m_lat.push_back(new_traj.point(pt_idx).lat());

            int new_traj_point_head = 450 - new_traj.point(pt_idx).head();
            if (new_traj_point_head > 360) {
                new_traj_point_head -= 360;
            }
            m_heading.push_back( new_traj_point_head );
            m_speed.push_back( new_traj.point(pt_idx).speed() );
            m_heavy.push_back( new_traj.point(pt_idx).heavy() );

            // Derive data
            a_traj.push_back(m_gpsPoints->size());
            m_trajIdx.push_back(id_traj);
            m_sampleIdxInTraj.push_back(pt_idx);
            
                // Compute easting and northing
            float easting, northing;
            double lat = static_cast<double>(new_traj.point(pt_idx).lat()) / 1.0e6;
            double lon = static_cast<double>(new_traj.point(pt_idx).lon()) / 1.0e6;
            latlon_converter.convertLatLonToXY(lat, lon, easting, northing);

            m_easting.push_back(easting);
            m_northing.push_back(northing);

                // Store point
            point.setCoordinate(easting, northing, 0.0f);
            m_gpsPoints->push_back(point);
        }
        m_indexedTraj.push_back(a_traj);
    }

    delete coded_input;
    delete raw_input;
    close(fid);

    // Sort timestamp
    printf("\tsorting points by timestamp ...");
    std::sort(tmp_timestamp.begin(), tmp_timestamp.end(),
            [](const pair<size_t, uint32_t>& a, const pair<size_t, uint32_t>& b) -> bool{
                return a.second < b.second;
            });

    m_sortedPointIdx.resize(tmp_timestamp.size(), 0);
    for(size_t i = 0; i < tmp_timestamp.size(); ++i) { 
        m_sortedPointIdx[i] = tmp_timestamp[i].first;
    } 

    m_minTimestamp = tmp_timestamp.front().second;
    m_maxTimestamp = tmp_timestamp.back().second;
    cout << "min timestamp " << m_minTimestamp << endl;
    cout << "max timestamp " << m_maxTimestamp << endl;
    printf("... Done.\n");

    clock_t t_end = clock();
    double elapsed_secs = double(t_end - t_begin) / CLOCKS_PER_SEC;

    GpsPointType min_pt, max_pt;
    pcl::getMinMax3D(*m_gpsPoints, min_pt, max_pt);

    m_boundBox[0] = min_pt.x;
    m_boundBox[1] = max_pt.x;
    m_boundBox[2] = min_pt.y;
    m_boundBox[3] = max_pt.y;

    // Update Scene Bounding Box
    updateBBOX(min_pt.x, max_pt.x,
               min_pt.y, max_pt.y);
    printf("traj updated bbox: %.2f, %.2f, %.2f, %.2f\n", m_boundBox[0], m_boundBox[1], m_boundBox[2], m_boundBox[3]);

    m_searchTree->setInputCloud(m_gpsPoints);

    printf("Loading complete. Time elapsed: %.1f sec\n", elapsed_secs);
    printf("\t%zu trajectories\t%zu points\n", m_indexedTraj.size(), m_gpsPoints->size());

    time_t start_date = static_cast<time_t>(m_minTimestamp);
    time_t end_date = static_cast<time_t>(m_maxTimestamp);
    printf("\tFrom: %s", ctime(&start_date));
    printf("\tTo:   %s", ctime(&end_date));

    return true;
}

bool Trajectories::savePBF(const string& filename){
    return true;
}
