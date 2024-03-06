#include "Cloth.h"

Cloth::Cloth(int width, int height) {
    color = glm::vec3(0.0f, 0.0f, 0.7f);
    model = glm::mat4(1.0f);
    // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    glGenVertexArrays(1, &meshVAO);
    glGenBuffers(1, &positionVBO);
    glGenBuffers(1, &normalVBO);
    glGenBuffers(1, &ebo);

    Width = width;
    Height = height;
    ParticleMass = 1.0f;
    TotalMass = Width*Height*ParticleMass;
    // Create particles
    for (int i=0; i<Width; i++) {
        for (int j=0; j<Height; j++) {
            Particle* p = new Particle();
            p->Position = glm::vec3(i, j, 0);
            p->Mass = ParticleMass;
            Particles.push_back(p);
        }
    }

    std::cout << "Particles created" << std::endl;

    // // Create springs
    // for (int i=0; i<Width; i++) {
    //     for (int j=0; j<Height; j++) {
    //         if (i<Width-1) {
    //             SpringDamper* sd = new SpringDamper;

    //             sd->P1 = Particles[i*Height+j];
    //             sd->P2 = Particles[(i+1)*Height+j];
    //             sd->RestLength = glm::length(sd->P1->Position - sd->P2->Position);

    //             Springs.push_back(sd);
    //         }
    //         if (j<Height-1) {
    //             SpringDamper* sd = new SpringDamper;

    //             sd->P1 = Particles[i*Height+j];
    //             sd->P2 = Particles[i*Height+j+1];
    //             sd->RestLength = glm::length(sd->P1->Position - sd->P2->Position);
                
    //             Springs.push_back(sd);
    //         }
    //         if (i<Width-1 && j<Height-1) {
    //             SpringDamper* sd = new SpringDamper;

    //             sd->P1 = Particles[i*Height+j];
    //             sd->P2 = Particles[(i+1)*Height+j+1];
    //             sd->RestLength = glm::length(sd->P1->Position - sd->P2->Position);
                
    //             Springs.push_back(sd);
    //         }
    //         if (i<Width-1 && j>0) {
    //             SpringDamper* sd = new SpringDamper;

    //             sd->P1 = Particles[i*Height+j];
    //             sd->P2 = Particles[(i+1)*Height+j-1];
    //             sd->RestLength = glm::length(sd->P1->Position - sd->P2->Position);
                
    //             Springs.push_back(sd);
    //         }
    //     }
    // }

    std::cout << "Springs created" << std::endl;

    // Create triangles
    for (int i=0; i<Width-1; i++) {
        for (int j=0; j<Height-1; j++) {
            Triangle* t1 = new Triangle;
            t1->P1 = Particles[i * Height+j];
            t1->P2 = Particles[(i+1) * Height+j];
            t1->P3 = Particles[i * Height + j + 1];

            t1->Normal = glm::cross(t1->P2->Position - t1->P1->Position, t1->P3->Position - t1->P1->Position);
            t1->Normal = glm::normalize(t1->Normal);

            Triangles.push_back(t1);
            Triangle* t2 = new Triangle;
            t2->P1 = Particles[(i+1)*Height+j];
            t2->P2 = Particles[(i+1)*Height+j+1];
            t2->P3 = Particles[i*Height+j+1];

            t2->Normal = glm::cross(t2->P2->Position - t2->P1->Position, t2->P3->Position - t2->P1->Position);
            t2->Normal = glm::normalize(t2->Normal);

            Triangles.push_back(t2);
        }
    }

    std::cout << "Triangles created" << std::endl;
}

Cloth::~Cloth() {
    for (int i=0; i<Particles.size(); i++) {
        delete Particles[i];
    }
    for (int i=0; i<Springs.size(); i++) {
        delete Springs[i];
    }
    for (int i=0; i<Triangles.size(); i++) {
        delete Triangles[i];
    }
    glDeleteBuffers(1, &positionVBO);
    glDeleteBuffers(1, &normalVBO);
    glDeleteVertexArrays(1, &meshVAO);
    glDeleteBuffers(1, &ebo);
}

void Cloth::Update(float deltaTime) {
    // Apply gravity
    for (int i=0; i<Particles.size(); i++) {
        if (!Particles[i]->isFixed){
            Particles[i]->Force = glm::vec3(0, -9.8f, 0) * Particles[i]->Mass;
        }
    }
    // Apply spring forces
    for (int i=0; i<Springs.size(); i++) {
        Springs[i]->ComputeForce();
    }
    // Integrate
    for (int i=0; i<Particles.size(); i++) {
        Particles[i]->Integrate(deltaTime);
    }
    // // Aerodynamics
    for (int i=0; i<Triangles.size(); i++) {
        glm::vec3 v1 = Triangles[i]->P1->Velocity;
        glm::vec3 v2 = Triangles[i]->P2->Velocity;
        glm::vec3 v3 = Triangles[i]->P3->Velocity;
        glm::vec3 normal = Triangles[i]->Normal;
        glm::vec3 v = (v1+v2+v3)/3.0f;
        glm::vec3 force = 0.5f*1.225f*glm::length(v)*glm::length(v)*0.5f*normal;
        Triangles[i]->P1->ApplyForce(force);
        Triangles[i]->P2->ApplyForce(force);
        Triangles[i]->P3->ApplyForce(force);
    }

    std::cout << "Cloth updated" << std::endl;
}

// Draw using OpenGL
void Cloth::Draw(const glm::mat4& viewProjMtx, GLuint shader) {
    // // Bind the VAO
    // glBindVertexArray(meshVAO);
    // // Bind the position VBO
    // glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    // // Upload the data
    // glBufferData(GL_ARRAY_BUFFER, Particles.size()*sizeof(glm::vec3), &Particles[0]->Position, GL_DYNAMIC_DRAW);
    // // Set the vertex attribute pointers
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // // Bind the normal VBO
    // glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    // // Upload the data
    // glBufferData(GL_ARRAY_BUFFER, Particles.size()*sizeof(glm::vec3), &Particles[0]->Normal, GL_DYNAMIC_DRAW);
    // // Set the vertex attribute pointers
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // // Bind the EBO
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    // // Upload the data
    // std::vector<unsigned int> indices;
    // for (int i=0; i<Width-1; i++) {
    //     for (int j=0; j<Height-1; j++) {
    //         indices.push_back(i*Height+j);
    //         indices.push_back((i+1)*Height+j);
    //         indices.push_back(i*Height+j+1);
    //         indices.push_back((i+1)*Height+j);
    //         indices.push_back((i+1)*Height+j+1);
    //         indices.push_back(i*Height+j+1);
    //     }
    // }
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

    // // actiavte the shader program
    // glUseProgram(shader);

    // // get the locations and send the uniforms to the shader
    // glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
    // glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
    // glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

    // // Bind the VAO
    // glBindVertexArray(meshVAO);

    // // Draw the triangles
    // glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    // // Unbind the VAO
    // glBindVertexArray(0);
}