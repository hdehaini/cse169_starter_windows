#include "Channel.h"

void Channel::Load(Tokenizer& token){
    token.FindToken("channel");
    token.SkipLine();

    token.FindToken("extrapolate");
    char temp1[256];
    token.GetToken(temp1);
    if(strcmp(temp1, "constant") == 0){
        extrapolate_in = "constant";
    } else if(strcmp(temp1, "linear") == 0){
        extrapolate_in = "linear";
    } else if(strcmp(temp1, "cycle") == 0){
        extrapolate_in = "cycle";
    } else if(strcmp(temp1, "cycle_offset") == 0){
        extrapolate_in = "cycle_offset";
    } else if(strcmp(temp1, "bounce") == 0){
        extrapolate_in = "bounce";
    }
    char temp2[256];
    token.GetToken(temp2);
    if(strcmp(temp2, "constant") == 0){
        extrapolate_out = "constant";
    } else if(strcmp(temp2, "linear") == 0){
        extrapolate_out = "linear";
    } else if(strcmp(temp2, "cycle") == 0){
        extrapolate_out = "cycle";
    } else if(strcmp(temp2, "cycle_offset") == 0){
        extrapolate_out = "cycle_offset";
    } else if(strcmp(temp2, "bounce") == 0){
        extrapolate_out = "bounce";
    }
    token.SkipLine();
    
    token.FindToken("keys");
    int numKeys = token.GetInt();
    token.SkipLine();
    for(int j = 0; j < numKeys; j++){
        Keyframe* newKeyframe = new Keyframe();
        newKeyframe->Time = token.GetFloat();
        newKeyframe->Value = token.GetFloat();
        char temp3[256];
        token.GetToken(temp3);
        if(strcmp(temp3, "flat") == 0){
            newKeyframe->RuleIn = "flat";
        } else if(strcmp(temp3, "linear") == 0){
            newKeyframe->RuleIn = "linear";
        } else if(strcmp(temp3, "smooth") == 0){
            newKeyframe->RuleIn = "smooth";
        }
        char temp4[256];
        token.GetToken(temp4);
        if(strcmp(temp4, "flat") == 0){
            newKeyframe->RuleOut = "flat";
        } else if(strcmp(temp4, "linear") == 0){
            newKeyframe->RuleOut = "linear";
        } else if(strcmp(temp4, "smooth") == 0){
            newKeyframe->RuleOut = "smooth";
        }
        keyframes.push_back(newKeyframe);
        token.SkipLine();
    }

    Precompute();
}

void Channel::Precompute(){
    size_t n = keyframes.size();
    
    // Ensure there are enough keyframes to compute tangents
    if (n < 2) return;

    // Compute tangents from rules
    for (size_t i = 0; i < n; ++i) {
        if (keyframes[i]->RuleIn == "flat") {
            keyframes[i]->TangentIn = 0.0f;
        } else if (keyframes[i]->RuleIn == "linear") {
            if (i == 0) {
                keyframes[i]->TangentIn = 0.0f;
            } else {
                keyframes[i]->TangentIn = (keyframes[i]->Value - keyframes[i - 1]->Value) / (keyframes[i]->Time - keyframes[i - 1]->Time);
            }
        } else if (keyframes[i]->RuleIn == "smooth") {
            if (i == 0) {
                keyframes[i]->TangentIn = 0.0f;
            } else {
                float dt = keyframes[i]->Time - keyframes[i - 1]->Time;
                float dp = keyframes[i]->Value - keyframes[i - 1]->Value;
                if (i == n - 1) {
                    keyframes[i]->TangentIn = dp / dt;
                } else {
                    float dt2 = keyframes[i + 1]->Time - keyframes[i]->Time;
                    float dp2 = keyframes[i + 1]->Value - keyframes[i]->Value;
                    keyframes[i]->TangentIn = 0.5f * (dp / dt + dp2 / dt2);
                }
            }
        }
        if (keyframes[i]->RuleOut == "flat") {
            keyframes[i]->TangentOut = 0.0f;
        } else if (keyframes[i]->RuleOut == "linear") {
            if (i == n - 1) {
                keyframes[i]->TangentOut = 0.0f;
            } else {
                keyframes[i]->TangentOut = (keyframes[i + 1]->Value - keyframes[i]->Value) / (keyframes[i + 1]->Time - keyframes[i]->Time);
            }
        } else if (keyframes[i]->RuleOut == "smooth") {
            if (i == n - 1) {
                keyframes[i]->TangentOut = 0.0f;
            } else {
                float dt = keyframes[i + 1]->Time - keyframes[i]->Time;
                float dp = keyframes[i + 1]->Value - keyframes[i]->Value;
                if (i == 0) {
                    keyframes[i]->TangentOut = dp / dt;
                } else {
                    float dt2 = keyframes[i]->Time - keyframes[i - 1]->Time;
                    float dp2 = keyframes[i]->Value - keyframes[i - 1]->Value;
                    keyframes[i]->TangentOut = 0.5f * (dp / dt + dp2 / dt2);
                }
            }
        }
    }
    


    // Compute cubic coefficients from tangents & other data
    for (size_t i = 0; i < keyframes.size() - 1; ++i) {
        float t0 = keyframes[i]->Time;
        float t1 = keyframes[i + 1]->Time;
        float p0 = keyframes[i]->Value;
        float p1 = keyframes[i + 1]->Value;
        float m0 = keyframes[i]->TangentOut * (t1 - t0);
        float m1 = keyframes[i + 1]->TangentIn * (t1 - t0);
        float a = 2.0f * p0 - 2.0f * p1 + m0 + m1;
        float b = -3.0f * p0 + 3.0f * p1 - 2.0f * m0 - m1;
        float c = m0;
        float d = p0;
        keyframes[i]->A = a;
        keyframes[i]->B = b;
        keyframes[i]->C = c;
        keyframes[i]->D = d;
    }
}

void Channel::Print(int i){
    std::cout << "   channel " << i << std::endl;
}

// Extrapolation for time before the first keyframe or after the last keyframe
float Channel::Extrapolate(std::string type, float time) {
    if (type == "constant") {
        if (time < keyframes.front()->Time) {
            return keyframes.front()->Value;
        } else {
            return keyframes.back()->Value;
        }
    } else if (type == "linear") {
        if (time < keyframes.front()->Time) {
            return keyframes.front()->Value + (time - keyframes.front()->Time) * keyframes.front()->TangentOut;
        } else {
            return keyframes.back()->Value + (time - keyframes.back()->Time) * keyframes.back()->TangentIn;
        }
    } else if (type == "cycle") {
        float duration = keyframes.back()->Time - keyframes.front()->Time;
        float t = fmod(time - keyframes.front()->Time, duration);
        if (t < 0.0f) {
            t += duration;
        }
        return Evaluate(t + keyframes.front()->Time);
    } else if (type == "cycle_offset") {
        float duration = keyframes.back()->Time - keyframes.front()->Time;
        float cycleCount = floor((time - keyframes.front()->Time) / duration);
        float t = fmod(time - keyframes.front()->Time, duration);
        if (t < 0.0f) {
            t += duration;
        }
        float offset = cycleCount * (keyframes.back()->Value - keyframes.front()->Value);
        return Evaluate(t + keyframes.front()->Time) + offset;
} else if (type == "bounce") {
        float duration = keyframes.back()->Time - keyframes.front()->Time;
        float t = fmod(time - keyframes.front()->Time, 2.0f * duration);
        if (t < 0.0f) {
            t += 2.0f * duration;
        }
        if (t < duration) {
            return Evaluate(t + keyframes.front()->Time);
        } else {
            return Evaluate((2.0f * duration - t) + keyframes.front()->Time);
        }
    }
    return 0.0f;
}

// float Channel::Extrapolate(std::string method, float time) {
//     if (method == "constant") {
//         return keyframes[0]->Value;
//     } else if (method == "linear") {
//         float t0 = keyframes[0]->Time;
//         float t1 = keyframes[1]->Time;
//         float p0 = keyframes[0]->Value;
//         float p1 = keyframes[1]->Value;
//         return p0 + (p1 - p0) * (time - t0) / (t1 - t0);
//     } else if (method == "cycle") {
//         float t0 = keyframes[0]->Time;
//         float t1 = keyframes.back()->Time;
//         float duration = t1 - t0;
//         float t = time - t0;
//         return keyframes[0]->Value + Evaluate(fmod(t, duration));
//     } else if (method == "cycle_offset") {
//         float t0 = keyframes[0]->Time;
//         float t1 = keyframes.back()->Time;
//         float duration = t1 - t0;
//         float cycleDuration = duration * (keyframes.size() - 1); // Assuming uniform keyframe spacing
//         float t = time - t0;
//         float cyclesCompleted = floor(t / cycleDuration);
//         float changeInValue = keyframes.back()->Value - keyframes[0]->Value;
//         float offset = cyclesCompleted * changeInValue;
//         return Evaluate(fmod(t, cycleDuration)) + offset;
//     } else if (method == "bounce") {
//         float t0 = keyframes[0]->Time;
//         float t1 = keyframes.back()->Time;
//         float duration = t1 - t0;
//         float t = time - t0;
//         int bounces = (int) (t / duration);
//         float t2 = fmod(t, duration);
//         if (bounces % 2 == 0) {
//             return Evaluate(t2);
//         } else {
//             return Evaluate(duration - t2);
//         }
//     }
//     return 0.0f;
// }

float Channel::Evaluate(float time, bool debug) {
    if(keyframes.empty()) return 0.0f;

    if(keyframes.size() == 1) {
        return keyframes[0]->Value;
    }

    // Extrapolation for time before the first keyframe or after the last keyframe
    if(time < keyframes.front()->Time) {
        return Extrapolate(extrapolate_in, time);
        // return keyframes.front()->Value;

    } else if (time > keyframes.back()->Time) {
        return Extrapolate(extrapolate_out, time);
        // return keyframes.back()->Value;
    }

    for(auto kf : keyframes) {
        if(debug){
            // glm::vec4 abcd = glm::vec4(keyframes[i]->A, keyframes[i]->B, keyframes[i]->C, keyframes[i]->D);
            // std::cout << glm::to_string(abcd) << " " << std::ends;
            std::cout << " Rule in: " << kf->RuleIn << " " << std::ends;
            std::cout << std::endl;
        }
        
    }

    // // Iterate through keyframes to find the correct span for the given time using glm binary search
    // auto it = std::lower_bound(keyframes.begin(), keyframes.end(), time, [](Keyframe* a, float b) {
    //     return a->Time < b;
    // });
    // int idx = it - keyframes.begin();
    // if (idx == 0) {
    //     idx = 1;
    // } else if (idx == keyframes.size()) {
    //     idx = keyframes.size() - 1;
    // }

    // // Normalized time u within the current span
    // float t0 = keyframes[idx - 1]->Time;
    // float t1 = keyframes[idx]->Time;
    // float duration = t1 - t0;
    // float u = (time - t0) / duration;

    // // Cubic interpolation using precomputed coefficients
    // float a = keyframes[idx - 1]->A;
    // float b = keyframes[idx - 1]->B;
    // float c = keyframes[idx - 1]->C;
    // float d = keyframes[idx - 1]->D;

    // if (debug) {
    //     std::cout << "idx: " << idx << "  " << std::ends;  
    //     std::cout << "u: " << u << std::ends;
    //     std::cout << "  a: " << a << "  b: " << b << "  c: " << c << "  d: " << d << std::ends;
    // }
    // // Evaluate the cubic equation
    // return d + u * (c + u * (b + u * a));



    for(int i = 0; i < keyframes.size()-1; i++) {
        if (time >= keyframes[i]->Time && time <= keyframes[i + 1]->Time) {
            // Normalized time u within the current span
            float t0 = keyframes[i]->Time;
            float t1 = keyframes[i + 1]->Time;
            float duration = t1 - t0;
            float u = (time - t0) / duration;

            // Cubic interpolation using precomputed coefficients
            float a = keyframes[i]->A;
            float b = keyframes[i]->B;
            float c = keyframes[i]->C;
            float d = keyframes[i]->D;

            if (debug) {
                std::cout << "idx: " << i << "  " << std::ends;  
                std::cout << "u: " << u << std::ends;
                std::cout << "  a: " << a << "  b: " << b << "  c: " << c << "  d: " << d << std::ends;
            }
            // Evaluate the cubic equation
            return d + u * (c + u * (b + u * a));
        }
    }

    return 0.0f;
}