#include "Ball.h"
#include "Simulator.h"

Ball::Ball(float radius, const glm::vec3& position, const glm::vec3& initialVelocity)
    : WorldObject(PrimitiveGenerator::createSphere(radius, 20, 20, glm::vec3(1.0f, 0.0f, 0.0f)), position),
    velocity(initialVelocity),
    acceleration(0.0f),
    mass(1.0f),
    radius(radius)
{
}

void Ball::update(float deltaTime, const Simulator& simulator) {
    // Update velocity
    velocity += acceleration * deltaTime;

    // Adjust velocity to follow the terrain surface
    glm::vec3 normal = simulator.getTerrainNormal(position.x, position.z);
    if (glm::length(normal) > 0.0f) {
        normal = glm::normalize(normal);
        velocity = velocity - normal * glm::dot(velocity, normal);
    }

    // Update position
    position += velocity * deltaTime;

    // Ensure the ball stays on the terrain surface
    float terrainHeight = simulator.getTerrainHeight(position.x, position.z);
    position.y = terrainHeight + radius;
}
