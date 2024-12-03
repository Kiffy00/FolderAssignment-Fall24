#include "Simulator.h"
#include "Barycentric.h"
#include <glm/gtx/normal.hpp>

Simulator::Simulator()
    : terrainMesh(nullptr) {}

Simulator::Simulator(const Mesh3D& terrainMesh)
    : terrainMesh(&terrainMesh) {}

void Simulator::setTerrain(const Mesh3D& terrainMesh) {
    this->terrainMesh = &terrainMesh;
}

float Simulator::getTerrainHeight(float x, float z) const {
    if (terrainMesh == nullptr) return 0.0f;

    const auto& vertices = terrainMesh->getVertices();
    const auto& indices = terrainMesh->getIndices();
    glm::vec2 pointXZ(x, z);

    for (size_t i = 0; i < indices.size(); i += 3) {
        const glm::vec3& v0 = vertices[indices[i]].position;
        const glm::vec3& v1 = vertices[indices[i + 1]].position;
        const glm::vec3& v2 = vertices[indices[i + 2]].position;

        glm::vec2 p0(v0.x, v0.z);
        glm::vec2 p1(v1.x, v1.z);
        glm::vec2 p2(v2.x, v2.z);

        glm::vec3 baryCoords = Barycentric::computeBarycentricCoordinates(pointXZ, p0, p1, p2);
        float a = baryCoords.x;
        float b = baryCoords.y;
        float c = baryCoords.z;

        if (a >= 0 && b >= 0 && c >= 0) { // Point is over this triangle
            float y = a * v0.y + b * v1.y + c * v2.y;
            return y;
        }
    }
    return 0.0f;
}

glm::vec3 Simulator::getTerrainNormal(float x, float z) const {
    if (terrainMesh == nullptr) return glm::vec3(0.0f, 1.0f, 0.0f);

    const auto& vertices = terrainMesh->getVertices();
    const auto& indices = terrainMesh->getIndices();
    glm::vec2 pointXZ(x, z);

    for (size_t i = 0; i < indices.size(); i += 3) {
        const glm::vec3& v0 = vertices[indices[i]].position;
        const glm::vec3& v1 = vertices[indices[i + 1]].position;
        const glm::vec3& v2 = vertices[indices[i + 2]].position;

        glm::vec2 p0(v0.x, v0.z);
        glm::vec2 p1(v1.x, v1.z);
        glm::vec2 p2(v2.x, v2.z);

        glm::vec3 baryCoords = Barycentric::computeBarycentricCoordinates(pointXZ, p0, p1, p2);
        float a = baryCoords.x;
        float b = baryCoords.y;
        float c = baryCoords.z;

        if (a >= 0 && b >= 0 && c >= 0) { // Point is over this triangle
            glm::vec3 normal = glm::triangleNormal(v0, v1, v2);
            if (glm::length(normal) > 0.0f) {
                return glm::normalize(normal);
            } else {
                return glm::vec3(0.0f, 1.0f, 0.0f);
            }
        }
    }

    return glm::vec3(0.0f, 1.0f, 0.0f); 
}

void Simulator::addBall(const glm::vec3& position, const glm::vec3& initialVelocity, float radius) {
    auto ball = std::make_shared<Ball>(radius, position, initialVelocity);
    balls.push_back(ball);
}

void Simulator::update(float deltaTime) {
    if (terrainMesh == nullptr) return;
    for (auto& ball : balls) {
        computeAcceleration(*ball);
        ball->update(deltaTime, *this);
    }
}

void Simulator::computeAcceleration(Ball& ball) {
    if (terrainMesh == nullptr) return;

    // 1. Identify the triangle under the ball using barycentric
    const auto& vertices = terrainMesh->getVertices();
    const auto& indices = terrainMesh->getIndices();

    glm::vec3 ballPos = ball.position;
    glm::vec2 ballXZ(ballPos.x, ballPos.z);

    bool foundTriangle = false;
    glm::vec3 normal;

    for (size_t i = 0; i < indices.size(); i += 3) {
        const glm::vec3& v0 = vertices[indices[i]].position;
        const glm::vec3& v1 = vertices[indices[i + 1]].position;
        const glm::vec3& v2 = vertices[indices[i + 2]].position;

        glm::vec2 p0(v0.x, v0.z);
        glm::vec2 p1(v1.x, v1.z);
        glm::vec2 p2(v2.x, v2.z);

        glm::vec3 baryCoords = Barycentric::computeBarycentricCoordinates(ballXZ, p0, p1, p2);
        float a = baryCoords.x;
        float b = baryCoords.y;
        float c = baryCoords.z;

        if (a >= 0 && b >= 0 && c >= 0) {
            foundTriangle = true;

            // Compute the normal vector at the contact point
            normal = glm::triangleNormal(v0, v1, v2);

            // Ensure the normal is normalized
            if (glm::length(normal) > 0.0f) {
                normal = glm::normalize(normal);
            } else {
                normal = glm::vec3(0.0f, 1.0f, 0.0f);
            }

            // Compute acceleration
            glm::vec3 g = glm::vec3(0.0f, -9.81f, 0.0f);

            // Acceleration is gravity projected onto the surface
            ball.acceleration = g - normal * glm::dot(g, normal);

            // Add friction (basic)
            float frictionCoefficient = 0.5f;
            glm::vec3 friction = -frictionCoefficient * ball.velocity;
            ball.acceleration += friction;

            break;
        }
    }

    if (!foundTriangle) { // Ball is not over the terrain, do gravity
        ball.acceleration = glm::vec3(0.0f, -9.81f, 0.0f);
    }
}

const std::vector<std::shared_ptr<Ball>>& Simulator::getBalls() const {
    return balls;
}