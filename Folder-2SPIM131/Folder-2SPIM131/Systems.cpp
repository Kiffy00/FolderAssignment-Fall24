#include "Systems.h"
#include "PrimitiveGenerator.h"
#include "WorldObject.h"
#include "Renderer.h"
#include "Camera.h"
#include <glm/gtc/random.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

extern "C" {
#include "lua54/include/lua.h"
#include "lua54/include/lauxlib.h"
#include "lua54/include/lualib.h"
}

// Link to Lua library
#ifdef _WIN32
#pragma comment(lib, "lua54/lua54.lib")
#endif

// External ECS
extern ECS g_ecs;

// Debug flag (large performance impact)
static constexpr bool DEBUG_PRINT = false;

// Emission System Implementation
void EmissionSystem::update(float dt) {
    if (DEBUG_PRINT) {
        std::cout << "[EmissionSystem] Updating with dt = " << dt << std::endl;
        std::cout << "[EmissionSystem] Number of emitters: " << g_ecs.emitterEntities.size() << std::endl;
    }

    for (size_t i = 0; i < g_ecs.emitterEntities.size(); ++i) {
        float rate = g_ecs.emitterData.emissionRates[i];
        g_ecs.emitterData.spawnAccumulators[i] += rate * dt;

        int numToSpawn = (int)floor(g_ecs.emitterData.spawnAccumulators[i]);
        if (numToSpawn > 0) {
            g_ecs.emitterData.spawnAccumulators[i] -= numToSpawn;
        }
        // All emitters default to first control point as origin
        glm::vec3 origin = g_ecs.emitterData.controlPoints[i][0];

        for (int p = 0; p < numToSpawn; ++p) {
            glm::vec3 spawnPos = origin;
            if (g_ecs.emitterData.spawnShapes[i] == 0) {
                float radius = g_ecs.emitterData.spawnRadii[i];
                spawnPos += glm::sphericalRand(radius);
            } else {
                glm::vec3 halfExt = g_ecs.emitterData.spawnBoxHalfExtents[i];
                spawnPos += glm::vec3(
                    glm::linearRand(-halfExt.x, halfExt.x),
                    glm::linearRand(-halfExt.y, halfExt.y),
                    glm::linearRand(-halfExt.z, halfExt.z)
                );
            }

            Entity e = g_ecs.createParticle();
            g_ecs.particlePositions.positions.emplace_back(spawnPos);
            g_ecs.particleVelocities.velocities.emplace_back(glm::vec3(0.0f));
            g_ecs.particleLifetimes.lifetimes.emplace_back(g_ecs.emitterData.initialLifetimes[i]);
            g_ecs.particleAppearance.scales.emplace_back(g_ecs.emitterData.initialScales[i]);
            g_ecs.particleAppearance.colors.emplace_back(g_ecs.emitterData.initialColors[i]);

            // Record which emitter this particle came from
            g_ecs.particleEmitterIndices.push_back((unsigned int)i);
            if (DEBUG_PRINT) {
                std::cout << "[EmissionSystem] Spawned particle " << e
                    << " at position " << glm::to_string(spawnPos) << std::endl;
            }
        }
    }
}

// Update System Implementation
void UpdateSystem::update(float dt) {
    if (DEBUG_PRINT) {
        std::cout << "[UpdateSystem] Updating with dt = " << dt << std::endl;
        std::cout << "[UpdateSystem] Particle count before update: " << g_ecs.particleEntities.size() << std::endl;
    }

    // Apply movementBasicForce if enabled
    for (size_t ei = 0; ei < g_ecs.emitterEntities.size(); ++ei) {
        if (g_ecs.emitterData.movementBasicEnabled[ei]) {
            glm::vec3 force = g_ecs.emitterData.movementBasicForce[ei];
            for (size_t i = 0; i < g_ecs.particleEntities.size(); ++i) {
                g_ecs.particleVelocities.velocities[i] += force * dt;
            }
        }
    }

    // Update particle positions and lifetimes
    for (size_t i = 0; i < g_ecs.particleEntities.size();) {
        g_ecs.particlePositions.positions[i] += g_ecs.particleVelocities.velocities[i] * dt;
        g_ecs.particleLifetimes.lifetimes[i] -= dt;

        if (g_ecs.particleLifetimes.lifetimes[i] <= 0.0f) {
            if (DEBUG_PRINT) {
                std::cout << "[UpdateSystem] Removing dead particle at index " << i
                    << " with lifetime <= 0" << std::endl;
            }
            g_ecs.removeParticle(i);
        }
        else {
            ++i;
        }
    }

    if (DEBUG_PRINT) {
        std::cout << "[UpdateSystem] Particle count after update: " << g_ecs.particleEntities.size() << std::endl;
    }
}

// Force System Implementation incl. Lua
static int l_getEmitterControlPoint(lua_State* L) {
    int emitterIndex = (int)luaL_checknumber(L, 1);
    int controlPointIndex = (int)luaL_checknumber(L, 2);
    if (emitterIndex < 0 || emitterIndex >= (int)g_ecs.emitterData.controlPoints.size()) {
        luaL_error(L, "Invalid emitter index");
        return 0;
    }
    auto& cps = g_ecs.emitterData.controlPoints[emitterIndex];
    if (controlPointIndex < 0 || controlPointIndex >= (int)cps.size()) {
        luaL_error(L, "Invalid control point index");
        return 0;
    }
    glm::vec3 cp = cps[controlPointIndex];
    lua_pushnumber(L, cp.x);
    lua_pushnumber(L, cp.y);
    lua_pushnumber(L, cp.z);
    return 3;
}

static int l_getParticleCount(lua_State* L) {
    lua_pushnumber(L, (int)g_ecs.particleEntities.size());
    return 1;
}

static int l_getParticlePosition(lua_State* L) {
    int index = (int)luaL_checknumber(L, 1);
    if (index < 0 || index >= (int)g_ecs.particlePositions.positions.size()) {
        luaL_error(L, "Invalid particle index");
        return 0;
    }
    glm::vec3 pos = g_ecs.particlePositions.positions[index];
    lua_pushnumber(L, pos.x);
    lua_pushnumber(L, pos.y);
    lua_pushnumber(L, pos.z);
    return 3;
}

static int l_getParticleVelocity(lua_State* L) {
    int index = (int)luaL_checknumber(L, 1);
    if (index < 0 || index >= (int)g_ecs.particleVelocities.velocities.size()) {
        luaL_error(L, "Invalid particle index");
        return 0;
    }
    glm::vec3 vel = g_ecs.particleVelocities.velocities[index];
    lua_pushnumber(L, vel.x);
    lua_pushnumber(L, vel.y);
    lua_pushnumber(L, vel.z);
    return 3;
}

static int l_setParticleVelocity(lua_State* L) {
    int index = (int)luaL_checknumber(L, 1);
    float vx = (float)luaL_checknumber(L, 2);
    float vy = (float)luaL_checknumber(L, 3);
    float vz = (float)luaL_checknumber(L, 4);

    if (index < 0 || index >= (int)g_ecs.particleVelocities.velocities.size()) {
        luaL_error(L, "Invalid particle index");
        return 0;
    }

    g_ecs.particleVelocities.velocities[index] = glm::vec3(vx, vy, vz);
    return 0;
}

static int l_applyForceToAll(lua_State* L) {
    float fx = (float)luaL_checknumber(L, 1);
    float fy = (float)luaL_checknumber(L, 2);
    float fz = (float)luaL_checknumber(L, 3);

    glm::vec3 force(fx, fy, fz);
    for (size_t i = 0; i < g_ecs.particleEntities.size(); ++i) {
        g_ecs.particleVelocities.velocities[i] += force;
    }

    return 0;
}

static int l_getParticleEmitterIndex(lua_State* L) {
    int index = (int)luaL_checknumber(L, 1);
    if (index < 0 || index >= (int)g_ecs.particleEmitterIndices.size()) {
        luaL_error(L, "Invalid particle index");
        return 0;
    }
    lua_pushnumber(L, (double)g_ecs.particleEmitterIndices[index]);
    return 1;
}

ForceSystem::ForceSystem() {
    L = luaL_newstate();
    luaL_openlibs(L);
    registerFunctions();
}

ForceSystem::~ForceSystem() {
    if (L) {
        lua_close(L);
    }
}

void ForceSystem::registerFunctions() {
    lua_register(L, "getEmitterControlPoint", l_getEmitterControlPoint);
    lua_register(L, "getParticleCount", l_getParticleCount);
    lua_register(L, "getParticlePosition", l_getParticlePosition);
    lua_register(L, "getParticleVelocity", l_getParticleVelocity);
    lua_register(L, "setParticleVelocity", l_setParticleVelocity);
    lua_register(L, "applyForceToAll", l_applyForceToAll);
    lua_register(L, "getParticleEmitterIndex", l_getParticleEmitterIndex);
}

void ForceSystem::applyForces(float dt) {
    for (size_t i = 0; i < g_ecs.emitterEntities.size(); ++i) {
        if (!g_ecs.emitterData.movementBasicEnabled[i]) // Does not have movement basic enabled
            continue;

        std::string script = g_ecs.emitterData.forceScripts[i];
        if (script.empty()) continue;

        // Load and execute the script
        if (luaL_dofile(L, script.c_str()) != LUA_OK) {
            std::cerr << "Failed to load Lua script: " << script << "\n"
                << lua_tostring(L, -1) << std::endl;
            lua_pop(L, 1);
            continue;
        }

        // Call applyForces function if it exists
        lua_getglobal(L, "applyForces");
        if (lua_isfunction(L, -1)) {
            lua_pushnumber(L, dt);
            lua_pushnumber(L, (float)i);
            if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
                std::cerr << "Error running 'applyForces': " << lua_tostring(L, -1) << std::endl;
                lua_pop(L, 1);
            }
        }
        else {
            lua_pop(L, 1);
        }
    }
}

// Collision System Implementation
void CollisionSystem::handleCollisions() {
    // Simple ground surface collision at y=0
    for (size_t i = 0; i < g_ecs.particleEntities.size();) {
        if (g_ecs.particlePositions.positions[i].y <= 0.0f) 
            g_ecs.removeParticle(i);
        else 
            ++i;
    }
}

// Render System Implementation
RenderSystem::RenderSystem(Renderer* renderer, const Camera* camera)
    : renderer(renderer), camera(camera)
{
    Mesh3D particleMesh = PrimitiveGenerator::createSphere(0.1f, 10, 10, glm::vec3(1.0f, 1.0f, 1.0f));
    sphereObject = std::make_shared<WorldObject>(particleMesh);
}

RenderSystem::~RenderSystem() { }

void RenderSystem::render() {
    size_t particleCount = g_ecs.particleEntities.size();
    if (DEBUG_PRINT) {
        std::cout << "[RenderSystem] Rendering " << particleCount << " particles." << std::endl;
    }

    for (size_t i = 0; i < particleCount; ++i) {
        sphereObject->position = g_ecs.particlePositions.positions[i];
        sphereObject->scale = glm::vec3(g_ecs.particleAppearance.scales[i]);
        const glm::vec3& col = g_ecs.particleAppearance.colors[i];

        if (DEBUG_PRINT) {
            std::cout << "[RenderSystem] Particle " << i
                << ": Position = " << glm::to_string(sphereObject->position)
                << ", Scale = " << glm::to_string(sphereObject->scale) << std::endl;
        }

        // Particle only shader color manipulation
        glUseProgram(renderer->getShaderProgram());
        glUniform3f(renderer->getColorLocation(), col.r, col.g, col.b);
        renderer->render(sphereObject, *camera);
        glUniform3f(renderer->getColorLocation(), 1.0f, 1.0f, 1.0f);
    }
}