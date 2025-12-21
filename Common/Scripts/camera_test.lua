-- Тестовый скрипт для проверки всех функций камеры

print("=== Camera Test Script Loaded ===")

local initialized = false
local test_phase = 0
local phase_time = 0
local phase_duration = 3.0  -- 3 секунды на каждую фазу

function init()
    print("Инициализация тестирования камеры")
    
    -- Устанавливаем начальные параметры
    setCameraPosition(0, 0, 5)
    setCameraTarget(0, 0, 0)
    setCameraUp(0, 1, 0)
    setPerspective(45.0, 16.0/9.0, 0.1, 100.0)
    
    initialized = true
    test_phase = 0
    phase_time = 0
    
    print("Камера инициализирована для тестирования")
    print("Начальная позиция:", getCameraPosition())
    print("Начальная цель:", getCameraTarget())
    print("Начальный up вектор:", getCameraUp())
end

function update(deltaTime)
    if not initialized then
        return
    end
    
    phase_time = phase_time + deltaTime
    
    -- Переключение между фазами тестирования
    if phase_time >= phase_duration then
        test_phase = test_phase + 1
        phase_time = 0
        
        if test_phase > 6 then
            test_phase = 0  -- Зацикливаем тест
        end
        
        print("=== Фаза тестирования:", test_phase, "===")
    end
    
    local t = phase_time / phase_duration  -- Нормализованное время фазы (0-1)
    
    if test_phase == 0 then
        -- Тест движения вперед/назад
        print("Тестируем moveForward/moveBackward")
        if t < 0.5 then
            moveForward(deltaTime * 2.0)
        else
            moveBackward(deltaTime * 2.0)
        end
        
    elseif test_phase == 1 then
        -- Тест движения влево/вправо
        print("Тестируем moveLeft/moveRight")
        if t < 0.5 then
            moveLeft(deltaTime * 2.0)
        else
            moveRight(deltaTime * 2.0)
        end
        
    elseif test_phase == 2 then
        -- Тест движения вверх/вниз
        print("Тестируем moveUp/moveDown")
        if t < 0.5 then
            moveUp(deltaTime * 2.0)
        else
            moveDown(deltaTime * 2.0)
        end
        
    elseif test_phase == 3 then
        -- Тест поворота Yaw (влево-вправо)
        print("Тестируем rotateYaw")
        rotateYaw(deltaTime * 1.0)
        
    elseif test_phase == 4 then
        -- Тест поворота Pitch (вверх-вниз)
        print("Тестируем rotatePitch")
        rotatePitch(deltaTime * 0.5)
        
    elseif test_phase == 5 then
        -- Тест поворота Roll (наклон)
        print("Тестируем rotateRoll")
        rotateRoll(deltaTime * 0.8)
        
    elseif test_phase == 6 then
        -- Тест изменения FOV
        print("Тестируем setFOV")
        local fov = 30.0 + sin(getTime() * 2.0) * 20.0
        setFOV(fov)
        
        -- Также тестируем lookAt
        local radius = 3.0
        local angle = getTime() * 0.5
        local x = cos(angle) * radius
        local z = sin(angle) * radius
        lookAt(x, 2.0, z, 0, 0, 0, 0, 1, 0)
    end
    
    -- Выводим текущие параметры камеры каждые 0.5 секунды
    if math.floor(phase_time * 2) ~= math.floor((phase_time - deltaTime) * 2) then
        local pos_x, pos_y, pos_z = getCameraPosition()
        local target_x, target_y, target_z = getCameraTarget()
        print(string.format("Позиция: %.2f, %.2f, %.2f", pos_x, pos_y, pos_z))
        print(string.format("Цель: %.2f, %.2f, %.2f", target_x, target_y, target_z))
    end
end

function render()
    -- Дополнительная логика рендеринга (если нужна)
end

print("=== Camera Test Script Ready ===")