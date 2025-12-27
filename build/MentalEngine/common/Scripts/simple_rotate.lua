-- Simple rotation script - just rotates around Y axis
local rotationY = 0.0

function init()
    print("Simple rotation script started!")
    rotationY = 0.0
end

function update()
    -- Increase rotation by 0.02 radians per frame (~1.15 degrees)
    rotationY = rotationY + 0.02
    
    -- Keep rotation in reasonable range
    if rotationY > 6.28 then  -- 2 * PI
        rotationY = 0.0
        print("Completed full rotation!")
    end
end

function getRotation()
    return rotationY
end