#include "pcl_wrapper.h"

#include <pcl/impl/pcl_base.hpp>
#include <pcl/search/impl/kdtree.hpp>
#include <pcl/search/impl/search.hpp>
#include <pcl/search/impl/organized.hpp>
#include <pcl/search/impl/flann_search.hpp>
#include <pcl/kdtree/impl/kdtree_flann.hpp>

template class pcl::KdTreeFLANN<pcl::PointXYZ>;
template class pcl::search::KdTree<pcl::PointXYZ>;
template class pcl::search::Search<pcl::PointXYZ>;
template class pcl::search::FlannSearch<pcl::PointXYZ>;
template class pcl::search::OrganizedNeighbor<pcl::PointXYZ>;

/*
 *For GPS Point
 */
template class pcl::KdTreeFLANN<GpsPointType>;
template class pcl::search::KdTree<GpsPointType>;
template class pcl::search::Search<GpsPointType>;
template class pcl::search::FlannSearch<GpsPointType>;
template class pcl::search::OrganizedNeighbor<GpsPointType>;

/*
 *For Map Point
 */
template class pcl::KdTreeFLANN<MapPointType>;
template class pcl::search::KdTree<MapPointType>;
template class pcl::search::Search<MapPointType>;
template class pcl::search::FlannSearch<MapPointType>;
template class pcl::search::OrganizedNeighbor<MapPointType>;
