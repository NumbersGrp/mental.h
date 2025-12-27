-- Rotate script with init and update functions
local rotationY = 0.0
local rotationSpeed = 0.02

function init()
    print("Rotation script initialized!")
    rotationY = 0.0
    rotationSpeed = 0.02
end

function update()
    rotationY = rotationY + rotationSpeed
    
    -- Keep rotation in 0-2π range
    if rotationY > 2 * math.pi then
        rotationY = rotationY - 2 * math.pi
    end
    
    -- Print rotation value occasionally
    if math.floor(rotationY * 50) % 100 == 0 then
        print("Rotation Y: " .. string.format("%.2f", rotationY) .. " rad (" .. string.format("%.1f", math.deg(rotationY)) .. "°)")
    end
end

function getRotation()
    return rotationY
end