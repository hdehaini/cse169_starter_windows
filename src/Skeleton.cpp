#include "../include/Skeleton.h"
#include "../include/Tokenizer.h"

Skeleton::Skeleton() {
    root = new Joint();
}

Skeleton::~Skeleton() {
    delete root;
}

bool Skeleton::Load(const char *file) {
    Tokenizer token;
    token.Open(file,"skel");
    token.FindToken("balljoint");
    // Parse tree
    root=new Joint;
    root->Load(token);
    // Finish
    token.Close();
    return true;
}


void Skeleton::Update() {
    root->Update();
}

void Skeleton::Draw() {
    root->Draw();
}

