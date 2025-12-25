-- Camera Controller Script
-- Handles camera movement and mouse look

camera_controller = {
    -- Movement settings
    movement_speed = 5.0,
    mouse_sensitivity = 0.1,
    
    -- Key bindings
    forward_key = "W",
    backward_key = "S", 
    left_key = "A",
    right_key = "D",
    up_key = "SPACE",
    down_key = "LSHIFT",
    
    -- Mouse settings
    mouse_look_button = "RIGHT_MOUSE",
    
    -- Internal state
    last_mouse_x = 0.0,
    last_mouse_y = 0.0,
    first_mouse = true
}

function camera_controller:init()
    print("Camera controller initialized")
    print("Controls:")
    print("  WASD - Move camera")
    print("  Hold Left Mouse + Move - Look around")
    print("  Space - Move up")
    print("  Left Shift - Move down")
end

function camera_controller:update(input_state, delta_time)
    local movement = {
        forward = 0.0,
        right = 0.0,
        up = 0.0
    }
    
    -- Process keyboard input
    if input_state.keys[self.forward_key] then
        movement.forward = movement.forward + 1.0
    end
    
    if input_state.keys[self.backward_key] then
        movement.forward = movement.forward - 1.0
    end
    
    if input_state.keys[self.right_key] then
        movement.right = movement.right + 1.0
    end
    
    if input_state.keys[self.left_key] then
        movement.right = movement.right - 1.0
    end
    
    if input_state.keys[self.up_key] then
        movement.up = movement.up + 1.0
    end
    
    if input_state.keys[self.down_key] then
        movement.up = movement.up - 1.0
    end
    
    -- Apply movement speed and delta time
    movement.forward = movement.forward * self.movement_speed * delta_time
    movement.right = movement.right * self.movement_speed * delta_time
    movement.up = movement.up * self.movement_speed * delta_time
    
    -- Process mouse look (only when left mouse button is held)
    local mouse_delta = {x = 0.0, y = 0.0}
    
    if input_state.mouse_buttons[self.mouse_look_button] then
        if self.first_mouse then
            self.last_mouse_x = input_state.mouse.x
            self.last_mouse_y = input_state.mouse.y
            self.first_mouse = false
        end
        
        mouse_delta.x = (input_state.mouse.x - self.last_mouse_x) * self.mouse_sensitivity
        mouse_delta.y = (input_state.mouse.y - self.last_mouse_y) * self.mouse_sensitivity
    else
        self.first_mouse = true
    end
    
    self.last_mouse_x = input_state.mouse.x
    self.last_mouse_y = input_state.mouse.y
    
    return {
        movement = movement,
        mouse_delta = mouse_delta,
        mouse_look_active = input_state.mouse_buttons[self.mouse_look_button] or false
    }
end

function camera_controller:cleanup()
    print("Camera controller cleaned up")
end

return camera_controller