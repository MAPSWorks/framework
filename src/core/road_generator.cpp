#include "road_generator.h"

#include <pcl/common/centroid.h>
#include <pcl/search/impl/flann_search.hpp>
#include "latlon_converter.h"

RoadGenerator::RoadGenerator(Trajectories* trajectories)
    : m_trajectoris(trajectories),
      m_points(new pcl::PointCloud<MapPointType>),
      m_searchTree(new pcl::search::FlannSearch<MapPointType>(false))
{

}

RoadGenerator::~RoadGenerator(){

}

void RoadGenerator::extractSamples(){
    if(m_trajectoris == nullptr) { 
        cout << "ERROR: RoadGenerator::m_trajectoris is NULL!" << endl;
        return;
    } 

    // Voting
        // Parameters

    // Extract local maxima
    
    // Non-maxima suppression
}

void RoadGenerator::traceRoadSegments(){
    if(m_trajectoris == nullptr) { 
        cout << "ERROR: RoadGenerator::m_trajectoris is NULL!" << endl;
        return;
    }
}

void RoadGenerator::linkRoads(){
    if(m_trajectoris == nullptr) { 
        cout << "ERROR: RoadGenerator::m_trajectoris is NULL!" << endl;
        return;
    }
}
