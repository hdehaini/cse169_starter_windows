#include "AnimationClip.h"



bool AnimationClip::Load(const char *filename) {
    Tokenizer token;
    if (!token.Open(filename)) {
        std::cout << "Error opening animation file: " << filename << std::endl;
        return false;
    }

    token.FindToken("animation");
    token.SkipLine();

    token.FindToken("range");
    timeStart = token.GetFloat();
    timeEnd = token.GetFloat();
    token.SkipLine();

    token.FindToken("numchannels");
    int numChannels = token.GetInt();
    token.SkipLine();

    for (int i = 0; i < numChannels; i++) {
        Channel* newChannel = new Channel();
        newChannel->Load(token);
        // newChannel->Print(i);
        // float vv = newChannel->Evaluate(1.5, true);
        // std::cout << "value: " << vv << "   " << std::ends;
        newChannel->Print(i);
        channels.push_back(newChannel);
    }
    token.Close();
    return true;
}

void AnimationClip::Evaluate(float time) {
    // n joints: 3n dofs
    // 3n + 3 channels
    // first 3 are for pos

    // std::cout << skeleton->allDOFs.size();
    if (skeleton == nullptr) {
        std::cout << "skeleton is a nullpointer";
        return;
    }

    if (skeleton->allJoints.empty()) {
        std::cout << "skeleton joints is empty";
        return;
    }

    float x, y, z;
    x = channels[0]->Evaluate(time);
    y = channels[1]->Evaluate(time);
    z = channels[2]->Evaluate(time);
    // std::cout << "z: " << z; 
    skeleton->World[3] = {x, y, z, 1.f};

    for (int i = 0; i < skeleton->allDOFs.size(); i++) {
        skeleton->allDOFs[i]->value = channels[i + 6]->Evaluate(time);
    }

    // for (int i = 0; i < skeleton->allDOFs.size(); i++) {
    //     float newValue = channels[i+3]->Evaluate(time);
        
    //     skeleton->allDOFs[i]->value = newValue;
    // }
}