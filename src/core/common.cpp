#include "common.h"
#include <cmath>
#include <iostream>

using namespace std;

float distance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

float deltaHeadingH1toH2(float h1, float h2) {
    // h2 is base. if h1 is counter-clockwise of h2, the result is positive;
    //             else: result is negative (h2->h1: clockwise)
    //             The results is always between 0 to 180
    float delta_angle = abs(h1 - h2);

    if (delta_angle > 180.0f) {
        delta_angle = 360.0f - delta_angle;

        if (h1 > h2) {
            return -1.0f * delta_angle;
        } else {
            return delta_angle;
        }
    } else {
        if (h1 > h2) {
            return delta_angle;
        } else {
            return -1.0 * delta_angle;
        }
    }
}

Eigen::Vector2f headingToVector2f(const float h) {
    if (h < 0.0f || h > 360.0f) {
        cout << "Error when converting heading to 2d vector. Invalid input: "
             << h << endl;
        return Eigen::Vector2f(0.0f, 0.0f);
    }

    float heading_in_radius = h * PI / 180.0f;

    return Eigen::Vector2f(cos(heading_in_radius), sin(heading_in_radius));
}

Eigen::Vector3f headingToVector3f(const float h) {
    if (h < 0.0f || h > 360.0f) {
        cout << "Error when converting heading to 3d vector. Invalid input! "
                "Input is: "
             << h << endl;
        return Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    }

    float heading_in_radius = h * PI / 180.0f;

    return Eigen::Vector3f(cos(heading_in_radius), sin(heading_in_radius),
                           0.0f);
}

Eigen::Vector2d headingToVector2d(const double h) {
    if (h < 0.0f || h > 360.0f) {
        cout << "Error when converting heading to 2d vector. Invalid input: "
             << h << endl;
        return Eigen::Vector2d(0.0f, 0.0f);
    }

    float heading_in_radius = h * PI / 180.0f;

    return Eigen::Vector2d(cos(heading_in_radius), sin(heading_in_radius));
}

Eigen::Vector3d headingToVector3d(const double h) {
    if (h < 0.0f || h > 360.0f) {
        cout << "Error when converting heading to 3d vector. Invalid input! "
                "Input is: "
             << h << endl;
        return Eigen::Vector3d(0.0f, 0.0f, 0.0f);
    }

    float heading_in_radius = h * PI / 180.0f;

    return Eigen::Vector3d(cos(heading_in_radius), sin(heading_in_radius),
                           0.0f);
}

float vector2fToHeading(const Eigen::Vector2f vec) {
    Eigen::Vector2f tmp_vec(vec);

    float length = tmp_vec.norm();

    if (length < 1e-3) {
        // very small vector
        cout << "Warning (from vectorToHeading2d): encountered zero vector "
                "when converting to heading."
             << endl;
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

    float angle = acos(cos_value) * 180.0f / PI;

    if (tmp_vec[1] < 0.0f) {
        angle = 360.0f - angle;
    }

    return angle;
}

float vector3fToHeading(const Eigen::Vector3f vec) {
    Eigen::Vector3f tmp_vec(vec);

    float length = tmp_vec.norm();

    if (length < 1e-3) {
        // very small vector
        cout << "Warning (from vectorToHeading3d): encountered zero vector "
                "when converting to heading."
             << endl;
        return 0.0f;
    }

    tmp_vec /= length;
    float cos_value = tmp_vec[0];
    if (cos_value > 1.0f) {
        cos_value = 1.0f;
    }
    if (cos_value < -1.0f) {
        cos_value = -1.0f;
    }

    float angle = acos(cos_value) * 180.0f / PI;

    if (tmp_vec[1] < 0) {
        angle = 360.0f - angle;
    }

    return angle;
}

double vector2dToHeading(const Eigen::Vector2d vec) {
    Eigen::Vector2d tmp_vec(vec);

    double length = tmp_vec.norm();

    if (length < 1e-3) {
        // very small vector
        cout << "Warning (from vectorToHeading2d): encountered zero vector "
                "when converting to heading."
             << endl;
        return 0;
    }

    tmp_vec /= length;
    double cos_value = tmp_vec[0];
    if (cos_value > 1.0f) {
        cos_value = 1.0f;
    }
    if (cos_value < -1.0f) {
        cos_value = -1.0f;
    }

    double angle = acos(cos_value) * 180.0f / PI;

    if (tmp_vec[1] < 0) {
        angle = 360 - angle;
    }

    return angle;
}

double vector3dToHeading(const Eigen::Vector3d vec) {
    Eigen::Vector3d tmp_vec(vec);

    double length = tmp_vec.norm();

    if (length < 1e-3) {
        // very small vector
        cout << "Warning (from vectorToHeading3d): encountered zero vector "
                "when converting to heading."
             << endl;
        return 0;
    }

    tmp_vec /= length;
    double cos_value = tmp_vec[0];
    if (cos_value > 1.0f) {
        cos_value = 1.0f;
    }
    if (cos_value < -1.0f) {
        cos_value = -1.0f;
    }

    double angle = acos(cos_value) * 180.0f / PI;

    if (tmp_vec[1] < 0) {
        angle = 360 - angle;
    }

    return angle;
}

float increaseHeadingBy(float delta_heading, const float orig_heading) {
    // Counter-clockwise: delta_heading > 0
    // Clockwise: delta_heading < 0
    if (orig_heading < 0 || orig_heading > 360) {
        cout << "Error (from increaseHeadingBy). Invalid input.!" << endl;
        return -1;
    }

    return fmod(orig_heading + delta_heading + 360, 360);
}

float decreaseHeadingBy(float delta_heading, const float orig_heading) {
    if (orig_heading < 0 || orig_heading > 360) {
        cout << "Error (from decreaseHeadingBy). Invalid input.!" << endl;
        return -1;
    }

    return fmod(orig_heading - delta_heading + 360, 360);
}

void peakDetector(vector<float>& hist, int window, float ratio,
                  vector<int>& peak_idxs, bool is_closed) {
    // Detecting peaks in a histogram
    // is_closed: true - loop around; false: do not loop around.
    // ratio should be >= 1.0f
    peak_idxs.clear();

    vector<pair<int, float>> raw_peak_idxs;
    int left_offset = window / 2;
    int right_offset = window - left_offset;

    if (is_closed) {
        for (int i = 0; i < hist.size(); ++i) {
            int start_idx = i - left_offset;
            int right_idx = i + right_offset;

            bool is_max = true;
            float min_value = 1e6;
            float avg_value = 0.0;
            int count = 0;
            for (int j = start_idx; j <= right_idx; ++j) {
                if (j == i) continue;

                int hist_idx = j;
                if (hist_idx < 0) {
                    hist_idx += hist.size();
                }
                if (hist_idx >= hist.size()) {
                    hist_idx = hist_idx % hist.size();
                }
                if (hist[hist_idx] > hist[i]) {
                    is_max = false;
                    break;
                }
                if (hist[hist_idx] < min_value) {
                    min_value = hist[hist_idx];
                }
                avg_value += hist[hist_idx];
                count += 1;
            }

            if (is_max) {
                if (count == 0) {
                    continue;
                }
                avg_value /= count;
                float d1 = hist[i] - min_value;
                float d2 = avg_value - min_value;

                if (d1 > ratio * d2) {
                    raw_peak_idxs.push_back(pair<int, float>(i, hist[i]));
                }
            }
        }
    } else {
        for (int i = 0; i < hist.size(); ++i) {
            int start_idx = i - left_offset;
            int right_idx = i + right_offset;
            if (start_idx < 0) {
                start_idx = 0;
            }
            if (right_idx >= hist.size()) {
                right_idx = hist.size() - 1;
            }

            bool is_max = true;
            float min_value = 1e6;
            float avg_value = 0.0;
            int count = 0;
            for (int j = start_idx; j <= right_idx; ++j) {
                if (i == j) continue;

                int hist_idx = j;

                if (hist[hist_idx] > hist[i]) {
                    is_max = false;
                    break;
                }

                if (hist[hist_idx] < min_value) {
                    min_value = hist[hist_idx];
                }
                avg_value += hist[hist_idx];
                count += 1;
            }

            if (is_max) {
                if (count == 0) {
                    continue;
                }
                avg_value /= count;
                float d1 = hist[i] - min_value;
                float d2 = avg_value - min_value;

                if (d1 > ratio * d2) {
                    raw_peak_idxs.push_back(pair<int, float>(i, hist[i]));
                }
            }
        }
    }

    std::sort(raw_peak_idxs.begin(), raw_peak_idxs.end(),
              [](const pair<int, float>& a, const pair<int, float>& b) -> bool {
                  return a.second > b.second;
              });
    for (int i = 0; i < raw_peak_idxs.size(); ++i) {
        if (i > 0) {
            int delta_to_previous =
                abs(raw_peak_idxs[i - 1].first - raw_peak_idxs[i].first);
            if (is_closed) {
                if (delta_to_previous > 0.5f * hist.size()) {
                    delta_to_previous = hist.size() - delta_to_previous;
                }
            }
            if (delta_to_previous > 0.5f * window) {
                peak_idxs.emplace_back(raw_peak_idxs[i].first);
            }
        } else {
            peak_idxs.emplace_back(raw_peak_idxs[i].first);
        }
    }

    // Debug
    // if (!is_closed) {
    //    cout << "hist: "<<endl;
    //    cout << "\t";
    //    for (size_t i = 0; i < hist.size(); ++i) {
    //        cout << hist[i] << ", ";
    //    }
    //    cout << endl;
    //    cout << "\t";
    //    for (size_t i = 0; i < peak_idxs.size(); ++i) {
    //        cout << peak_idxs[i] << ", ";
    //    }
    //    cout << endl;
    //    cout << endl;
    //}
}

// Intersection of two line segments: p11->p12 and p21->p22
Eigen::Vector2d lineSegmentIntersection(Eigen::Vector2d p11,
                                        Eigen::Vector2d p12,
                                        Eigen::Vector2d p21,
                                        Eigen::Vector2d p22) {
    // Line equations
    double a1 = p12[1] - p11[1];
    double b1 = p11[0] - p12[0];
    double c1 = p12[0] * p11[1] - p11[0] * p12[1];

    double a2 = p22[1] - p21[1];
    double b2 = p21[0] - p22[0];
    double c2 = p22[0] * p21[1] - p21[0] * p22[1];

    double denominator = a1 * b2 - a2 * b1;

    if (abs(denominator) < 1e-5) {
        return Eigen::Vector2d(POSITIVE_INFINITY, POSITIVE_INFINITY);
    } else {
        double x = (b1 * c2 - b2 * c1) / denominator;
        double y = (a2 * c1 - a1 * c2) / denominator;
        return Eigen::Vector2d(x, y);
    }
}

glm::vec3 convertToDisplayCoord(float easting, float northing, float yCoord){
    glm::vec3 normalized_v = BBOXNormalize(easting, northing, 0.0f);
    glm::vec3 res(normalized_v.x * params::inst().scale,
                  yCoord,
                  -normalized_v.y * params::inst().scale);
    return res;
}
