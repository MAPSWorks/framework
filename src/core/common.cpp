#include "common.h"
#include <cmath> 
#include <iostream> 

using namespace std;

float distance(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrt(dx*dx + dy*dy);
}

float deltaHeadingH1toH2(float h1, float h2){
    float delta_angle = abs(h1 - h2);
   
    if (delta_angle > 180.0f) {
        delta_angle = 360.0f - delta_angle;
       
        if(h1 > h2){
            return -1.0f * delta_angle;
        }
        else{
            return delta_angle;
        }
    }
    else{
        if(h1 > h2){
            return delta_angle;
        }
        else{
            return -1.0 * delta_angle;
        }
    }
}

Eigen::Vector2f headingToVector2f(const int h){
    if (h < 0 || h > 360) {
        cout << "Error when converting heading to 2d vector. Invalid input: " << h << endl;
        return Eigen::Vector2f(0.0f, 0.0f);
    }
    
    float heading_in_radius = h * PI / 180.0f;
    
    return Eigen::Vector2f(cos(heading_in_radius),
                           sin(heading_in_radius));
}

Eigen::Vector3f headingToVector3f(const int h){
    if (h < 0 || h > 360) {
        cout << "Error when converting heading to 3d vector. Invalid input! Input is: " << h << endl;
        return Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    }
    
    float heading_in_radius = h * PI / 180.0f;
    
    return Eigen::Vector3f(cos(heading_in_radius),
                           sin(heading_in_radius),
                           0.0f);
}

int vector2fToHeading(const Eigen::Vector2f vec){
    Eigen::Vector2f tmp_vec(vec);
   
    float length = tmp_vec.norm();
    
    if(length < 1e-3){
        // very small vector
        cout << "Warning (from vectorToHeading2d): encountered zero vector when converting to heading." << endl;
        return 0;
    }
    
    tmp_vec /= length;
    float cos_value = tmp_vec[0];
    if (cos_value > 1.0f) {
        cos_value = 1.0f;
    }
    if (cos_value < -1.0f) {
        cos_value = -1.0f;
    }
    
    int angle = floor(acos(cos_value) * 180.0f / PI);
    
    if (tmp_vec[1] < 0) {
        angle = 360 - angle;
    }
    
    return angle;
}

int vector3fToHeading(const Eigen::Vector3f vec){
    Eigen::Vector3f tmp_vec(vec);
    
    float length = tmp_vec.norm();
    
    if(length < 1e-3){
        // very small vector
        cout << "Warning (from vectorToHeading3d): encountered zero vector when converting to heading." << endl;
        return 0;
    }

    tmp_vec /= length;
    float cos_value = tmp_vec[0];
    if (cos_value > 1.0f) {
        cos_value = 1.0f;
    }
    if (cos_value < -1.0f) {
        cos_value = -1.0f;
    }
    
    int angle = floor(acos(cos_value) * 180.0f / PI);
    
    if (tmp_vec[1] < 0) {
        angle = 360 - angle;
    }
    
    return angle;
}

int increaseHeadingBy(int delta_heading,
                      const int orig_heading){
    if (orig_heading < 0 || orig_heading > 360) {
        cout << "Error (from increaseHeadingBy). Invalid input.!" << endl;
        return -1;
    }
    
    return (orig_heading + delta_heading + 360) % 360;
}

int decreaseHeadingBy(int delta_heading,
                      const int orig_heading){
    if (orig_heading < 0 || orig_heading > 360) {
        cout << "Error (from decreaseHeadingBy). Invalid input.!" << endl;
        return -1;
    }
    
    return (orig_heading - delta_heading + 360) % 360;
}
