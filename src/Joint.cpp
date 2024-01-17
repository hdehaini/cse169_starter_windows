#include "Joint.h"
#include "Tokenizer.h"

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

void Joint::Update(glm::mat4 parentWorld) {
    glm::mat4x4 translate = glm::translate(glm::mat4(1.0f), offset);
    glm::mat4x4 rotateX = glm::rotate(glm::mat4(1.0f), dofs[0]->GetValue(), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4x4 rotateY = glm::rotate(glm::mat4(1.0f), dofs[1]->GetValue(), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4x4 rotateZ = glm::rotate(glm::mat4(1.0f), dofs[2]->GetValue(), glm::vec3(0.0f, 0.0f, 1.0f));

    local = translate * rotateZ * rotateY * rotateX;

    world = parentWorld * local;

    for (int i = 0; i < children.size(); i++) {
        children[i]->Update(world);
    }
}

bool Joint::Load(Tokenizer &token) {
    token.FindToken("{");
    while(1) {
        char temp[256];
        token.GetToken(temp);
        if(strcmp(temp,"offset")==0) {
            offset.x = token.GetFloat();
            offset.y = token.GetFloat();
            offset.z = token.GetFloat();

        } else if(strcmp(temp,"boxmin")==0){
            boxmin.x = token.GetFloat();
            boxmin.y = token.GetFloat();
            boxmin.z = token.GetFloat();

        } else if(strcmp(temp,"boxmax")==0){
            boxmax.x = token.GetFloat();
            boxmax.y = token.GetFloat();
            boxmax.z = token.GetFloat();

        } else if(strcmp(temp,"rotxlimit")==0){
            float mn = token.GetFloat();
            float mx = token.GetFloat();
            dofs[0]->SetMinMax(mn, mx);

        } else if(strcmp(temp,"rotylimit")==0){
            float mn = token.GetFloat();
            float mx = token.GetFloat();
            dofs[1]->SetMinMax(mn, mx);

        } else if(strcmp(temp,"rotzlimit")==0){
            float mn = token.GetFloat();
            float mx = token.GetFloat();
            dofs[2]->SetMinMax(mn, mx);

        } else if(strcmp(temp,"pose")==0){
            dofs[0]->SetValue(token.GetFloat());
            dofs[1]->SetValue(token.GetFloat());
            dofs[2]->SetValue(token.GetFloat());

        } else if(strcmp(temp,"balljoint")==0) {
            Joint *jnt = new Joint;
            jnt->Load(token);
            AddChild(jnt);
        } else if(strcmp(temp,"}")==0){
            cube = new Cube(boxmin, boxmax);
            return true;
        } else {
            token.SkipLine(); // Unrecognized token
            return false;
        }
    }
}

void Joint::AddChild(Joint* child) {
    children.push_back(child);
}

void Joint::Draw(const glm::mat4& viewProjMtx, GLuint shader) {
    // Update the world matrix
    world = viewProjMtx * world;
    // Draw the box
    cube->draw(world, shader);
    // Draw every child joint
    for (int i = 0; i < children.size(); i++) {
        children[i]->Draw(viewProjMtx, shader);
    }
}