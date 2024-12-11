function applyForces(dt, emitterIndex)
    local count = getParticleCount()
    local px, py, pz = getEmitterControlPoint(emitterIndex, 1)
    
    for i = 0, count-1 do
        local pEmitter = getParticleEmitterIndex(i)
        if pEmitter == emitterIndex then
            local x, y, z = getParticlePosition(i)
            local vx, vy, vz = getParticleVelocity(i)

            local dx = px - x
            local dy = py - y
            local dz = pz - z
			
            local length = math.sqrt(dx*dx + dy*dy + dz*dz)
            if length > 0.0001 then
                dx = dx / length
                dy = dy / length
                dz = dz / length
            end
			
            local factor = 2.0
            vx = vx + dx * factor * dt
            vy = vy + dy * factor * dt
            vz = vz + dz * factor * dt

            setParticleVelocity(i, vx, vy, vz)
        end
    end
end
