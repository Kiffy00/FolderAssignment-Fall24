local minForce = {x = -1.0, y = -1.0, z = -1.0}
local maxForce = {x = 1.0, y = 1.0, z = 1.0}
-- math.randomseed(os.time())

function applyForces(dt, emitterIndex)
    local count = getParticleCount()

    for i = 0, count - 1 do
        local pEmitter = getParticleEmitterIndex(i)
        if pEmitter == emitterIndex then
            local vx, vy, vz = getParticleVelocity(i)

            local fx = minForce.x + (maxForce.x - minForce.x) * math.random()
            local fy = minForce.y + (maxForce.y - minForce.y) * math.random()
            local fz = minForce.z + (maxForce.z - minForce.z) * math.random()
            local factor = 1.0
			
            vx = vx + fx * factor * dt
            vy = vy + fy * factor * dt
            vz = vz + fz * factor * dt

            setParticleVelocity(i, vx, vy, vz)
        end
    end
end