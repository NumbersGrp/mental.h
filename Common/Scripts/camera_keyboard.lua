-- Скрипт управления камерой с клавиатуры

print("=== Camera Keyboard Control Script Loaded ===")

-- Переменные состояния
local initialized = false
local camera_speed = 3.0
local rotation_speed = 1.5
local mouse_sensitivity = 0.002

function init()
    print("Инициализация управления камерой с клавиатуры")
    
    -- Устанавливаем начальную позицию камеры
    setCameraPosition(0, 2, 5)
    setCameraTarget(0, 0, 0)
    setCameraUp(0, 1, 0)
    
    -- Настраиваем перспективу
    setPerspective(60.0, 16.0/9.0, 0.1, 100.0)
    
    initialized = true
    
    print("Камера готова к управлению")
    print("Управление:")
    print("  Стрелочки - поворот камеры")
    print("  WASD - движение камеры")
    print("  Позиция камеры:", getCameraPosition())
end

function update(deltaTime)
    if not initialized then
        return
    end
    
    local movement_delta = camera_speed * deltaTime
    local rotation_delta = rotation_speed * deltaTime
    
    -- Управление движением (WASD)
    if isKeyPressed(KEY_W()) then
        moveForward(movement_delta)
    end
    if isKeyPressed(KEY_S()) then
        moveBackward(movement_delta)
    end
    if isKeyPressed(KEY_A()) then
        moveLeft(movement_delta)
    end
    if isKeyPressed(KEY_D()) then
        moveRight(movement_delta)
    end
    
    -- Управление поворотом (стрелочки)
    if isKeyPressed(KEY_LEFT()) then
        rotateYaw(-rotation_delta)  -- Поворот влево
    end
    if isKeyPressed(KEY_RIGHT()) then
        rotateYaw(rotation_delta)   -- Поворот вправо
    end
    if isKeyPressed(KEY_UP()) then
        rotatePitch(-rotation_delta) -- Поворот вверх
    end
    if isKeyPressed(KEY_DOWN()) then
        rotatePitch(rotation_delta)  -- Поворот вниз
    end
    
    -- Дополнительные клавиши для движения вверх/вниз
    -- Можно использовать Q/E или Space/Shift
    -- if isKeyPressed(KEY_Q()) then
    --     moveUp(movement_delta)
    -- end
    -- if isKeyPressed(KEY_E()) then
    --     moveDown(movement_delta)
    -- end
end

function render()
    -- Дополнительная логика рендеринга (если нужна)
end

print("=== Camera Keyboard Control Script Ready ===")