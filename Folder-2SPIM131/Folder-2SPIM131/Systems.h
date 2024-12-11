#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "ECS.h"
#include "WorldObject.h"
#include <memory>

// Forward declarations
class Renderer;
class Camera;

// Forward declaration for Lua
extern "C" {
    typedef struct lua_State lua_State;
}

// Emission System
class EmissionSystem {
public:
    void update(float dt);
};

// Update System
class UpdateSystem {
public:
    void update(float dt);
};

// Force System (Lua Integration)
class ForceSystem {
public:
    ForceSystem();
    ~ForceSystem();
    void applyForces(float dt);
    void registerFunctions();

private:
    lua_State* L;
};

// Collision System
class CollisionSystem {
public:
    void handleCollisions();
};

// Render System
class RenderSystem {
public:
    RenderSystem(Renderer* renderer, const Camera* camera);
    ~RenderSystem();
    void render();

private:
    Renderer* renderer;
    const Camera* camera;
    std::shared_ptr<WorldObject> sphereObject;
};

// Lua-C++ binding functions
static int l_getEmitterControlPoint(lua_State* L);
static int l_getParticleCount(lua_State* L);
static int l_getParticlePosition(lua_State* L);
static int l_getParticleVelocity(lua_State* L);
static int l_setParticleVelocity(lua_State* L);

#endif