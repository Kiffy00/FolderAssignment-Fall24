#ifndef VERTEX_H
#define VERTEX_H

#include <glm/vec3.hpp>

class Vertex3D {
public:
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;

    // Default constructor
    Vertex3D() : position(0.0f), color(0.0f), normal(0.0f) {}

    // Constructor for position, color, and normal
    Vertex3D(const glm::vec3& pos, const glm::vec3& col, const glm::vec3& norm = glm::vec3(0.0f))
        : position(pos), color(col), normal(norm) {}
};

#endif