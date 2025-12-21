-- Тестовый скрипт корневого узла с горизонтальным движением треугольника
-- Простая демонстрация движения влево-вправо

print("=== Test Horizontal Triangle Script Loaded ===")

local initialized = false

function init()
    print("Инициализация тестовой сцены с горизонтальным движением")
    
    -- Создаем шейдеры
    print("Создание шейдеров...")
    local glow_shader_created = createShader("glow", "Common/Shaders/Glow/vertex_shader.glsl", "Common/Shaders/Glow/fragment_shader.glsl")
    
    if not glow_shader_created then
        print("ОШИБКА: Не удалось создать glow шейдер!")
        return
    end
    
    print("Шейдеры созданы успешно")
    
    -- Создаем один треугольник в центре
    local triangle_created = createObject("moving_triangle", "triangle")
    if triangle_created then
        print("Треугольник создан")
        attachShaderToObject("moving_triangle", "glow")
        -- НЕ подключаем индивидуальный скрипт - управляем из этого скрипта
        setObjectPosition("moving_triangle", 0.0, 0.0, 0.0)
        setObjectScale("moving_triangle", 2.0, 2.0, 2.0)  -- Большой треугольник
        setObjectColor("moving_triangle", 1.0, 0.5, 0.0, 1.0)  -- Оранжевый
        print("Треугольник настроен для горизонтального движения")
    else
        print("ОШИБКА: Не удалось создать треугольник!")
        return
    end
    
    initialized = true
    
    print("=== ТЕСТОВАЯ СЦЕНА ГОТОВА ===")
    print("Треугольник будет двигаться влево-вправо")
    print("Камера должна быть на позиции (0, 0, 8) для просмотра")
    print("=============================")
end

function update(deltaTime)
    if not initialized then
        return
    end
    
    local time = getTime()
    
    -- Горизонтальное движение треугольника
    local movement_speed = 2.0
    local movement_range = 4.0
    local offset_x = sin(time * movement_speed) * movement_range
    
    -- Устанавливаем новую позицию
    setObjectPosition("moving_triangle", offset_x, 0.0, 0.0)
    
    -- Небольшое вращение для визуального эффекта
    local rotation = time * 0.5
    setObjectRotation("moving_triangle", 0.0, 0.0, rotation)
    
    -- Вывод позиции каждые 2 секунды
    if math.floor(time) % 2 == 0 and math.floor(time * 10) % 10 == 0 then
        print(string.format("Triangle position: (%.2f, 0.0, 0.0)", offset_x))
    end
end

function render()
    -- Дополнительная логика рендеринга
end

print("=== Test Horizontal Triangle Script Ready ===")