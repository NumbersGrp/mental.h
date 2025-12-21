-- Скрипт управления камерой мышью и клавиатурой с плавным движением

print("=== Smooth Camera Mouse Control Script Loaded ===")

-- Переменные состояния
local initialized = false
local camera_speed = 3.0
local mouse_sensitivity = 0.0008
local yaw = -90.0  -- Начальный поворот (смотрим вперед)
local pitch = 0.0
local last_mouse_x = 0
local last_mouse_y = 0
local first_mouse = true

-- Переменные для плавного движения
local target_yaw = yaw
local target_pitch = pitch
local rotation_smoothing = 8.0  -- Скорость сглаживания поворота
local movement_smoothing = 10.0  -- Скорость сглаживания движения

-- Векторы направления камеры (кэшируем для избежания пересчетов)
local front_x, front_y, front_z = 0.0, 0.0, -1.0
local right_x, right_y, right_z = 1.0, 0.0, 0.0
local up_x, up_y, up_z = 0.0, 1.0, 0.0

-- Целевая и текущая позиция камеры для плавного движения
local current_pos_x, current_pos_y, current_pos_z = 0.0, 0.0, 8.0  -- Дальше для просмотра всех объектов
local target_pos_x, target_pos_y, target_pos_z = 0.0, 0.0, 8.0

-- Функция линейной интерполяции
local function lerp(a, b, t)
    return a + (b - a) * t
end

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
    print("Инициализация плавного управления камерой мышью")
    
    -- Устанавливаем начальную позицию камеры
    setCameraPosition(current_pos_x, current_pos_y, current_pos_z)
    setCameraUp(0, 1, 0)
    
    -- Настраиваем перспективу с более широким полем зрения
    setPerspective(60.0, 16.0/9.0, 0.1, 100.0)  -- Увеличиваем FOV до 60 градусов
    
    -- Инициализируем векторы направления
    updateCameraVectors()
    
    -- Устанавливаем начальную цель камеры
    setCameraTarget(current_pos_x + front_x, current_pos_y + front_y, current_pos_z + front_z)
    
    initialized = true
    
    print("Плавная камера готова к управлению")
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
    
    -- Управление движением (WASD) - обновляем целевую позицию
    if isKeyPressed(KEY_W()) then
        target_pos_x = target_pos_x + front_x * movement_delta
        target_pos_y = target_pos_y + front_y * movement_delta
        target_pos_z = target_pos_z + front_z * movement_delta
    end
    if isKeyPressed(KEY_S()) then
        target_pos_x = target_pos_x - front_x * movement_delta
        target_pos_y = target_pos_y - front_y * movement_delta
        target_pos_z = target_pos_z - front_z * movement_delta
    end
    if isKeyPressed(KEY_A()) then
        target_pos_x = target_pos_x - right_x * movement_delta
        target_pos_y = target_pos_y - right_y * movement_delta
        target_pos_z = target_pos_z - right_z * movement_delta
    end
    if isKeyPressed(KEY_D()) then
        target_pos_x = target_pos_x + right_x * movement_delta
        target_pos_y = target_pos_y + right_y * movement_delta
        target_pos_z = target_pos_z + right_z * movement_delta
    end
    
    -- Управление поворотом мышью только при нажатой левой кнопке
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
        
        -- Обновляем целевые углы поворота
        if mouse_dx ~= 0 or mouse_dy ~= 0 then
            target_yaw = target_yaw + mouse_dx * mouse_sensitivity * 100.0
            target_pitch = target_pitch + mouse_dy * mouse_sensitivity * 100.0
            
            -- Ограничиваем target_pitch чтобы не переворачиваться
            if target_pitch > 89.0 then
                target_pitch = 89.0
            elseif target_pitch < -89.0 then
                target_pitch = -89.0
            end
        end
    else
        -- Сбрасываем флаг первого движения мыши когда кнопка не нажата
        first_mouse = true
    end
    
    -- Плавно интерполируем позицию камеры
    local movement_lerp_factor = movement_smoothing * deltaTime
    if movement_lerp_factor > 1.0 then movement_lerp_factor = 1.0 end
    
    current_pos_x = lerp(current_pos_x, target_pos_x, movement_lerp_factor)
    current_pos_y = lerp(current_pos_y, target_pos_y, movement_lerp_factor)
    current_pos_z = lerp(current_pos_z, target_pos_z, movement_lerp_factor)
    
    -- Плавно интерполируем поворот камеры
    local rotation_lerp_factor = rotation_smoothing * deltaTime
    if rotation_lerp_factor > 1.0 then rotation_lerp_factor = 1.0 end
    
    yaw = lerp(yaw, target_yaw, rotation_lerp_factor)
    pitch = lerp(pitch, target_pitch, rotation_lerp_factor)
    
    -- Обновляем векторы направления
    updateCameraVectors()
    
    -- Обновляем позицию и цель камеры
    setCameraPosition(current_pos_x, current_pos_y, current_pos_z)
    setCameraTarget(current_pos_x + front_x, current_pos_y + front_y, current_pos_z + front_z)
end

function render()
    -- Дополнительная логика рендеринга (если нужна)
end

print("=== Smooth Camera Mouse Control Script Ready ===")