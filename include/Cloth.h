#pragma once

#include "core.h"
#include "Particle.h"
#include "SpringDamper.h"
#include "Triangle.h"
#include <iostream>
#include <vector>

class Cloth {
public:

    Cloth(int width, int height);
    ~Cloth();

    void Update(float deltaTime);
    void Draw();

    std::vector<Particle*> Particles;
    std::vector<SpringDamper*> Springs;
    std::vector<Triangle*> Triangles;

    int Width, Height;
    float ParticleMass;
    float TotalMass; // n*ParticleMass

    // OpenGL stuff from Skin
    GLuint meshVAO;
    GLuint positionVBO;
    GLuint normalVBO;
    GLuint ebo;
    glm::vec3 color;
    glm::mat4 model;
};
