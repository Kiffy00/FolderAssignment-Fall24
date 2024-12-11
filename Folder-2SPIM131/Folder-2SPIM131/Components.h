#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <vector>
#include <string>
#include <glm/glm.hpp>

typedef unsigned int Entity;

struct PositionComponent {
    std::vector<glm::vec3> positions;
};

struct VelocityComponent {
    std::vector<glm::vec3> velocities;
};

struct LifetimeComponent {
    std::vector<float> lifetimes;
};

struct AppearanceComponent {
    std::vector<float> scales;
    std::vector<glm::vec3> colors;
};

struct EmitterComponent {
    // Multiple control points. The first is also the origin of the emitter.
    std::vector<std::vector<glm::vec3>> controlPoints;

    std::vector<float> emissionRates;
    std::vector<int> spawnShapes; // 0 = Sphere, 1 = Box
    std::vector<float> spawnRadii;
    std::vector<glm::vec3> spawnBoxHalfExtents;
    std::vector<float> initialLifetimes;
    std::vector<float> initialScales;
    std::vector<glm::vec3> initialColors;

    // Movement Basic operator
    std::vector<bool> movementBasicEnabled;
    std::vector<glm::vec3> movementBasicForce;

    std::vector<std::string> forceScripts;
    std::vector<float> spawnAccumulators;
};

#endif