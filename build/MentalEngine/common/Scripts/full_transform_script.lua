-- Full transform script - controls position, rotation, and scale
local time = 0.0
local rotationSpeed = 1.0  -- radians per second
local scaleSpeed = 0.5     -- scale oscillation speed
local moveSpeed = 0.3      -- movement speed

-- Current transform values
local currentRotation = {x = 0.0, y = 0.0, z = 0.0}
local currentPosition = {x = 0.0, y = 0.0, z = 0.0}
local currentScale = {x = 1.0, y = 1.0, z = 1.0}

function init()
    print("=== Full Transform Script Initialized ===")
    time = 0.0
    
    -- Initialize transforms
    currentRotation = {x = 0.0, y = 0.0, z = 0.0}
    currentPosition = {x = 0.0, y = 0.0, z = 0.0}
    currentScale = {x = 1.0, y = 1.0, z = 1.0}
    
    print("Object will rotate, scale, and move!")
end

function update()
    -- Increment time (assuming ~60 FPS, so 1/60 per frame)
    time = time + 0.016
    
    -- Update rotation (rotate around Y axis)
    currentRotation.y = time * rotationSpeed
    
    -- Update scale (oscillate between 0.5 and 1.5)
    local scaleValue = 1.0 + 0.5 * math.sin(time * scaleSpeed)
    currentScale.x = scaleValue
    currentScale.y = scaleValue
    currentScale.z = scaleValue
    
    -- Update position (move in a circle)
    currentPosition.x = math.sin(time * moveSpeed) * 0.5
    currentPosition.z = math.cos(time * moveSpeed) * 0.5
    
    -- Print status every few seconds
    if math.floor(time * 0.5) > math.floor((time - 0.016) * 0.5) then
        print(string.format("Time: %.1fs | Rot: %.1f° | Scale: %.2f | Pos: (%.2f, %.2f, %.2f)", 
              time, math.deg(currentRotation.y), scaleValue, 
              currentPosition.x, currentPosition.y, currentPosition.z))
    end
end

-- Functions to get transform values
function getRotation()
    return currentRotation.y  -- Return Y rotation for now
end

function getPosition()
    return currentPosition.x, currentPosition.y, currentPosition.z
end

function getScale()
    return currentScale.x, currentScale.y, currentScale.z
end