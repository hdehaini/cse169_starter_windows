#include "Fluid.h"

Fluid::Fluid(int numParticles) {
    color = glm::vec3(0.2f, 0.0f, 0.7f);
    model = glm::mat4(1.0f);
    // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    glGenVertexArrays(1, &meshVAO);
    glGenBuffers(1, &positionVBO);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> distX(velX, 0.5);
    std::normal_distribution<float> distY(velY, 0.25);
    std::normal_distribution<float> distZ(velZ, 0.25);

    // Create the particles
    for (int i = 0; i < numParticles; i++) {
        Particle* p = new Particle();
        p->Position = glm::vec3(positionX, positionY, positionZ);
        float velocityX = distX(gen);
        float velocityY = distY(gen);
        float velocityZ = distZ(gen);
        velocityX = std::max(velX - 1, std::min(velocityX, velX + 1));
        velocityY = std::max(velY - 0.5f, std::min(velocityY, velY + 0.5f));
        velocityZ = std::max(velZ - 0.5f, std::min(velocityZ, velZ + 0.5f));
        p->Velocity = glm::vec3(velocityX, velocityY, velocityZ);
        p->Mass = ParticleMass;
        p->Radius = ParticleRadius;
        p->startLife = 0.0f;
        p->Life = LifeSpan;
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

    Particles.clear();
    positions.clear();
}

void Fluid::Update(float deltaTime) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> distX(velX, 0.5);
    std::normal_distribution<float> distY(velY, 0.25);
    std::normal_distribution<float> distZ(velZ, 0.25);
    // Add particles over time using particle rate
    static float time = 0;
    deltaTime /= 20.0f;
    time += deltaTime;
    if (time > 1.0f / ParticleRate) {
        Particle* p = new Particle();
        p->Position = glm::vec3(positionX, positionY, positionZ);
        float velocityX = distX(gen);
        float velocityY = distY(gen);
        float velocityZ = distZ(gen);
        velocityX = std::max(velX - 1, std::min(velocityX, velX + 1));
        velocityY = std::max(velY - 0.5f, std::min(velocityY, velY + 0.5f));
        velocityZ = std::max(velZ - 0.5f, std::min(velocityZ, velZ + 0.5f));
        p->Velocity = glm::vec3(velocityX, velocityY, velocityZ);
        p->Mass = ParticleMass;
        p->Radius = ParticleRadius;
        p->startLife = time;
        p->Life = LifeSpan;
        Particles.push_back(p);
        positions.push_back(p->Position);
        time -= 1.0f / ParticleRate;
    }

    // If Life is less than 0, remove the particle
    for (int i = 0; i < Particles.size(); i++) {
        Particles[i]->Life -= deltaTime;
        if (Particles[i]->Life < 0) {
            delete Particles[i];
            Particles.erase(Particles.begin() + i);
            positions.erase(positions.begin() + i);
        }
    }
    
    // Apply Gravity
    for (int i = 0; i < Particles.size(); i++) {
        Particles[i]->Force += glm::vec3(0, Gravity, 0) * Particles[i]->Mass;
    }

    // Aerodynamics
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
            Particles[i]->Velocity *= CollisionElasticity;
            Particles[i]->Velocity.y *= -1.f;
        }

        // Ceiling
        if (Particles[i]->Position.y > 5) {
            Particles[i]->Position.y = 5;
            Particles[i]->Velocity *= CollisionElasticity;
            Particles[i]->Velocity.y *= -1.f;
        }

        // Walls
        if (Particles[i]->Position.x < -5) {
            Particles[i]->Position.x = -5;
            Particles[i]->Velocity *= CollisionElasticity;
            Particles[i]->Velocity.x *= -1.f;
        }
        if (Particles[i]->Position.x > 5) {
            Particles[i]->Position.x = 5;
            Particles[i]->Velocity *= CollisionElasticity;
            Particles[i]->Velocity.x *= -1.f;
        }
        if (Particles[i]->Position.z < -5) {
            Particles[i]->Position.z = -5;
            Particles[i]->Velocity *= CollisionElasticity;
            Particles[i]->Velocity.z *= -1.f;
        }
        if (Particles[i]->Position.z > 5) {
            Particles[i]->Position.z = 5;
            Particles[i]->Velocity *= CollisionElasticity;
            Particles[i]->Velocity.z *= -1.f;
        }

        // Collision with other particles with collision elasticity and friction
        for (int j = i + 1; j < Particles.size(); j++) {
            glm::vec3 dist = Particles[i]->Position - Particles[j]->Position;
            float d = glm::length(dist);
            // std::cout << "Distance: " << d << std::endl;
            if (d < 0.1f) {
                glm::vec3 normal = glm::normalize(dist);
                glm::vec3 relativeVelocity = Particles[i]->Velocity - Particles[j]->Velocity;
                float relativeSpeed = glm::dot(relativeVelocity, normal);
                if (relativeSpeed < 0) {
                    // std::cout << "Collision" << std::endl;
                    float impulse = -(1 + CollisionElasticity) * relativeSpeed / (1 / Particles[i]->Mass + 1 / Particles[j]->Mass);
                    // Use particle impulse function to apply the impulse
                    Particles[i]->ApplyImpulse(impulse * normal);
                    Particles[j]->ApplyImpulse(-impulse * normal);
                    // // Apply friction
                    glm::vec3 tangent = relativeVelocity - relativeSpeed * normal;
                    tangent = glm::normalize(tangent);
                    float frictionImpulse = -glm::dot(relativeVelocity, tangent) / (1 / Particles[i]->Mass + 1 / Particles[j]->Mass);
                    if (glm::length(frictionImpulse * tangent) < impulse * CollisionFriction) {
                        Particles[i]->ApplyImpulse(-frictionImpulse * tangent);
                        Particles[j]->ApplyImpulse(frictionImpulse * tangent);
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

// Draw using particle.vert and particle.frag
void Fluid::Draw(const glm::mat4& viewProjMtx, GLuint shader) {
    // Use the shader of programID
    glUseProgram(shader);

    // Bind the VAO
    glBindVertexArray(meshVAO);

    // Bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Set the shader uniforms
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&viewProjMtx);
    glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, GL_FALSE, (float*)&model);
    // glUniform3fv(glGetUniformLocation(shader, "color"), 1, &color[0]);

    // Draw the points
    glPointSize(ParticleRadius);
    glDrawArrays(GL_POINTS, 0, positions.size());

    // Unbind the VAO
    glBindVertexArray(0);

    // Unbind the shader
    glUseProgram(0);

    // Unbind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}