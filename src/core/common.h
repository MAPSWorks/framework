/*=====================================================================================
                                common.h

    Description:  Common Data Structures used by multiple classes
        
    Created by Chen Chen on 10/20/2015
=====================================================================================*/

#ifndef COMMON_H_I2PM7QS4
#define COMMON_H_I2PM7QS4

// Boost Graph
#include <boost/graph/graph_traits.hpp> 
#include <boost/graph/adjacency_list.hpp> 
#include <boost/property_map/property_map.hpp> 
#include <Eigen/Dense>

static const float POSITIVE_INFINITY = 1e10;
static const float PI                = 3.1415926f;

// Map representation
struct GraphNode{
    float easting;
    float northing;
};

struct GraphEdge{
    float length = POSITIVE_INFINITY;
    int   type   = 0;
};

// Using bidirectionalS because it's easier to get in-edges
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, GraphNode, GraphEdge> graph_t;
typedef boost::graph_traits<graph_t>::vertex_descriptor                                     graph_vertex_descriptor;
typedef boost::graph_traits<graph_t>::edge_descriptor                                       graph_edge_descriptor;

// Utility functions
float distance(float x1, float y1, float x2, float y2);

float deltaHeadingH1toH2(float h1, float h2);
Eigen::Vector2f headingToVector2f(const int);
Eigen::Vector3f headingToVector3f(const int);
int vector2fToHeading(const Eigen::Vector2f);
int vector3fToHeading(const Eigen::Vector3f);

int increaseHeadingBy(int delta, const int orig_h);
int decreaseHeadingBy(int delta, const int orig_h);

#endif /* end of include guard: COMMON_H_I2PM7QS4 */
