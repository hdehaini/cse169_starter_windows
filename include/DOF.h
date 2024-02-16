#pragma once

#include "core.h"

class DOF {
public:
    DOF();
    ~DOF();
    
    void SetValue(float v);
    float GetValue();
    void SetMinMax(float min, float max);

    float value;
    float min;
    float max;

private:
};