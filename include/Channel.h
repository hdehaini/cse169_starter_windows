#pragma once

#include "core.h"
#include "Keyframe.h"
#include "Tokenizer.h"
#include <iostream>
#include <vector>
#include <cmath>
#include "glm/gtx/string_cast.hpp"

class Channel {
    public:
        void Load(Tokenizer& token);
        void Precompute();
        float Evaluate(float time);
        float Extrapolate(std::string method, float time);
        
        std::vector<Keyframe*> keyframes;
        std::string extrapolate_in, extrapolate_out;
};