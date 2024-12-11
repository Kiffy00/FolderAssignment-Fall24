#ifndef PRIMITIVE_GENERATOR_H
#define PRIMITIVE_GENERATOR_H

#include "Mesh.h"
#include <glm/glm.hpp>

class PrimitiveGenerator {
public:
    // Generates a plane in the XZ plane
    static Mesh3D createPlane(float width, float depth, const glm::vec3& color);

    // Generates a simple cube
    static Mesh3D createCube(float sideLength, const glm::vec3& color);

    // Generates a circle in the XY plane
    static Mesh3D createCircle(float radius, int segments, const glm::vec3& color);

    // Generates a cylinder along the Y-axis
    static Mesh3D createCylinder(float radius, float height, int segments, const glm::vec3& color);

    // Generates a sphere
    static Mesh3D createSphere(float radius, int rings, int sectors, const glm::vec3& color);
};

#endif