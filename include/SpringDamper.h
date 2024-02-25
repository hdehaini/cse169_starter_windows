#pragma once

#include "core.h"
#include "Particle.h"
#include <iostream>

class SpringDamper {
public:
    void ComputeForce(){
        glm::vec3 P1P2 = P2->Position - P1->Position;
        glm::vec3 V1V2 = P2->Velocity - P1->Velocity;
        float L = glm::length(P1P2);
        glm::vec3 force = -SpringConstant * (L - RestLength) * glm::normalize(P1P2) - DampingConstant * V1V2;
        P1->ApplyForce(force);
        P2->ApplyForce(-force);
    }
    
    float restLength = glm::length(P1->Position - P2->Position);
    float SpringConstant;
    float DampingConstant;
    float RestLength;
    Particle *P1, *P2;
};
