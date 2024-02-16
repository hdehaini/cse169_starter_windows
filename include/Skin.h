#pragma once

#include "core.h"
#include "Tokenizer.h"
#include "Vertex.h"
#include "Triangle.h"
#include "Skeleton.h"
#include <vector>
#include "glm/gtx/string_cast.hpp"

class Skin {
public:
    Skin();
    ~Skin();

    void load(const char* filename);
    void update();
    void draw(const glm::mat4& viewProjMtx, GLuint shader, bool useGlm);
    void glmInit();
    void drawGlm(const glm::mat4& viewProjMtx, GLuint shader);

    std::vector<glm::mat4> bindingInverses;
    std::vector<Vertex*> vertices;
    // std::vector<Triangle*> triangles;
    std::vector<unsigned int> indices;
    Skeleton* skeleton;
    GLuint meshVAO;
    GLuint positionVBO;
    GLuint normalVBO;
    GLuint ebo;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> positionsLocal;
    std::vector<glm::vec3> normalsLocal;


private:
    glm::vec3 color;
    glm::mat4 model;
};