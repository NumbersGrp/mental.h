-- Rotate script with init and update functions
local rotationY = 0.0
local rotationSpeed = 0.02
local characterBody = require "./common/Scripts/CharacterBody2D"

function init()
    print("Rotation script initialized!")
end

function update()
    print("Rotating")
    print(characterBody.getPosition())
end

function getRotation()
    return rotationY
end