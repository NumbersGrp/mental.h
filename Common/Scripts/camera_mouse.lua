-- Скрипт управления камерой мышью и клавиатурой

print("=== Camera Mouse Control Script Loaded ===")

-- Переменные состояния
local initialized = false
local camera_speed = 3.0
local mouse_sensitivity = 0.001
local yaw = -90.0  -- Начальный поворот (смотрим вперед)
local pitch = 0.0
local last_mouse_x = 0
local last_mouse_y = 0
local first_mouse = true

-- Векторы направления камеры (кэшируем для избежания пересчетов)
local front_x, front_y, front_z = 0.0, 0.0, -1.0
local right_x, right_y, right_z = 1.0, 0.0, 0.0
local up_x, up_y, up_z = 0.0, 1.0, 0.0

-- Функция для обновления векторов направления
local function updateCameraVectors()
    local yaw_rad = yaw * 3.14159 / 180.0
    local pitch_rad = pitch * 3.14159 / 180.0
    
    -- Вычисляем новое направление front
    front_x = cos(yaw_rad) * cos(pitch_rad)
    front_y = sin(pitch_rad)
    front_z = sin(yaw_rad) * cos(pitch_rad)
    
    -- Нормализуем front вектор
    local front_length = (front_x * front_x + front_y * front_y + front_z * front_z) ^ 0.5
    if front_length > 0 then
        front_x = front_x / front_length
        front_y = front_y / front_length
        front_z = front_z / front_length
    end
    
    -- Вычисляем right вектор (cross product front x world_up)
    local world_up_x, world_up_y, world_up_z = 0.0, 1.0, 0.0
    right_x = front_y * world_up_z - front_z * world_up_y
    right_y = front_z * world_up_x - front_x * world_up_z
    right_z = front_x * world_up_y - front_y * world_up_x
    
    -- Нормализуем right вектор
    local right_length = (right_x * right_x + right_y * right_y + right_z * right_z) ^ 0.5
    if right_length > 0 then
        right_x = right_x / right_length
        right_y = right_y / right_length
        right_z = right_z / right_length
    end
    
    -- Вычисляем up вектор (cross product right x front)
    up_x = right_y * front_z - right_z * front_y
    up_y = right_z * front_x - right_x * front_z
    up_z = right_x * front_y - right_y * front_x
end

function init()
    print("Инициализация управления камерой мышью")
    
    -- Устанавливаем начальную позицию камеры (еще дальше, чтобы видеть все объекты)
    setCameraPosition(0, 0, 8)  -- Отодвигаем камеру еще дальше
    setCameraUp(0, 1, 0)
    
    -- Настраиваем перспективу с более широким полем зрения
    setPerspective(60.0, 16.0/9.0, 0.1, 100.0)  -- Увеличиваем FOV до 60 градусов
    
    -- Инициализируем векторы направления
    updateCameraVectors()
    
    -- Устанавливаем начальную цель камеры
    local pos_x, pos_y, pos_z = getCameraPosition()
    setCameraTarget(pos_x + front_x, pos_y + front_y, pos_z + front_z)
    
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
    
    local movement_delta = camera_speed * deltaTime
    local camera_moved = false
    
    -- Получаем текущую позицию камеры
    local pos_x, pos_y, pos_z = getCameraPosition()
    
    -- Управление движением (WASD) - используем кэшированные векторы
    if isKeyPressed(KEY_W()) then
        pos_x = pos_x + front_x * movement_delta
        pos_y = pos_y + front_y * movement_delta
        pos_z = pos_z + front_z * movement_delta
        camera_moved = true
    end
    if isKeyPressed(KEY_S()) then
        pos_x = pos_x - front_x * movement_delta
        pos_y = pos_y - front_y * movement_delta
        pos_z = pos_z - front_z * movement_delta
        camera_moved = true
    end
    if isKeyPressed(KEY_A()) then
        pos_x = pos_x - right_x * movement_delta
        pos_y = pos_y - right_y * movement_delta
        pos_z = pos_z - right_z * movement_delta
        camera_moved = true
    end
    if isKeyPressed(KEY_D()) then
        pos_x = pos_x + right_x * movement_delta
        pos_y = pos_y + right_y * movement_delta
        pos_z = pos_z + right_z * movement_delta
        camera_moved = true
    end
    
    -- Управление поворотом мышью только при нажатой левой кнопке
    local camera_rotated = false
    if isMouseButtonPressed(MOUSE_LEFT()) then
        -- Получаем текущую позицию мыши относительно окна
        local mouse_x, mouse_y = getMousePosition()
        
        if first_mouse then
            last_mouse_x = mouse_x
            last_mouse_y = mouse_y
            first_mouse = false
        end
        
        -- Вычисляем дельту движения мыши
        local mouse_dx = mouse_x - last_mouse_x
        local mouse_dy = last_mouse_y - mouse_y  -- Инвертируем Y для правильного направления
        
        last_mouse_x = mouse_x
        last_mouse_y = mouse_y
        
        -- Обновляем углы поворота только если есть движение
        if mouse_dx ~= 0 or mouse_dy ~= 0 then
            yaw = yaw + mouse_dx * mouse_sensitivity * 100.0
            pitch = pitch + mouse_dy * mouse_sensitivity * 100.0
            
            -- Ограничиваем pitch чтобы не переворачиваться
            if pitch > 89.0 then
                pitch = 89.0
            elseif pitch < -89.0 then
                pitch = -89.0
            end
            
            -- Обновляем векторы направления
            updateCameraVectors()
            camera_rotated = true
        end
    else
        -- Сбрасываем флаг первого движения мыши когда кнопка не нажата
        first_mouse = true
    end
    
    -- Обновляем позицию и цель камеры одновременно, чтобы избежать дерганий
    if camera_moved or camera_rotated then
        setCameraPosition(pos_x, pos_y, pos_z)
        setCameraTarget(pos_x + front_x, pos_y + front_y, pos_z + front_z)
    end
end

function render()
    -- Дополнительная логика рендеринга (если нужна)
end

print("=== Camera Mouse Control Script Ready ===")