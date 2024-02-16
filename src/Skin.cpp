#include "Skin.h"
#include <iostream>

Skin::Skin() {
    color = glm::vec3(0.0f, 0.0f, 0.7f);
    model = glm::mat4(1.0f);
    // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    glGenVertexArrays(1, &meshVAO);
    glGenBuffers(1, &positionVBO);
    glGenBuffers(1, &normalVBO);
}

Skin::~Skin() {
    for(Vertex* vertex : vertices){
        delete vertex;
    }
    for(unsigned int i = 0; i < indices.size(); i++){
        indices.pop_back();
    }
    for(glm::vec3 pos : positions){
        positions.pop_back();
    }
    for(glm::vec3 norm : normals){
        normals.pop_back();
    }
    for(glm::vec3 pos : positionsLocal){
        positionsLocal.pop_back();
    }
    for(glm::vec3 norm : normalsLocal){
        normalsLocal.pop_back();
    }
    glDeleteBuffers(1, &positionVBO);
    glDeleteBuffers(1, &normalVBO);
    glDeleteVertexArrays(1, &meshVAO);
    glDeleteBuffers(1, &ebo);
}

void Skin::load(const char* file) {
    Tokenizer token;
    if (! token.Open(file) ) {
        std::cerr << "Error: Could not open " << file << std::endl;
        return;
    }
    char temp[256];
    token.GetToken(temp);
    if (strcmp(temp, "positions") != 0) {
        std::cout << "success pos" << std::endl;
    }
    int numPositions = token.GetInt();

    token.SkipLine();
    // Read in the positions
    for (int i = 0; i < numPositions; i++) {
        Vertex *currVertex = new Vertex();
        glm::vec3 pos;
        pos.x = token.GetFloat();
        pos.y = token.GetFloat();
        pos.z = token.GetFloat();

        // Set the position of the vertex
        currVertex->setPos(pos);
        positionsLocal.push_back(pos);
        vertices.push_back(currVertex);
    }
    std::cout << "Num Vertices: " << vertices.size() << std::endl;

    // Read in the normals
    // token.GetToken(temp);
    // if(strcmp(temp, "normals") != 0){
    //     std::cout << "success norm" << std::endl;
    // }
    token.FindToken("normals");
    int numNormals = token.GetInt();
    token.SkipLine();
    for (int i = 0; i < numNormals; i++) {
        glm::vec3 norm;
        norm.x = token.GetFloat();
        norm.y = token.GetFloat();
        norm.z = token.GetFloat();

        // Set the normal of the vertex
        vertices[i]->setNormal(norm);
        normalsLocal.push_back(norm);
    }
    std::cout << "Num Normals: " << numNormals << std::endl;

    // Read in the skinweights
    token.FindToken("skinweights");
    int numSkinWeights = token.GetInt();
    token.SkipLine();
    for (int i = 0; i < numSkinWeights; i++) {
        int numJoints = token.GetInt();

        // Make a temporary vector to store the joint indices and weights
        std::vector<int> jointIndices;
        std::vector<float> jointWeights;
        for (int j = 0; j < numJoints; j++) {

            // Read in the joint index and weight
            int jointIndex = token.GetInt();
            float jointWeight = token.GetFloat();
            jointIndices.push_back(jointIndex);
            jointWeights.push_back(jointWeight);
        }

        // Set the skin weights of the vertex
        vertices[i]->setSkinWeights(jointIndices, jointWeights);
    }

    std::cout << "Num Skin Weights: " << numSkinWeights << std::endl;

    // Read in the triangles
    token.FindToken("triangles");
    int numTriangles = token.GetInt();
    token.SkipLine();
    for (int i = 0; i < numTriangles; i++) {
        int index1 = token.GetInt();
        int index2 = token.GetInt();
        int index3 = token.GetInt();

        // Set the indices of the triangles
        indices.push_back(index1);
        indices.push_back(index2);
        indices.push_back(index3);
    }

    std::cout << "Num Triangles: " << numTriangles << std::endl;

    // Read in the bindings
    token.FindToken("bindings");
    int numBindings = token.GetInt();
    token.SkipLine();
    glm::mat4 bCheck;
    float x, y, z;
    for (int i = 0; i < numBindings; i++) {
        token.FindToken("matrix");
        token.SkipLine();
        glm::mat4 bindings = glm::mat4(1.0f);
        for (int j = 0; j < 4; j++) {
            x = token.GetFloat();
            y = token.GetFloat();
            z = token.GetFloat();
            // bindings[j] = glm::vec4(token.GetFloat(), token.GetFloat(), token.GetFloat(), 0.f);
            bindings[j] = glm::vec4(x, y, z, 0.f);
            // bindings[0][j] = token.GetFloat();
            // bindings[1][j] = token.GetFloat();
            // bindings[2][j] = token.GetFloat();
            // bindings[3][j] = token.GetFloat();
        }
        bindings[3][3] = 1.0f;
        if (i == 0) {
            bCheck = bindings;
        }

        // Set the binding inverse of the vertex
        bindingInverses.push_back(glm::inverse(bindings));

        token.FindToken("}"); // end of matrix
        
    }

    std::cout << "Num Bindings: " << bindingInverses.size() << std::endl;

    std::cout << "Num Vertices: " << vertices.size() << std::endl;
    
    positions = positionsLocal;
    normals = normalsLocal;
    assert(positionsLocal.size() == positions.size());
    assert(normalsLocal.size() == normals.size());
    token.Close();

    // debug
    std::cout << "CHECK THIS: " << glm::to_string(bCheck) << std::endl;

}

void Skin::update() {
    if(skeleton == nullptr){
        return;
    }

    // Compute skinning matrix for each joint 𝐌𝑖 = 𝐖𝑖 ⋅ 𝐁𝑖−1
    std::vector<glm::mat4> skms;

    for(int i = 0; i < skeleton->allJoints.size(); i++){
        glm::mat4 skm = skeleton->GetWorldMatrix(i) * bindingInverses[i];
        
        skms.push_back(skm);
    }

    // std::cout << "Num skms: " << skms.size() << std::endl;

    glm::vec4 pos, norm, skinnedPos, skinnedNorm;
    // Compute blended world space positions & normals  
    for(int j = 0; j < vertices.size(); j++){
        pos = glm::vec4(positionsLocal[j], 1.0f);
        norm = glm::vec4(normalsLocal[j], 0.0f);

        skinnedPos = glm::vec4(0.0f); 
        skinnedNorm = glm::vec4(0.0f);

        std::vector<int> jointIndices = vertices[j]->getJointIndices();
        std::vector<float> jointWeights = vertices[j]->getJointWeights();
        // Gettting the joint indices and weights
        for(int i = 0; i < jointIndices.size(); i++){
            int jointIndex = jointIndices[i];
            float weight = jointWeights[i];
            // std::cout << skms[jointIndex][0][0] << " " << skms[jointIndex][0][1] << " " << skms[jointIndex][0][2] << " " << skms[jointIndex][0][3] << std::endl;

            // Compute the skinned position and normal
            skinnedPos += weight * skms[jointIndex] * pos;
            skinnedNorm += weight * skms[jointIndex] * norm;
        }

        // Set the final position and normal
        glm::vec4 finalPos = skinnedPos;
        glm::vec4 finalNorm = glm::normalize(skinnedNorm);
        
        // vertex->setPos(glm::vec3(finalPos));
        // vertex->setNormal(glm::vec3(finalNorm));
        positions[j] = glm::vec3(finalPos);
        normals[j] = glm::vec3(finalNorm);
    }
    // std::cout << "Vertex 0 position: " << vertices[0]->position.x << " " << vertices[0]->position.y << " " << vertices[0]->position.z << std::endl;
}

void Skin::glmInit(){
    // Bind to the VAO.
    glBindVertexArray(meshVAO);

    // Bind to the first VBO - We will use it to store the vertices
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    // std::vector<glm::vec3> positions;
    // std::vector<glm::vec3> normals;
    // for(Vertex* vertex : vertices){
    //     positions.push_back(vertex->position);
    //     normals.push_back(vertex->normal);
    // }
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Bind to the second VBO - We will use it to store the normals
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Generate EBO, bind the EBO to the bound VAO and send the data
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);
    
    // Unbind the VBOs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Skin::drawGlm(const glm::mat4& viewProjMtx, GLuint shader) {
    // actiavte the shader program
    glUseProgram(shader);

    // get the locations and send the uniforms to the shader
    glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
    glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

    // Bind the VAO
    glBindVertexArray(meshVAO);

    // draw the points using triangles, indexed with the EBO
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // Unbind the VAO and shader program
    glBindVertexArray(0);
    glUseProgram(0);
}

void Skin::draw(const glm::mat4& viewProjMtx, GLuint shader, bool useGlm) {
    if(useGlm){
        Skin::glmInit();
        Skin::drawGlm(viewProjMtx, shader);
    }
}

