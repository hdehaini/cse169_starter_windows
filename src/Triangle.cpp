#include "Triangle.h"

Triangle::Triangle() {
}

Triangle::~Triangle() {
    delete &v1;
    delete &v2;
    delete &v3;
}

void Triangle::setVerticies(Vertex vt1, Vertex vt2, Vertex vt3) {
    v1 = vt1;
    v2 = vt2;
    v3 = vt3;
}