-- Advanced script with deltaTime support
local totalTime = 0.0
local rotation = 0.0
local rotationSpeed = 2.0 -- radians per second
local initialized = false

function init()
    print("=== Advanced Script Initialization ===")
    totalTime = 0.0
    rotation = 0.0
    rotationSpeed = 2.0
    initialized = true
    print("Advanced script ready!")
end

function update(deltaTime)
    if not initialized then
        print("Warning: update called before init!")
        return
    end
    
    -- Use deltaTime for frame-rate independent animation
    if deltaTime then
        totalTime = totalTime + deltaTime
        rotation = rotation + (rotationSpeed * deltaTime)
        
        -- Keep rotation in 0-2π range
        if rotation > 2 * math.pi then
            rotation = rotation - 2 * math.pi
        end
        
        -- Print status every 2 seconds
        if math.floor(totalTime * 0.5) > math.floor((totalTime - deltaTime) * 0.5) then
            print(string.format("Time: %.2fs, Rotation: %.2f rad (%.1f°)", 
                  totalTime, rotation, math.deg(rotation)))
        end
    else
        -- Fallback for when deltaTime is not provided
        rotation = rotation + 0.016 -- Assume ~60 FPS
        totalTime = totalTime + 0.016
    end
end