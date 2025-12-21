-- Скрипт управления камерой мышью (отладочная версия)

print("=== Camera Mouse Debug Script Loaded ===")

-- Переменные состояния
local initialized = false
local camera_speed = 3.0
local mouse_sensitivity = 0.002
local yaw = -90.0  -- Начальный поворот (смотрим вперед)
local pitch = 0.0
local debug_counter = 0

function init()
    print("Инициализация отладочного управления камерой мышью")
    
    -- Устанавливаем начальную позицию камеры
    setCameraPosition(0, 0, 3)
    setCameraTarget(0, 0, 0)
    setCameraUp(0, 1, 0)
    
    -- Настраиваем перспективу
    setPerspective(45.0, 16.0/9.0, 0.1, 100.0)
    
    initialized = true
    
    print("Камера готова к управлению")
    print("Управление:")
    print("  Мышь - поворот камеры")
    print("  WASD - движение камеры")
    print("  Позиция камеры:", getCameraPosition())
end

function update(deltaTime)
    if not initialized then
        return
    end
    
    debug_counter = debug_counter + 1
    
    local movement_delta = camera_speed * deltaTime
    
    -- Управление движением (WASD)
    if isKeyPressed(KEY_W()) then
        moveForward(movement_delta)
        print("Движение вперед")
    end
    if isKeyPressed(KEY_S()) then
        moveBackward(movement_delta)
        print("Движение назад")
    end
    if isKeyPressed(KEY_A()) then
        moveLeft(movement_delta)
        print("Движение влево")
    end
    if isKeyPressed(KEY_D()) then
        moveRight(movement_delta)
        print("Движение вправо")
    end
    
    -- Управление поворотом мышью
    local mouse_dx, mouse_dy = getMouseDelta()
    
    -- Отладочный вывод каждые 60 кадров (примерно раз в секунду)
    if debug_counter % 60 == 0 then
        print("Mouse delta:", mouse_dx, mouse_dy)
        local mouse_x, mouse_y = getMousePosition()
        print("Mouse position:", mouse_x, mouse_y)
    end
    
    if mouse_dx ~= 0 or mouse_dy ~= 0 then
        print("Обнаружено движение мыши:", mouse_dx, mouse_dy)
        
        -- Обновляем углы поворота
        yaw = yaw + mouse_dx * mouse_sensitivity * 50.0  -- Масштабируем для удобства
        pitch = pitch + mouse_dy * mouse_sensitivity * 50.0
        
        -- Ограничиваем pitch чтобы не переворачиваться
        if pitch > 89.0 then
            pitch = 89.0
        elseif pitch < -89.0 then
            pitch = -89.0
        end
        
        print("Новые углы - yaw:", yaw, "pitch:", pitch)
        
        -- Вычисляем новое направление камеры
        local yaw_rad = yaw * 3.14159 / 180.0  -- Конвертируем в радианы
        local pitch_rad = pitch * 3.14159 / 180.0
        
        -- Используем наши зарегистрированные функции sin/cos
        local front_x = cos(yaw_rad) * cos(pitch_rad)
        local front_y = sin(pitch_rad)
        local front_z = sin(yaw_rad) * cos(pitch_rad)
        
        print("Направление камеры:", front_x, front_y, front_z)
        
        -- Получаем текущую позицию камеры
        local pos_x, pos_y, pos_z = getCameraPosition()
        
        -- Устанавливаем новую цель камеры
        setCameraTarget(pos_x + front_x, pos_y + front_y, pos_z + front_z)
        
        print("Новая цель камеры:", pos_x + front_x, pos_y + front_y, pos_z + front_z)
    end
end

function render()
    -- Дополнительная логика рендеринга (если нужна)
end

print("=== Camera Mouse Debug Script Ready ===")