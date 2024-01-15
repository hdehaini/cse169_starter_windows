#include "Joint.h"
#include "Tokenizer.h"

class Skeleton {
public:
    Skeleton();
    ~Skeleton();

    bool Load(const char *file);
    void Update();
    void Draw();

private:
    Joint* root;
};