#include "PrimitiveGenerator.h"
#include "corecrt_math_defines.h"

Mesh3D PrimitiveGenerator::createPlane(float width, float depth, const glm::vec3& color) {
    Mesh3D mesh;
    float x = width / 2.0f;
    float z = depth / 2.0f;

    // Vertices
    std::vector<Vertex3D> vertices = {
        {{-x, 0.0, -z}, color},
        {{ x, 0.0, -z}, color},
        {{ x, 0.0,  z}, color},
        {{-x, 0.0,  z}, color}
    };

    // Indices for two triangles that make up the rectangle
    std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0
    };

    mesh.addVertices(vertices);
    mesh.addIndices(indices);
    return mesh;
}

Mesh3D PrimitiveGenerator::createCube(float sideLength, const glm::vec3& color) {
    Mesh3D mesh;
    float halfSide = sideLength / 2.0f;

    // Define vertices
    std::vector<Vertex3D> vertices = {
        Vertex3D(glm::vec3(-halfSide, -halfSide, -halfSide), color),
        Vertex3D(glm::vec3(halfSide, -halfSide, -halfSide), color),
        Vertex3D(glm::vec3(halfSide, halfSide, -halfSide), color),
        Vertex3D(glm::vec3(-halfSide, halfSide, -halfSide), color),
        Vertex3D(glm::vec3(-halfSide, -halfSide, halfSide), color),
        Vertex3D(glm::vec3(halfSide, -halfSide, halfSide), color),
        Vertex3D(glm::vec3(halfSide, halfSide, halfSide), color),
        Vertex3D(glm::vec3(-halfSide, halfSide, halfSide), color)
    };

    // 12 triangles, two per face
    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0, // Front face
        1, 5, 6, 6, 2, 1, // Right face
        7, 6, 5, 5, 4, 7, // Back face
        4, 0, 3, 3, 7, 4, // Left face
        4, 5, 1, 1, 0, 4, // Bottom face
        3, 2, 6, 6, 7, 3  // Top face
    };

    mesh.addVertices(vertices);
    mesh.addIndices(indices);
    return mesh;
}

Mesh3D PrimitiveGenerator::createCircle(float radius, int segments, const glm::vec3& color) {
    Mesh3D mesh;
    std::vector<Vertex3D> vertices;
    std::vector<unsigned int> indices;

    // Center vertex
    vertices.push_back(Vertex3D({ 0.0f, 0.0f, 0.0f }, color));

    // Angle between vertices
    float angle = 2.0f * M_PI / segments;

    for (int i = 0; i <= segments; i++) {
        float x = radius * cosf(angle * i);
        float y = radius * sinf(angle * i);
        vertices.push_back(Vertex3D({ x, y, 0.0f }, color));
        if (i > 0) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }
    }

    mesh.addVertices(vertices);
    mesh.addIndices(indices);
    return mesh;
}

Mesh3D PrimitiveGenerator::createCylinder(float radius, float height, int segments, const glm::vec3& color) {
    Mesh3D mesh;
    std::vector<Vertex3D> vertices;
    std::vector<unsigned int> indices;

    float halfHeight = height / 2.0f;
    float angleStep = 2.0f * M_PI / segments;

    // Top and bottom center vertices
    vertices.push_back(Vertex3D({ 0.0f, halfHeight, 0.0f }, color)); // Top center
    vertices.push_back(Vertex3D({ 0.0f, -halfHeight, 0.0f }, color)); // Bottom center

    for (int i = 0; i <= segments; ++i) {
        float x = radius * cosf(i * angleStep);
        float z = radius * sinf(i * angleStep);
        vertices.push_back(Vertex3D({ x, halfHeight, z }, color)); // Top rim
        vertices.push_back(Vertex3D({ x, -halfHeight, z }, color)); // Bottom rim

        if (i > 0) {
            int base = 2 + 2 * (i - 1);
            indices.push_back(base);
            indices.push_back(base + 1);
            indices.push_back(base + 2);

            indices.push_back(base + 1);
            indices.push_back(base + 3);
            indices.push_back(base + 2);
        }
    }

    mesh.addVertices(vertices);
    mesh.addIndices(indices);
    return mesh;
}

Mesh3D PrimitiveGenerator::createSphere(float radius, int rings, int sectors, const glm::vec3& color) {
    Mesh3D mesh;
    std::vector<Vertex3D> vertices;
    std::vector<unsigned int> indices;

    float const R = 1.0f / (float)(rings - 1);
    float const S = 1.0f / (float)(sectors - 1);
    int r, s;

    for (r = 0; r < rings; r++) {
        for (s = 0; s < sectors; s++) {
            float const y = sin(-M_PI_2 + M_PI * r * R);
            float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
            float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

            vertices.push_back(Vertex3D({ x * radius, y * radius, z * radius }, color));

            int curRow = r * sectors;
            int nextRow = (r + 1) * sectors;
            indices.push_back(curRow + s);
            indices.push_back(nextRow + s);
            indices.push_back(nextRow + (s + 1));

            indices.push_back(curRow + s);
            indices.push_back(nextRow + (s + 1));
            indices.push_back(curRow + (s + 1));
        }
    }

    mesh.addVertices(vertices);
    mesh.addIndices(indices);
    return mesh;
}
