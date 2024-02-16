#include "Skeleton.h"
#include "Tokenizer.h"

Skeleton::Skeleton() {
    root = new Joint();
    World = glm::mat4(1.f);
}

Skeleton::~Skeleton() {
    delete root;
}

bool Skeleton::Load(const char *file) {
    Tokenizer token;
    token.Open(file);
    token.FindToken("balljoint");
    // Parse tree
    root=new Joint;
    root->Load(token);
    // Finish
    token.Close();
    return true;
}


void Skeleton::Update() {
    root->Update(World);
}

void Skeleton::Draw(const glm::mat4& viewProjMtx, GLuint shader) {
    root->Draw(viewProjMtx, shader);
}

void Skeleton::populateDOFVector(Joint* joint) {
    allJoints.push_back(joint);
    for (Joint* child : joint->children){
        for(DOF* dof : child->dofs){
            allDOFs.push_back(dof);
        }
        populateDOFVector(child);
    }
}

glm::mat4 Skeleton::GetWorldMatrix(int joint){
    return allJoints[joint]->world;
}
