-- Triangle Controller Script
-- This script controls triangle behavior

triangle = {
    name = "ColorfulTriangle",
    rotation_speed = 1.0,
    color_tint = {1.0, 1.0, 1.0},
    scale_factor = 1.0,
    pulse_speed = 2.0
}

function triangle:init()
    print("Triangle controller initialized: " .. self.name)
    self.time = 0.0
end

function triangle:update(deltaTime)
    self.time = self.time + deltaTime
    
    -- Rotate the triangle
    local rotation_y = self.time * self.rotation_speed
    
    -- Pulse the scale
    local pulse = 1.0 + 0.2 * math.sin(self.time * self.pulse_speed)
    self.scale_factor = pulse
    
    -- Cycle through colors
    self.color_tint[1] = 0.5 + 0.5 * math.sin(self.time * 0.5)
    self.color_tint[2] = 0.5 + 0.5 * math.sin(self.time * 0.7)
    self.color_tint[3] = 0.5 + 0.5 * math.sin(self.time * 0.9)
    
    return {
        rotation = {0.0, rotation_y, 0.0},
        scale = {self.scale_factor, self.scale_factor, self.scale_factor},
        color_tint = self.color_tint
    }
end

function triangle:cleanup()
    print("Triangle controller cleaned up")
end

return triangle