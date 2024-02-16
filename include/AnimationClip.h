#pragma once

#include "core.h"
#include "Channel.h"
#include "Tokenizer.h"
#include "Joint.h"
#include "DOF.h"
#include "Skeleton.h"
#include <iostream>
#include <vector>

class AnimationClip {
    public:
        bool Load(const char *filename);
        void Evaluate(float time);

        std::vector<Channel*> channels;
        float timeStart;
        float timeEnd;

        Skeleton* skeleton;
};