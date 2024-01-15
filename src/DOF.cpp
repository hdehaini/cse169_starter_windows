#include "../include/DOF.h"

DOF::DOF() {
    value = 0.0f;
    min = -360.0f;
    max = 360.0f;
}

DOF::~DOF() {
}

void DOF::SetValue(float v) {
    value = v;
}

float DOF::GetValue() {
    return value;
}

void DOF::SetMinMax(float min, float max) {
    this->min = min;
    this->max = max;
}