#include "Channel.h"

void Channel::Load(Tokenizer& token){
    token.FindToken("channel");
    token.SkipLine();

    token.FindToken("extrapolate");
    char temp1[256];
    extrapolate_in = token.GetToken(temp1);
    char temp2[256];
    extrapolate_out = token.GetToken(temp2);
    token.SkipLine();
    
    token.FindToken("keys");
    int numKeys = token.GetInt();
    token.SkipLine();
    for(int j = 0; j < numKeys; j++){
        Keyframe* newKeyframe = new Keyframe();
        newKeyframe->Time = token.GetFloat();
        newKeyframe->Value = token.GetFloat();
        char temp3[256];
        newKeyframe->RuleIn = token.GetToken(temp3);
        char temp4[256];
        newKeyframe->RuleOut = token.GetToken(temp4);
        keyframes.push_back(newKeyframe);
        token.SkipLine();
    }

    Precompute();
}

void Channel::Precompute(){
    size_t n = keyframes.size();
    
    // Ensure there are enough keyframes to compute tangents
    if (n < 2) return;
    
    // Handle first keyframe's tangents
    if (keyframes[0]->RuleIn == "flat" || keyframes[0]->RuleOut == "flat") {
        keyframes[0]->TangentIn = 0.0f;
        keyframes[0]->TangentOut = 0.0f;
    } else if (keyframes[0]->RuleIn == "linear" || keyframes[0]->RuleOut == "linear") {
        keyframes[0]->TangentIn = (keyframes[1]->Value - keyframes[0]->Value) / (keyframes[1]->Time - keyframes[0]->Time);
        keyframes[0]->TangentOut = keyframes[0]->TangentIn;
    } else if (keyframes[0]->RuleIn == "smooth" || keyframes[0]->RuleOut == "smooth") {
        // For a smooth tangent at the first keyframe, use the slope to the next keyframe
        keyframes[0]->TangentIn = (keyframes[1]->Value - keyframes[0]->Value) / (keyframes[1]->Time - keyframes[0]->Time);
        keyframes[0]->TangentOut = keyframes[0]->TangentIn;
    }

    // Handle last keyframe's tangents
    if (keyframes[n - 1]->RuleIn == "flat" || keyframes[n - 1]->RuleOut == "flat") {
        keyframes[n - 1]->TangentIn = 0.0f;
        keyframes[n - 1]->TangentOut = 0.0f;
    } else if (keyframes[n - 1]->RuleIn == "linear" || keyframes[n - 1]->RuleOut == "linear") {
        keyframes[n - 1]->TangentIn = (keyframes[n - 1]->Value - keyframes[n - 2]->Value) / (keyframes[n - 1]->Time - keyframes[n - 2]->Time);
        keyframes[n - 1]->TangentOut = keyframes[n - 1]->TangentIn;
    } else if (keyframes[n - 1]->RuleIn == "smooth" || keyframes[n - 1]->RuleOut == "smooth") {
        // For a smooth tangent at the last keyframe, use the slope from the previous keyframe
        keyframes[n - 1]->TangentIn = (keyframes[n - 1]->Value - keyframes[n - 2]->Value) / (keyframes[n - 1]->Time - keyframes[n - 2]->Time);
        keyframes[n - 1]->TangentOut = keyframes[n - 1]->TangentIn;
    }

    for (size_t i = 1; i < keyframes.size() - 1; ++i) {
        std::string rule1 = keyframes[i]->RuleIn;
        if (rule1 == "flat") {
            keyframes[i]->TangentOut = 0.0f;
            keyframes[i+1]->TangentIn = 0.0f;
        } else if (rule1 == "linear"){
            float val = (keyframes[i+1]->Value - keyframes[i]->Value) / (keyframes[i+1]->Time - keyframes[i]->Time);
            keyframes[i]->TangentOut = val;
            keyframes[i+1]->TangentIn = val;
        
        } else if (keyframes[i]->RuleIn == "smooth"){ // fix
            float slope = (keyframes[i+1]->Value - keyframes[i-1]->Value) / (keyframes[i+1]->Time - keyframes[i-1]->Time);
            keyframes[i]->TangentIn = slope;
            keyframes[i]->TangentOut = slope;
            keyframes[i+1]->TangentIn = slope;
        }

        
    }
    // Compute cubic coefficients from tangents & other data
    for (int i = 0; i < keyframes.size() - 1; i++) {
        float p0 = keyframes[i]->Value;
        float p1 = keyframes[i + 1]->Value;
        float v0 = keyframes[i]->TangentOut;
        float v1 = keyframes[i + 1]->TangentIn;
        float t0 = keyframes[i]->Time;
        float t1 = keyframes[i + 1]->Time;

        // Create the basis matrix for cubic Hermite spline
        glm::mat4 basis(2, -3, 0, 1,
                        -2, 3, 0, 0,
                        1, -2, 1, 0,
                        1, -1, 0, 0);

        std::cout << glm::to_string(basis);

        // Create the vector for p0, p1, v0, v1 (note the v0 and v1 are scaled by (t1 - t0))
        glm::vec4 p(p0, p1, (t1 - t0) * v0, (t1 - t0) * v1);

        // Multiply the basis matrix by the p vector to get the coefficients
        glm::vec4 coefficients = basis * p;

        // Extract the coefficients into the keyframe's cubic coefficients
        keyframes[i]->A = coefficients[0];
        keyframes[i]->B = coefficients[1];
        keyframes[i]->C = coefficients[2];
        keyframes[i]->D = coefficients[3];
    }

    // size_t last = keyframes.size() - 1;
    // if (last >= 0) {
    //     keyframes[last]->A = 0.0f;
    //     keyframes[last]->B = 0.0f;
    //     keyframes[last]->C = 0.0f;
    //     keyframes[last]->D = keyframes[last]->Value; // This ensures the curve evaluates to the last keyframe's value
    // }
}

float Channel::Extrapolate(std::string method, float time) {
    if (method == "constant") {
        if (time < keyframes.front()->Time) {
            return keyframes.front()->Value;
        } else {
            return keyframes.back()->Value;
        }
    } else if(method == "linear") {
        size_t n = keyframes.size();
        if (n < 2) return 0.0f;

        if (time < keyframes.front()->Time) {
            float slope = keyframes[0]->TangentOut;
            return keyframes.front()->Value + slope * (time - keyframes.front()->Time);
        } else { // time > keyframes.back()->Time
            float slope = keyframes[n - 1]->TangentIn;
            return keyframes.back()->Value + slope * (time - keyframes.back()->Time);
        }

    } else if(method == "cycle") {
        float duration = keyframes.back()->Time - keyframes.front()->Time;
        float modTime = fmod(time - keyframes.front()->Time, duration) + keyframes.front()->Time;

        return Evaluate(modTime);
    
    } else if(method == "cycle_offset") {
        float duration = keyframes.back()->Time - keyframes.front()->Time;
        float cycles = floor((time - keyframes.front()->Time) / duration);
        float modTime = fmod(time - keyframes.front()->Time, duration) + keyframes.front()->Time;
        float offset = cycles * (keyframes.back()->Value - keyframes.front()->Value);

        return Evaluate(modTime) + offset;
        
    } else if(method == "bounce") {
        float duration = keyframes.back()->Time - keyframes.front()->Time;
        float cycles = floor((time - keyframes.front()->Time) / duration);
        float modTime = fmod(time - keyframes.front()->Time, duration);
        if (int(cycles) % 2 == 0) {
            // Even cycle, play normally
            modTime += keyframes.front()->Time;
        } else {
            // Odd cycle, reverse
            modTime = keyframes.back()->Time - modTime;
        }
        return Evaluate(modTime);
    }
    return 0.0f;
}

float Channel::Evaluate(float time) {
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

    // Iterate through keyframes to find the correct span for the given time
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
            // Evaluate the cubic equation
            return (a*u*u*u) + (b*u*u) + (c*u) + d;
        }
    }

    return 0.0f;
}