#include "Fluid.h"

Fluid::Fluid(int numParticles) {
    color = glm::vec3(0.2f, 0.0f, 0.7f);
    model = glm::mat4(1.0f);
    // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    glGenVertexArrays(1, &meshVAO);
    glGenBuffers(1, &positionVBO);

    // Create the particles
    for (int i = 0; i < numParticles; i++) {
        Particle* p = new Particle();
        p->Position = glm::vec3(i, i, -i) * 0.1f;
        p->Velocity = glm::vec3(rand() % 10 - 5, rand() % 10 - 5, rand() % 10 - 5);
        p->Mass = 0.1f;
        p->Radius = 0.1f;
        p->startLife = 0.0f;
        Particles.push_back(p);
    }

    // Set the particle positions
    for (int i = 0; i < Particles.size(); i++) {
        positions.push_back(Particles[i]->Position);
    }
}

Fluid::~Fluid() {
    // Delete the VBOs and the VAO.
    glDeleteBuffers(1, &positionVBO);
    glDeleteVertexArrays(1, &meshVAO);

    // Delete the particles
    for (int i = 0; i < Particles.size(); i++) {
        delete Particles[i];
    }
}

void Fluid::Update(float deltaTime) {
    // Add particles over time
    static float time = 0;
    time += deltaTime;
    if (time > 1.0f / ParticleRate) {
        Particle* p = new Particle();
        p->Position = glm::vec3(0, 10, 0);
        p->Velocity = glm::vec3(0, 0, 0);
        p->Mass = 0.1f;
        p->Radius = 0.1f;
        p->startLife = time;
        Particles.push_back(p);
        positions.push_back(p->Position);
        time = 0;
    }

    // If Life is less than 0, remove the particle
    for (int i = 0; i < Particles.size(); i++) {
        Particles[i]->Life = Particles[i]->startLife - time;
        if (Particles[i]->Life < 0) {
            delete Particles[i];
            Particles.erase(Particles.begin() + i);
            positions.erase(positions.begin() + i);
        }
    }
    
    // Apply Gravity
    for (int i = 0; i < Particles.size(); i++) {
        Particles[i]->Force += glm::vec3(0, -9.8f, 0) * Particles[i]->Mass;
    }

    // Aerodynamics
    for (int i = 0; i < Particles.size(); i++) {
        glm::vec3 v = Particles[i]->Velocity;
        float speed = glm::length(v);
        glm::vec3 drag = -0.5f * 1.225f * speed * speed * 0.47f * 0.1f * v;
        Particles[i]->Force += drag;
    }

    // Aerodynamics using Fluid variables
    for(int i = 0; i < Particles.size(); i++) {
        glm::vec3 v = Particles[i]->Velocity;
        float speed = glm::length(v);
        glm::vec3 drag = -0.5f * AirDensity * speed * speed * DragCoefficient * ParticleRadius * v;
        Particles[i]->Force += drag;
    }


    // Integrate
    for (int i=0; i<Particles.size(); i++) {
        Particles[i]->Integrate(deltaTime);
    }
    

    // Collision detection and response with the ground, walls, and other particles including radius
    for (int i = 0; i < Particles.size(); i++) {
        // Ground
        if (Particles[i]->Position.y < 0) {
            Particles[i]->Position.y = 0;
            Particles[i]->Velocity.y *= -0.5f;
        }

        // Walls
        if (Particles[i]->Position.x < -25) {
            Particles[i]->Position.x = -25;
            Particles[i]->Velocity.x *= -0.5f;
        }
        if (Particles[i]->Position.x > 25) {
            Particles[i]->Position.x = 25;
            Particles[i]->Velocity.x *= -0.5f;
        }
        if (Particles[i]->Position.z < -25) {
            Particles[i]->Position.z = -25;
            Particles[i]->Velocity.z *= -0.5f;
        }
        if (Particles[i]->Position.z > 25) {
            Particles[i]->Position.z = 25;
            Particles[i]->Velocity.z *= -0.5f;
        }

        // Other particles
        for (int j = 0; j < Particles.size(); j++) {
            if (i != j) {
                glm::vec3 dist = Particles[i]->Position - Particles[j]->Position;
                float len = glm::length(dist);
                if (len < Particles[i]->Radius + Particles[j]->Radius) {
                    glm::vec3 n = glm::normalize(dist);
                    glm::vec3 v = Particles[i]->Velocity - Particles[j]->Velocity;
                    float vn = glm::dot(v, n);
                    if (vn > 0) {
                        float e = CollisionElasticity;
                        float f = -(1 + e) * vn / (1 / Particles[i]->Mass + 1 / Particles[j]->Mass);
                        glm::vec3 impulse = f * n;
                        Particles[i]->Velocity += 1 / Particles[i]->Mass * impulse;
                        Particles[j]->Velocity -= 1 / Particles[j]->Mass * impulse;
                    }
                }
            }
        }
    }


    // Update the positions
    for (int i = 0; i < Particles.size(); i++) {
        positions[i] = Particles[i]->Position;
    }
}

void Fluid::Draw(const glm::mat4& viewProjMtx, GLuint shader) {
    // Use the shader of programID
    glUseProgram(shader);

    // Bind the VAO
    glBindVertexArray(meshVAO);

    // Set up the model, view, projection matrices
    glm::mat4 modelViewProjMtx = viewProjMtx * model;
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelViewProjMtx"), 1, false, &modelViewProjMtx[0][0]);    

    // Set up the color
    glUniform3fv(glGetUniformLocation(shader, "color"), 1, &color[0]);

    // Set up the position VBO
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);

    // Set up the position input
    GLuint positionInput = glGetAttribLocation(shader, "position");
    glEnableVertexAttribArray(positionInput);
    glVertexAttribPointer(positionInput, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Draw the points
    glDrawElements(GL_POINTS, positions.size(), GL_UNSIGNED_INT, 0);

    // Unbind the VAO
    glBindVertexArray(0);
}