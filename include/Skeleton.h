#pragma once

#include "Joint.h"
#include "Tokenizer.h"

class Skeleton {
public:
    Skeleton();
    ~Skeleton();

    bool Load(const char *file);
    void Update();
    void Draw(const glm::mat4& viewProjMtx, GLuint shader);
    void populateDOFVector(Joint* joint);
    glm::mat4 GetWorldMatrix(int joint);

    std::vector<Joint*> allJoints;
    std::vector<DOF*> allDOFs;
    int currJoint = 0;
    Joint* root;
    glm::mat4 World;

private:
};