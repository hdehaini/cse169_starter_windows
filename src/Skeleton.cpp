#include "Skeleton.h"
#include "Tokenizer.h"

Skeleton::Skeleton() {
    root = new Joint();
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


void Skeleton::Update(glm::mat4 parentWorld) {
    root->Update(parentWorld);
}

void Skeleton::Draw(const glm::mat4& viewProjMtx, GLuint shader) {
    root->Draw(viewProjMtx, shader);
}

