#pragma once

#include "core.h"
#include "Particle.h"
#include "SpringDamper.h"
#include "Triangle.h"
#include <stdlib.h>
#include <iostream>
#include <vector>

class Fluid {
public:

    Fluid(int numParticles);
    ~Fluid();

    void Update(float deltaTime);
    void Draw(const glm::mat4& viewProjMtx, GLuint shader);

    std::vector<Particle*> Particles;

    int ParticleRate = 10;
    float ParticleMass = 0.1f;
    float TotalMass; // n*ParticleMass
    float forwardSpeed = 0.1f;
    float Gravity = 9.8f;
    float AirDensity = 1.225f;
    float DragCoefficient = 0.47f;
    float ParticleRadius = 0.1f;
    float CollisionElasticity = 0.5f;
    float CollisionFriction = 0.5f;


    std::vector<glm::vec3> positions;

    // OpenGL stuff from Skin
    GLuint meshVAO;
    GLuint positionVBO;
    GLuint ebo;
    glm::vec3 color;
    glm::mat4 model;
};