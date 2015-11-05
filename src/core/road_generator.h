/*=====================================================================================
                                road_generator.h

    Description:  Generate road network from GPS trajectories.
        
    Created by Chen Chen on 10/19/2015
=====================================================================================*/

#ifndef ROAD_GENERATOR_H_IM1C8TR0
#define ROAD_GENERATOR_H_IM1C8TR0

#include "headers.h"

#include "trajectories.h"

class RoadGenerator { 
    public: 
        RoadGenerator(Trajectories* trajectories = nullptr); 
        virtual ~RoadGenerator(); 

        void extractSamples();
        void traceRoadSegments();
        void linkRoads();

    private: 
        // Point cloud and search tree
        pcl::PointCloud<MapPointType>::Ptr       m_points;
        pcl::search::Search<MapPointType>::Ptr   m_searchTree;

        Trajectories*                            m_trajectoris;
}; 

#endif /* end of include guard: ROAD_GENERATOR_H_IM1C8TR0 */
