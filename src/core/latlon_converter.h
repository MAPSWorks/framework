/*=====================================================================================
                                latlon_converter.h
    Description:  Convert lat / lon to easting / northing
        
    Created by Chen Chen on 10/12/2015
=====================================================================================*/


#ifndef LATLON_CONVERTER_H_
#define LATLON_CONVERTER_H_

#include <proj_api.h>
#include <cstdio>
using namespace std;

static int32_t UTC_OFFSET = 1241100000;

static const char *BJ_DST_PROJ = "+proj=utm +zone=50 +south=False +ellps=WGS84";
static const char *BJ_SRC_PROJ = "+proj=latlon +ellps=WGS84";

static const char *SF_DST_PROJ = "+proj=utm +zone=10 +south=False +ellps=WGS84";
static const char *SF_SRC_PROJ = "+proj=latlon +ellps=WGS84";

enum class UtmRegion{
    Beijing = 0,
    California = 1
};

class Converter{
    public:
        static Converter& getInstance(){
            static Converter singleton;
            return singleton;
        }

        void setUTMZone(UtmRegion zone);
        void convertLatLonToXY(float, float, float&, float&);

    private:
        Converter();
        virtual ~Converter(){}

        projPJ      src_proj_;
        projPJ      dst_proj_;
};

#endif
