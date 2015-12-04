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

class Shader;
class RenderableObject;

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
        void render(unique_ptr<Shader>& shader);
        void prepareForRendering();

        // Clear data
        void clear();

        bool isEmpty();

    public:
        // Publicly available data
        vector<graph_vertex_descriptor>         m_graphVertices;
        Eigen::Vector4f                         m_boundBox; // [minX, maxX, minY, maxY]
        // Indexed Roads
        vector<vector<graph_vertex_descriptor>> m_indexedRoads;

        // Routing graph
        graph_t                                 m_graph;

        // Below is only valid after running interpolateMap()
        pcl::PointCloud<MapPointType>::Ptr      m_mapPoints; 
        vector<int>                             m_pointHeading;
        pcl::search::Search<MapPointType>::Ptr  m_searchTree;
    
    private: 
        float                                   m_interpolation;

        // Rendering
        bool                                    m_dataUpdated;
        unique_ptr<RenderableObject>            m_vboPoints;
        unique_ptr<RenderableObject>            m_vboLines;
}; 

#endif /* end of include guard: OPENSTREETMAP_H_K3GWYIMN */
