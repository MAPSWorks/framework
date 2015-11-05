#include "latlon_converter.h"

Converter::Converter(){
    setUTMZone(UtmRegion::California);
}

void Converter::setUTMZone(UtmRegion zone){
    const char* src_proj_str;
    const char* dst_proj_str;

    switch (zone) {
        case UtmRegion::Beijing:
            src_proj_str = BJ_SRC_PROJ;
            dst_proj_str = BJ_DST_PROJ;        
            break;
        case UtmRegion::California:
            src_proj_str = SF_SRC_PROJ;
            dst_proj_str = SF_DST_PROJ;
            break;
        default:
            break;
    }

    if (!(src_proj_ = pj_init_plus(SF_SRC_PROJ))) {
        fprintf(stderr, "Error! Cannot initialize latlon to XY projector!\n");
        exit(1);
    }
    if (!(dst_proj_ = pj_init_plus(SF_DST_PROJ))) {
        fprintf(stderr, "Error! Cannot initialize latlon to XY projector!\n");
        exit(1);
    }
}

void Converter::convertLatLonToXY(float lat, float lon, float& x, float&y){
    double tmp_x = lon * DEG_TO_RAD;
    double tmp_y = lat * DEG_TO_RAD;
    pj_transform(src_proj_, dst_proj_, 1, 1, &tmp_x, &tmp_y, NULL);
    x = tmp_x;
    y = tmp_y;
}
