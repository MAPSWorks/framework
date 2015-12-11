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
#include "headers.h"

using namespace std;

static const float POSITIVE_INFINITY = 1e10;
static const float PI = 3.1415926f;

const string defaultDataDir = "/Users/chenchen/Research/data";
const string defaultTrajDir = "/Users/chenchen/Research/data/trajectories";
const string defaultOSMDir = "/Users/chenchen/Research/data/openstreetmap";

// Map representation
struct GraphNode {
    float easting;
    float northing;
};

struct GraphEdge {
    float length = POSITIVE_INFINITY;
    int type = 0;
};

// Using bidirectionalS because it's easier to get in-edges
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                              GraphNode, GraphEdge> graph_t;
typedef boost::graph_traits<graph_t>::vertex_descriptor graph_vertex_descriptor;
typedef boost::graph_traits<graph_t>::edge_descriptor graph_edge_descriptor;

// Utility functions
float distance(float x1, float y1, float x2, float y2);

float deltaHeadingH1toH2(float h1, float h2);
Eigen::Vector2f headingToVector2f(const float);
Eigen::Vector3f headingToVector3f(const float);
Eigen::Vector2d headingToVector2d(const double);
Eigen::Vector3d headingToVector3d(const double);
float vector2fToHeading(const Eigen::Vector2f);
float vector3fToHeading(const Eigen::Vector3f);
double vector2dToHeading(const Eigen::Vector2d);
double vector3dToHeading(const Eigen::Vector3d);

float increaseHeadingBy(float delta, const float orig_h);
float decreaseHeadingBy(float delta, const float orig_h);

void peakDetector(vector<float>& hist, int window, float ratio,
                  vector<int>& peak_idxs, bool is_closed = false);

// Intersection of two line segments: p11->p12 and p21->p22
Eigen::Vector2d lineSegmentIntersection(Eigen::Vector2d p11,
                                        Eigen::Vector2d p12,
                                        Eigen::Vector2d p21,
                                        Eigen::Vector2d p22);

// Convert (easting, northing, 0.0) to display coordinate
// easting   -> x
// yCoord    -> y
// -northing -> z
glm::vec3 convertToDisplayCoord(float easting, float northing, float yCoord);

#endif /* end of include guard: COMMON_H_I2PM7QS4 */
