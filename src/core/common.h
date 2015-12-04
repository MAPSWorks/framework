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
#include "color.h"

using namespace std;

static const float POSITIVE_INFINITY = 1e10;
static const float PI                = 3.1415926f;

const string defaultDataDir = "/Users/chenchen/Research/data";
const string defaultTrajDir = "/Users/chenchen/Research/data/trajectories";
const string defaultOSMDir = "/Users/chenchen/Research/data/openstreetmap";

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
Eigen::Vector2d headingToVector2d(const int);
Eigen::Vector3d headingToVector3d(const int);
int vector2fToHeading(const Eigen::Vector2f);
int vector3fToHeading(const Eigen::Vector3f);
int vector2dToHeading(const Eigen::Vector2d);
int vector3dToHeading(const Eigen::Vector3d);

int increaseHeadingBy(int delta, const int orig_h);
int decreaseHeadingBy(int delta, const int orig_h);

void peakDetector(vector<float>& hist, int window, float ratio, vector<int>& peak_idxs,bool is_closed = false);

// Intersection of two line segments: p11->p12 and p21->p22
Eigen::Vector2d lineSegmentIntersection(Eigen::Vector2d p11, Eigen::Vector2d p12,
                                        Eigen::Vector2d p21, Eigen::Vector2d p22);

#endif /* end of include guard: COMMON_H_I2PM7QS4 */
