#include "math_utils.h"

float d2(float x1, float x2, float y1, float y2) {
    float dx = std::pow(x2-x1,2);
    float dy = std::pow(y2-y1,2);
    return std::pow(dx+dy,0.5);
}