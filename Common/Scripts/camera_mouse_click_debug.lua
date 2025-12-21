-- Скрипт управления камерой мышью с отладкой (только при нажатой левой кнопке)

print("=== Camera Mouse Click Debug Script Loaded ===")

-- Переменные состояния
local initialized = false
local camera_speed = 3.0
local mouse_sensitivity = 0.002
local yaw = -90.0  -- Начальный поворот (смотрим вперед)
local pitch = 0.0
local last_mouse_x = 0
local last_mouse_y = 0
local first_mouse = true
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
    print("  Левая кнопка мыши + движение - поворот камеры")
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
    
    -- Отладочный вывод каждые 60 кадров (примерно раз в секунду)
    if debug_counter % 60 == 0 then
        local mouse_x, mouse_y = getMousePosition()
        local left_pressed = isMouseButtonPressed(MOUSE_LEFT())
        print("Mouse position:", mouse_x, mouse_y, "Left button:", left_pressed)
    end
    
    -- Управление поворотом мышью только при нажатой левой кнопке
    if isMouseButtonPressed(MOUSE_LEFT()) then
        -- Получаем текущую позицию мыши относительно окна
        local mouse_x, mouse_y = getMousePosition()
        
        if first_mouse then
            last_mouse_x = mouse_x
            last_mouse_y = mouse_y
            first_mouse = false
            print("Первое нажатие левой кнопки мыши в позиции:", mouse_x, mouse_y)
        end
        
        -- Вычисляем дельту движения мыши
        local mouse_dx = mouse_x - last_mouse_x
        local mouse_dy = last_mouse_y - mouse_y  -- Инвертируем Y для правильного направления
        
        last_mouse_x = mouse_x
        last_mouse_y = mouse_y
        
        -- Обновляем углы поворота только если есть движение
        if mouse_dx ~= 0 or mouse_dy ~= 0 then
            print("Движение мыши при нажатой левой кнопке:", mouse_dx, mouse_dy)
            
            yaw = yaw + mouse_dx * mouse_sensitivity * 50.0
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
    else
        -- Сбрасываем флаг первого движения мыши когда кнопка не нажата
        if not first_mouse then
            print("Левая кнопка мыши отпущена, сброс отслеживания")
            first_mouse = true
        end
    end
end

function render()
    -- Дополнительная логика рендеринга (если нужна)
end

print("=== Camera Mouse Click Debug Script Ready ===")