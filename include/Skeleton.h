#pragma once

#include "Joint.h"
#include "Tokenizer.h"

class Skeleton {
public:
    Skeleton();
    ~Skeleton();

    bool Load(const char *file);
    void Update(glm::mat4 parentWorld);
    void Draw(const glm::mat4& viewProjMtx, GLuint shader);

private:
    Joint* root;
};