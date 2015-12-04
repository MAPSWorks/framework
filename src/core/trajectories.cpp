#include "trajectories.h"

#include "shader.h"
#include "renderable_object.h"

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
      m_vboPoints(new RenderableObject),
      m_vboAnimation(new RenderableObject),
      m_renderMode(POINTS),
      m_animationTime(0.0f)
{
}

Trajectories::~Trajectories(){

}

bool Trajectories::load(const string& filename){
    return loadPBF(filename);
}

bool Trajectories::save(const string& filename){
    if (savePBF(filename)){
        printf("%s saved.\n", filename.c_str());
    }
    else{
        printf("File cannot be saved.");
    }
    return true;
}

void Trajectories::clear(){
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

bool Trajectories::isEmpty(){
    if(m_gpsPoints->size() == 0)
        return true;

    return false;
}

void Trajectories::update(float delta){
    if(m_renderMode == ANIMATE) { 
        m_animationTime += delta;
    } 
}

void Trajectories::render(Shader* shader){
    glm::mat4 model(1.0f);

    shader->setMatrix("matModel", model);

    switch(m_renderMode) { 
        case POINTS: 
            glPointSize(20.0f); 
            m_vboPoints->render();
            glPointSize(1.0f); 
            break; 
        case LINES:
            break; 
        case ANIMATE:
            glPointSize(20.0f); 
            m_vboAnimation->render();
            glPointSize(1.0f); 
            break;
        default: 
            break; 
    } 
}

/*=====================================================================================
        Update VBOs according to different drawing mode
=====================================================================================*/
void Trajectories::prepareForRendering(){
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
    vector<RenderableObject::Vertex> vertexData;
    glm::vec4 color(1.0f, 1.0f, 0.0f, 0.3f);

    float scale = params::inst().scale;
    for(size_t i = 0; i < m_easting.size(); ++i) { 
        RenderableObject::Vertex newPt;
        glm::vec3 normalizedV = BBOXNormalize(m_easting[i], m_northing[i], 0.0);
        newPt.Position = glm::vec3(normalizedV.x * scale,
                                   1.0f,
                                   -normalizedV.y * scale);

        float ratio = (m_speed[i] * m_speed[i]) / 1e6;
        if(ratio > 1.0f)
            ratio = 1.0f;
        if(ratio < 0.1f)
            ratio = 0.1f;
        newPt.Color = glm::vec4(ratio, 1.0f, 1.0f - ratio, color.w);

        vertexData.push_back(newPt);
    } 
    m_vboPoints->setData(vertexData,
                        GL_POINTS);
}

void Trajectories::updateAnimateVBO(){
    vector<RenderableObject::Vertex> vertexData;
    glm::vec4 color(1.0f, 1.0f, 0.0f, 0.3f);

    float max_deltaT = 0.0f;
    float t_window = 100.0f;

    float cur_t = m_animationTime / 10.0f;
    float scale = params::inst().scale;
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

            RenderableObject::Vertex newPt;
            glm::vec3 normalizedV = BBOXNormalize(m_easting[traj[j]], m_northing[traj[j]], 0.0);
            newPt.Position = glm::vec3(normalizedV.x * scale,
                                   1.0f,
                                   -normalizedV.y * scale);

            float ratio = (dt - cur_t + t_window) / t_window;
            if(ratio > 1.0f)
                ratio = 1.0f;
            if(ratio < 0)
                ratio = 0.0f;

            newPt.Color = glm::vec4(ratio, 1.0f, 1.0f - ratio, color.w);
            vertexData.push_back(newPt);
        } 
        if(dt > max_deltaT) { 
            max_deltaT = dt; 
        } 
    } 

    if(cur_t > max_deltaT) { 
        m_animationTime = 0; 
    } 

    m_vboAnimation->setData(vertexData,
                          GL_POINTS);
}
/*=====================================================================================
        End of Updating VBOs
=====================================================================================*/

bool Trajectories::loadPBF(const string& filename){
    clear();

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

            // Derived data
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
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    
    // Read the trajectory collection from file.
    int fid = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
    
    if (fid == -1) {
        fprintf(stderr, "ERROR! Cannot create protobuf trajectory file!\n");
        return false;
    }
    
    google::protobuf::io::ZeroCopyOutputStream *raw_output = new google::protobuf::io::FileOutputStream(fid);
    google::protobuf::io::CodedOutputStream *coded_output = new google::protobuf::io::CodedOutputStream(raw_output);
    
    uint32_t num_trajectory = m_indexedTraj.size();
    
    coded_output->WriteLittleEndian32(num_trajectory);
    
    for (size_t id_traj = 0; id_traj < num_trajectory; ++id_traj) {
        GpsTraj new_traj;
        for (size_t k = 0; k < m_indexedTraj[id_traj].size(); ++k) {
            TrajPoint* new_pt = new_traj.add_point();

            size_t ptIdx = m_indexedTraj[id_traj][k];
            size_t carIdx = m_carIdx[ptIdx];
            uint32_t timestamp = m_timestamp[ptIdx];
            int32_t lon = m_lon[ptIdx];
            int32_t lat = m_lat[ptIdx];
            int32_t heading = m_heading[ptIdx];
            int32_t speed = m_speed[ptIdx];
            bool    heavy = m_heavy[ptIdx];

            new_pt->set_car_id(carIdx);
            new_pt->set_speed(speed);
            
            int old_head = 450 - heading;
            if (old_head > 360) {
                old_head -= 360;
            }
            
            new_pt->set_head(old_head);
            new_pt->set_lon(lon);
            new_pt->set_lat(lat);
            new_pt->set_timestamp(timestamp);
            new_pt->set_heavy(heavy);
        }
        string s;
        new_traj.SerializeToString(&s);
        coded_output->WriteLittleEndian32(s.size());
        coded_output->WriteString(s);
    }
    delete coded_output;
    delete raw_output;
    close(fid);
    
    return true;
}

// Extract trajectories from multiple files
bool Trajectories::extractFromMultipleFiles(const QStringList& filenames,
                                            Eigen::Vector4f    boundbox,
                                            int minNumPt){
    clear(); 

    // Check bounding box
    if(boundbox[1] < boundbox[0] || boundbox[3] < boundbox[2]) { 
        fprintf(stderr, "Trajectories::extractFromMultipleFiles: Bounding Box ERROR.\n\t(min_easting = %.2f, max_easting = %.2f, min_northing = %.2f, max_northing = %.2f)\n", boundbox[0], boundbox[1], boundbox[2], boundbox[3]);
        return false;
    } 

    GpsPointType point;
    vector<pair<size_t, uint32_t>> tmp_timestamp;
    size_t id_traj = 0;
    for(size_t i = 0; i < filenames.size(); ++i) { 
        string filename(filenames.at(i).toLocal8Bit().constData()) ;
        printf("\tExtracting trajectories from %s\n", filename.c_str());
        Trajectories *new_trajectories = new Trajectories();
        new_trajectories->load(filename);

        // Iterate over the trajectories
        for(size_t j = 0; j < new_trajectories->m_indexedTraj.size(); ++j) { 
            vector<size_t> traj = new_trajectories->m_indexedTraj[j];
            bool recording = false;

            // Traverse the trajectory and chop into segments
            vector<size_t> candidate_traj;
            for(size_t k = 0; k < traj.size(); ++k) { 
                size_t ptIdx = traj[k]; 
                float easting = new_trajectories->m_easting[ptIdx];
                float northing = new_trajectories->m_northing[ptIdx];
                if(easting < boundbox[1] && easting > boundbox[0] &&
                        northing < boundbox[3] && northing > boundbox[2]) { 
                    // Point is inside the query boundbox
                    if(!recording) { 
                        recording = true; 
                        candidate_traj.clear();
                        candidate_traj.push_back(ptIdx);
                    }
                    else{
                        // Insert point
                        candidate_traj.push_back(ptIdx);
                    } 
                } 
                else{
                    // Point is outside the query boundbox
                    if(recording) { 
                        recording = false; 
                        if(candidate_traj.size() > minNumPt) { 
                            // Insert the candidate trajectory
                            vector<size_t> a_traj;
                            for(size_t s = 0; s < candidate_traj.size(); ++s) { 
                                size_t ptIdx = candidate_traj[s];
                                size_t carIdx = new_trajectories->m_carIdx[ptIdx];
                                uint32_t timestamp = new_trajectories->m_timestamp[ptIdx];
                                int32_t lon = new_trajectories->m_lon[ptIdx];
                                int32_t lat = new_trajectories->m_lat[ptIdx];
                                int32_t heading = new_trajectories->m_heading[ptIdx];
                                int32_t speed = new_trajectories->m_speed[ptIdx];
                                bool    heavy = new_trajectories->m_heavy[ptIdx];
                                float easting = new_trajectories->m_easting[ptIdx];
                                float northing = new_trajectories->m_northing[ptIdx];

                                m_carIdx.push_back(carIdx);
                                m_timestamp.push_back(timestamp);
                                tmp_timestamp.push_back(pair<size_t, uint32_t>(m_gpsPoints->size(),
                                                                               timestamp));
                                m_lon.push_back(lon);
                                m_lat.push_back(lat);

                                m_heading.push_back(heading);
                                m_speed.push_back(speed);
                                m_heavy.push_back(heavy);

                                // Derived data
                                a_traj.push_back(m_gpsPoints->size());
                                m_trajIdx.push_back(id_traj);
                                m_sampleIdxInTraj.push_back(s);
                                
                                m_easting.push_back(easting);
                                m_northing.push_back(northing);

                                    // Store point
                                point.setCoordinate(easting, northing, 0.0f);
                                m_gpsPoints->push_back(point); 
                            } 
                            m_indexedTraj.push_back(a_traj);
                            id_traj++;
                            candidate_traj.clear();
                        } 
                    } 
                }
            } 

            if(recording) { 
                if(candidate_traj.size() > minNumPt) { 
                    // Insert the candidate trajectory
                    vector<size_t> a_traj;
                    for(size_t s = 0; s < candidate_traj.size(); ++s) { 
                        size_t ptIdx = candidate_traj[s];
                        size_t carIdx = new_trajectories->m_carIdx[ptIdx];
                        uint32_t timestamp = new_trajectories->m_timestamp[ptIdx];
                        int32_t lon = new_trajectories->m_lon[ptIdx];
                        int32_t lat = new_trajectories->m_lat[ptIdx];
                        int32_t heading = new_trajectories->m_heading[ptIdx];
                        int32_t speed = new_trajectories->m_speed[ptIdx];
                        bool    heavy = new_trajectories->m_heavy[ptIdx];
                        float easting = new_trajectories->m_easting[ptIdx];
                        float northing = new_trajectories->m_northing[ptIdx];

                        m_carIdx.push_back(carIdx);
                        m_timestamp.push_back(timestamp);
                        tmp_timestamp.push_back(pair<size_t, uint32_t>(m_gpsPoints->size(),
                                                                       timestamp));
                        m_lon.push_back(lon);
                        m_lat.push_back(lat);

                        m_heading.push_back(heading);
                        m_speed.push_back(speed);
                        m_heavy.push_back(heavy);

                        // Derived data
                        a_traj.push_back(m_gpsPoints->size());
                        m_trajIdx.push_back(id_traj);
                        m_sampleIdxInTraj.push_back(s);
                        
                        m_easting.push_back(easting);
                        m_northing.push_back(northing);

                            // Store point
                        point.setCoordinate(easting, northing, 0.0f);
                        m_gpsPoints->push_back(point); 
                    } 
                    m_indexedTraj.push_back(a_traj);
                    id_traj++;
                    candidate_traj.clear();
                }
            }
        } 

        delete new_trajectories;
    } 

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

    GpsPointType min_pt, max_pt;
    pcl::getMinMax3D(*m_gpsPoints, min_pt, max_pt);

    m_boundBox[0] = min_pt.x;
    m_boundBox[1] = max_pt.x;
    m_boundBox[2] = min_pt.y;
    m_boundBox[3] = max_pt.y;

    // Update Scene Bounding Box
    resetBBOX(); 
    updateBBOX(min_pt.x, max_pt.x,
               min_pt.y, max_pt.y);
    printf("Totally %lu trajectories, %lu points.\n", m_indexedTraj.size(), m_gpsPoints->size());

    m_searchTree->setInputCloud(m_gpsPoints);

    time_t start_date = static_cast<time_t>(m_minTimestamp);
    time_t end_date = static_cast<time_t>(m_maxTimestamp);
    printf("\tFrom: %s", ctime(&start_date));
    printf("\tTo:   %s", ctime(&end_date));

    return true;
}
