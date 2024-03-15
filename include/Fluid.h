#pragma once

#include "core.h"
#include "Particle.h"
#include "SpringDamper.h"
#include "Triangle.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <random>

class Fluid {
public:

    Fluid(int numParticles);
    ~Fluid();

    void Update(float deltaTime);
    void Draw(const glm::mat4& viewProjMtx, GLuint shader);

    std::vector<Particle*> Particles;

    float positionX = 0.0f;
    float positionY = 2.0f;
    float positionZ = 0.0f;
    float velX = 4.0f;
    float velY = 0.5f;
    float velZ = 0.5f;
    int ParticleRate = 0;
    float ParticleMass = 0.5f;
    float TotalMass; // n*ParticleMass
    float forwardSpeed = 1.0f;
    float Gravity = -1.0f;
    float AirDensity = 1.225f;
    float DragCoefficient = 0.47f;
    float ParticleRadius = 5.0f;
    float CollisionElasticity = 0.5f;
    float CollisionFriction = 0.5f;
    float LifeSpan = 5.0f;


    std::vector<glm::vec3> positions;

    // OpenGL stuff from Skin
    GLuint meshVAO;
    GLuint positionVBO;
    GLuint ebo;
    glm::vec3 color;
    glm::mat4 model;
};