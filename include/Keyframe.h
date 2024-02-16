#pragma once

#include "core.h"
#include <iostream>

class Keyframe {
    public:
        float Time;
        float Value;
        float TangentIn,TangentOut;
        std::string RuleIn,RuleOut; // Tangent rules
        float A,B,C,D; // Cubic coefficients
};