#include "../include/Joint.h"
#include "../include/Tokenizer.h"

Joint::Joint() {
    local = glm::mat4(1.0f);
    world = glm::mat4(1.0f);
}

Joint::~Joint() {
    for (int i = 0; i < dofs.size(); i++) {
        delete dofs[i];
    }
    for (int i = 0; i < children.size(); i++) {
        delete children[i];
    }
}

void Joint::Update() {
}

bool Joint::Load(Tokenizer &t) {
    token.FindToken("{");
    while(1) {
        char temp[256];
        token.GetToken(temp);
        if(strcmp(temp,"offset")==0) {
            Offset.x=token.GetFloat();
            Offset.y=token.GetFloat();
            Offset.z=token.GetFloat();
        }
        else // Check for other tokens
        else if(strcmp(temp,"balljoint")==0) {
            Joint *jnt=new Joint;
            jnt->Load(token);
            AddChild(*jnt);
        }
        else if(strcmp(temp,"}")==0) return true;
        else token.SkipLine(); // Unrecognized token
    }
}

void Joint::AddChild(Joint* child) {
    children.push_back(child);
}

void Joint::Draw() {
}