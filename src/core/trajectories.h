/*=====================================================================================
                                trajectories.h
    Description:  Trajectory Containers for GPS Trajectories
        
    Created by Chen Chen on 10/12/2015
=====================================================================================*/

#ifndef TRAJECTORIES_H_
#define TRAJECTORIES_H_

#include "headers.h"

#include <pcl/common/common.h>
#include <pcl/search/search.h>

#include "pcl_wrapper.h"
#include <Eigen/Dense> 

class Shader;
class RenderableObject;

class Trajectories { 
    public: 
        enum DrawMode{
            POINTS,
            LINES,
            ANIMATE   
        };

        Trajectories(); 
        virtual ~Trajectories(); 

        // IO
        bool load(const string& filename);
        bool save(const string& filename);

        // Extract from files
        // boundbox: (min_easting, max_easting, min_northing, max_northing)
        bool extractFromMultipleFiles(const QStringList& filenames,
                                      Eigen::Vector4f    boundbox,
                                      int                minNumPt = 3);
        
        // Rendering
        void render(unique_ptr<Shader>& shader);
        void prepareForRendering();
        void updatePointVBO();
        void updateAnimateVBO();
        void update(float delta); // delta in milliseconds

        // Clear data
        void clear();

        bool isEmpty();

    public:
        // Data that can be accessible from outside
        
        // Point cloud and search tree
        pcl::PointCloud<GpsPointType>::Ptr       m_gpsPoints;
        pcl::search::Search<GpsPointType>::Ptr   m_searchTree;
        Eigen::Vector4f                          m_boundBox; // [minX, maxX, minY, maxY]

        // Two indexing scheme:
        //  - Original indexing: according to the sequence of reading data from file.
        //  - Sorted indexing: according to sorting points by timestamp.
        
        // Raw data
        vector<size_t>                           m_carIdx;
        vector<uint32_t>                         m_timestamp;
        vector<int32_t>                          m_lon;
        vector<int32_t>                          m_lat;
        vector<int32_t>                          m_heading; // in degrees
        vector<int32_t>                          m_speed;   // in cm/s
        vector<bool>                             m_heavy;

        // Derived data 
        vector<vector<size_t>>                   m_indexedTraj;
        vector<size_t>                           m_trajIdx;
        vector<size_t>                           m_sampleIdxInTraj;
        vector<float>                            m_easting;
        vector<float>                            m_northing;

        vector<size_t>                           m_sortedPointIdx;  // by timestamp

    private: 
        bool loadPBF(const string& filename);
        bool savePBF(const string& filename);

        // Indexed trajectories
        uint32_t                                 m_minTimestamp; // minimum timestamp
        uint32_t                                 m_maxTimestamp; // maximum timestamp

        // Rendering
        DrawMode                                 m_renderMode;
        unique_ptr<RenderableObject>             m_vboPoints;
        unique_ptr<RenderableObject>             m_vboAnimation;

        // Animation
        float                                    m_animationTime;
}; 

#endif /* end of include guard: TRAJECTORIES_H_ */
