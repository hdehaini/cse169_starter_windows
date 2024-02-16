#pragma once

#include "glm/gtx/euler_angles.hpp"
#include "DOF.h"
#include "Tokenizer.h"
#include "Cube.h"
#include <vector>

class Joint {
public:
    Joint();
    ~Joint();

    bool Load(Tokenizer &token);
    void Update(glm::mat4 parentWorld);
    void AddChild(Joint* child);
    void Draw(const glm::mat4& viewProjMtx, GLuint shader);

    DOF *dx = new DOF;
    DOF *dy = new DOF;
    DOF *dz = new DOF;
    std::vector<DOF*> dofs = {dx, dy, dz};
    std::vector<Joint*> children;
    glm::mat4 local;
    glm::mat4 world;
    glm::vec3 offset;
    glm::vec3 boxmin;
    glm::vec3 boxmax;
    Cube *cube;

private:
};