/*=====================================================================================
                                pcl_wrapper.h
    Description:  Wrapper for PCL Point Cloud
        
    Created by Chen Chen on 10/12/2015
=====================================================================================*/

#ifndef PCL_WRAPPER_H
#define PCL_WRAPPER_H

#define PCL_NO_PRECOMPILE
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>

/*
 * GPS Point
 */
struct _GpsPointType{
    PCL_ADD_POINT4D;                  // preferred way of adding a XYZ+padding

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW   // make sure our new allocators are aligned
} EIGEN_ALIGN16;

/*
 *This is the GpsPointType used throughout the project with some initialization functions.
 */
struct GpsPointType : public _GpsPointType{
    static const int ID_UNINITIALIZED = -1;
    inline GpsPointType ()
	{
		x = y = z = 0.0f;
	}

    inline void setCoordinate(float _x, float _y, float _z){
        x = _x;
        y = _y;
        z = _z;
    }
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
} EIGEN_ALIGN16;

POINT_CLOUD_REGISTER_POINT_STRUCT (GpsPointType,
                                    (float, x, x)
                                    (float, y, y)
                                    (float, z, z)
)

POINT_CLOUD_REGISTER_POINT_WRAPPER(GpsPointType, _GpsPointType)

/*
 *Map Point
 */
struct _MapPointType{
    PCL_ADD_POINT4D;                  // preferred way of adding a XYZ+padding

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW   // make sure our new allocators are aligned
} EIGEN_ALIGN16;

/*
 *This is the MapPointType used throughout the project with some initialization functions.
 */
struct MapPointType : public _MapPointType{
    static const int ID_UNINITIALIZED = -1;
    inline MapPointType ()
	{
		x = y = z = 0.0f;
	}

    inline void setCoordinate(float _x, float _y, float _z){
        x = _x;
        y = _y;
        z = _z;
    }
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
} EIGEN_ALIGN16;

POINT_CLOUD_REGISTER_POINT_STRUCT (MapPointType,
                                   (float, x, x)
                                   (float, y, y)
                                   (float, z, z)
)

POINT_CLOUD_REGISTER_POINT_WRAPPER(MapPointType, _MapPointType)

/*
 *End of Map Point
 */

#endif
