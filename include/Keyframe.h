#pragma once

#include "core.h"
#include <iostream>

class Keyframe {
    public:
        float Time;
        float Value;
        float TangentIn,TangentOut;
        std::string RuleIn,RuleOut; // Tangent rules
        float A = 0.f;
        float B = 0.f;
        float C = 0.f;
        float D = 0.f;
};