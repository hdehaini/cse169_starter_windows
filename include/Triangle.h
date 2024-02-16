#pragma once

#include "core.h"
#include "Vertex.h"

class Triangle {
public:
    Triangle();
    ~Triangle();

    void setVerticies(Vertex v1, Vertex v2, Vertex v3);

    Vertex v1;
    Vertex v2;
    Vertex v3;

private:
};