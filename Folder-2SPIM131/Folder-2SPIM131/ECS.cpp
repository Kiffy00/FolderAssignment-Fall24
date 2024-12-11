#include "ECS.h"

ECS g_ecs;

// Create and initialize an emitter with default values
Entity ECS::createEmitter() {
    Entity e = static_cast<Entity>(emitterEntities.size());
    emitterEntities.push_back(e);

    // Initialize origin control point
    emitterData.controlPoints.emplace_back();
    emitterData.controlPoints.back().push_back(glm::vec3(0.0f, 0.0f, 0.0f));

    emitterData.emissionRates.emplace_back(10.0f);
    emitterData.spawnShapes.emplace_back(0); // Sphere
    emitterData.spawnRadii.emplace_back(1.0f);
    emitterData.spawnBoxHalfExtents.emplace_back(glm::vec3(0.5f));
    emitterData.initialLifetimes.emplace_back(2.0f);
    emitterData.initialScales.emplace_back(0.1f);
    emitterData.initialColors.emplace_back(glm::vec3(1.0f, 1.0f, 1.0f));

    // Movement Basic operator defaults
    emitterData.movementBasicEnabled.emplace_back(false);
    emitterData.movementBasicForce.emplace_back(glm::vec3(0.0f));

    emitterData.forceScripts.emplace_back("force_random.lua");
    emitterData.spawnAccumulators.emplace_back(0.0f);

    return e;
}

void ECS::removeEmitter(size_t index) {
    if (index >= emitterEntities.size()) return;
    size_t last = emitterEntities.size() - 1;
    emitterEntities[index] = emitterEntities[last];
    emitterEntities.pop_back();

    emitterData.controlPoints[index] = emitterData.controlPoints[last];
    emitterData.controlPoints.pop_back();

    emitterData.emissionRates[index] = emitterData.emissionRates[last];
    emitterData.emissionRates.pop_back();

    emitterData.spawnShapes[index] = emitterData.spawnShapes[last];
    emitterData.spawnShapes.pop_back();

    emitterData.spawnRadii[index] = emitterData.spawnRadii[last];
    emitterData.spawnRadii.pop_back();

    emitterData.spawnBoxHalfExtents[index] = emitterData.spawnBoxHalfExtents[last];
    emitterData.spawnBoxHalfExtents.pop_back();

    emitterData.initialLifetimes[index] = emitterData.initialLifetimes[last];
    emitterData.initialLifetimes.pop_back();

    emitterData.initialScales[index] = emitterData.initialScales[last];
    emitterData.initialScales.pop_back();

    emitterData.initialColors[index] = emitterData.initialColors[last];
    emitterData.initialColors.pop_back();

    emitterData.movementBasicEnabled[index] = emitterData.movementBasicEnabled[last];
    emitterData.movementBasicEnabled.pop_back();

    emitterData.movementBasicForce[index] = emitterData.movementBasicForce[last];
    emitterData.movementBasicForce.pop_back();

    emitterData.forceScripts[index] = emitterData.forceScripts[last];
    emitterData.forceScripts.pop_back();

    emitterData.spawnAccumulators[index] = emitterData.spawnAccumulators[last];
    emitterData.spawnAccumulators.pop_back();
}

Entity ECS::createParticle() {
    Entity e = static_cast<Entity>(particleEntities.size());
    particleEntities.push_back(e);
    return e;
}

void ECS::removeParticle(size_t index) {
    if (index >= particleEntities.size()) return;
    size_t last = particleEntities.size() - 1;
    particleEntities[index] = particleEntities[last];
    particleEntities.pop_back();

    particlePositions.positions[index] = particlePositions.positions[last];
    particlePositions.positions.pop_back();

    particleVelocities.velocities[index] = particleVelocities.velocities[last];
    particleVelocities.velocities.pop_back();

    particleLifetimes.lifetimes[index] = particleLifetimes.lifetimes[last];
    particleLifetimes.lifetimes.pop_back();

    particleAppearance.scales[index] = particleAppearance.scales[last];
    particleAppearance.scales.pop_back();

    particleAppearance.colors[index] = particleAppearance.colors[last];
    particleAppearance.colors.pop_back();

    g_ecs.particleEmitterIndices[index] = g_ecs.particleEmitterIndices[last];
    g_ecs.particleEmitterIndices.pop_back();
}
