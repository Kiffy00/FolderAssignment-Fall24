#ifndef ECS_H
#define ECS_H

#include "Components.h"

// Particle ECS Global Storage
struct ECS {
    // Entities
    std::vector<Entity> particleEntities;
    std::vector<Entity> emitterEntities;

    // Components
    PositionComponent particlePositions;
    VelocityComponent particleVelocities;
    LifetimeComponent particleLifetimes;
    AppearanceComponent particleAppearance;

    EmitterComponent emitterData;
    std::vector<unsigned int> particleEmitterIndices;

    // Entity Management
    Entity createEmitter();
    void removeEmitter(size_t index);
    Entity createParticle();
    void removeParticle(size_t index);
};

extern ECS g_ecs;

#endif