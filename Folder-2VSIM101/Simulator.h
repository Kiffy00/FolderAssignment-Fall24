#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>
#include <memory>
#include "Ball.h"
#include "Mesh.h"

class Simulator {
public:
    Simulator();  // Default constructor
    Simulator(const Mesh3D& terrainMesh);

    void setTerrain(const Mesh3D& terrainMesh);

    float getTerrainHeight(float x, float z) const;

    glm::vec3 getTerrainNormal(float x, float z) const;

    void addBall(const glm::vec3& position, const glm::vec3& initialVelocity, float radius);

    void update(float deltaTime);

    const std::vector<std::shared_ptr<Ball>>& getBalls() const;

private:
    std::vector<std::shared_ptr<Ball>> balls;
    const Mesh3D* terrainMesh;

    void computeAcceleration(Ball& ball);
};

#endif