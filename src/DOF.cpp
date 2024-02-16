#include "DOF.h"

DOF::DOF() {
    value = 0.0f;
    min = -6.28318f;
    max = 6.28318f;
}

DOF::~DOF() {
}

void DOF::SetValue(float v) {
    if (v < min) {
        value = min;
    } else if (v > max) {
        value = max;
    } else {
        value = v;
    }
}

float DOF::GetValue() {
    return value;
}

void DOF::SetMinMax(float min, float max) {
    this->min = min;
    this->max = max;
}