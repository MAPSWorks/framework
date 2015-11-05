/*=====================================================================================
                                openstreetmap.h

    Description:  OpenStreetMap
        
    Created by Chen Chen on 10/20/2015
=====================================================================================*/

#ifndef OPENSTREETMAP_H_K3GWYIMN
#define OPENSTREETMAP_H_K3GWYIMN

#include "headers.h"
#include "common.h"

#include <pcl/common/common.h>
#include <pcl/search/search.h>

#include "pcl_wrapper.h"
#include <Eigen/Dense> 

class Shader;
class VertexBufferObjectAttribs;

enum WayType{
    MOTORWAY = 0,
    MOTORWAY_LINK = 1,
    TRUNK = 2,
    TRUNK_LINK = 3,
    PRIMARY = 4,
    PRIMARY_LINK = 5,
    SECONDARY = 6,
    SECONDARY_LINK = 7,
    TERTIARY = 8,
    TERTIARY_LINK = 9,
    OTHER = 10
};

class OpenStreetMap { 
    public: 
        OpenStreetMap(); 
        virtual ~OpenStreetMap(); 

        // IO
        bool load(const string& filename);

        // Interpolate map to produce a point cloud for searching
        void interpolateMap();

        // Rendering
        void render(Shader* shader);
        void prepareForVisualization();

        // Clear data
        void clear();

    public:
        // Publicly available data
        pcl::PointCloud<MapPointType>::Ptr      m_mapPoints;
        vector<int>                             m_pointHeading;
        vector<graph_vertex_descriptor>         m_graphVertex;
        pcl::search::Search<MapPointType>::Ptr  m_searchTree;
        Eigen::Vector4f                         m_boundBox; // [minX, maxX, minY, maxY]
    
    private: 
        float                                   m_interpolation;

        // Indexed Roads
        vector<vector<graph_vertex_descriptor>> m_indexedRoads;

        // Routing graph
        graph_t                                 m_graph; 

        // Rendering
        bool                                    m_dataUpdated;
        unique_ptr<VertexBufferObjectAttribs>   m_pointVBO;
        unique_ptr<VertexBufferObjectAttribs>   m_lineVBO;
}; 

#endif /* end of include guard: OPENSTREETMAP_H_K3GWYIMN */
