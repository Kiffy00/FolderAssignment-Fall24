#ifndef BALL_H
#define BALL_H

#include "WorldObject.h"
#include "PrimitiveGenerator.h"
#include <glm/glm.hpp>

class Simulator;

class Ball : public WorldObject {
public:
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float mass;
    float radius;

    Ball(float radius, const glm::vec3& position, const glm::vec3& initialVelocity);

    void update(float deltaTime, const Simulator& simulator);
};

#endif