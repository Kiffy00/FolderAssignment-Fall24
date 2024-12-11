-- Twist any active XY-velocity
function applyForces(dt, emitterIndex)
    local count = getParticleCount()
    
    local rotationSpeed = math.rad(360)
    local angle = rotationSpeed * dt
    local cosA = math.cos(angle)
    local sinA = math.sin(angle)
    
    for i = 0, count - 1 do
        local pEmitter = getParticleEmitterIndex(i)
        if pEmitter == emitterIndex then
            local vx, vy, vz = getParticleVelocity(i)
            
            local newVx = vx * cosA - vz * sinA
            local newVy = vy
            local newVz = vx * sinA + vz * cosA
            
            setParticleVelocity(i, newVx, newVy, newVz)
        end
    end
end