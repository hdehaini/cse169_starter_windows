#include "glm/gtx/euler_angles.hpp"
#include "./DOF.h"
#include "./Tokenizer.h"
#include <vector>

class Joint {
public:
    Joint();
    ~Joint();

    void Update();
    void Load(Tokenizer &t);
    void AddChild(Joint* child);
    void Draw();

private:
    glm::mat4 local;
    glm::mat4 world;
    std::vector<DOF*> dofs;
    std::vector<Joint*> children;
};