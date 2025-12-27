-- Example script demonstrating init and update functions
local frameCount = 0
local startTime = 0
local rotation = 0.0
local scale = 1.0
local scaleDirection = 1

function init()
    print("=== Script Initialization ===")
    print("Setting up initial values...")
    frameCount = 0
    startTime = os.time()
    rotation = 0.0
    scale = 1.0
    scaleDirection = 1
    print("Script initialized successfully!")
end

function update()
    frameCount = frameCount + 1
    
    -- Update rotation
    rotation = rotation + 0.02
    if rotation > 2 * math.pi then
        rotation = rotation - 2 * math.pi
    end
    
    -- Update scale with oscillation
    scale = scale + (0.01 * scaleDirection)
    if scale > 1.5 then
        scaleDirection = -1
    elseif scale < 0.5 then
        scaleDirection = 1
    end
    
    -- Print status every 60 frames (approximately 1 second at 60 FPS)
    if frameCount % 60 == 0 then
        local currentTime = os.time()
        local elapsed = currentTime - startTime
        print(string.format("Frame: %d, Time: %ds, Rotation: %.2f, Scale: %.2f", 
              frameCount, elapsed, rotation, scale))
    end
end

-- Optional cleanup function (not called automatically yet, but could be added)
function cleanup()
    print("Script cleanup called")
end