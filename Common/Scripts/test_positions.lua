-- Тестовый скрипт для проверки позиций всех объектов
-- Размещает объекты в четких, видимых позициях

print("=== Test Positions Script Loaded ===")

local initialized = false

function init()
    print("Инициализация тестовых позиций")
    
    -- Создаем шейдеры
    print("Создание шейдеров...")
    local rainbow_shader_created = createShader("rainbow", "Common/Shaders/Rainbow/vertex_shader.glsl", "Common/Shaders/Rainbow/fragment_shader.glsl")
    local glow_shader_created = createShader("glow", "Common/Shaders/Glow/vertex_shader.glsl", "Common/Shaders/Glow/fragment_shader.glsl")
    
    if not rainbow_shader_created then
        print("ОШИБКА: Не удалось создать rainbow шейдер!")
        return
    end
    
    if not glow_shader_created then
        print("ОШИБКА: Не удалось создать glow шейдер!")
        return
    end
    
    print("Шейдеры созданы успешно")
    
    -- Создаем прямоугольник в центре
    local rect_created = createObject("main_rectangle", "rectangle")
    if rect_created then
        print("Прямоугольник создан")
        attachShaderToObject("main_rectangle", "rainbow")
        attachScriptToObject("main_rectangle", "Common/Scripts/simple_rectangle.lua")
        setObjectPosition("main_rectangle", 0.0, 0.0, 0.0)
        setObjectScale("main_rectangle", 1.0, 1.0, 1.0)
        setObjectColor("main_rectangle", 1.0, 1.0, 1.0, 1.0)
        print("Прямоугольник: центр (0, 0, 0)")
    end
    
    -- Создаем треугольники в четырех углах
    local triangle1_created = createObject("triangle_1", "triangle")
    if triangle1_created then
        print("Треугольник 1 создан")
        attachShaderToObject("triangle_1", "glow")
        setObjectPosition("triangle_1", 4.0, 3.0, 0.0)  -- правый верх
        setObjectScale("triangle_1", 1.5, 1.5, 1.5)
        setObjectColor("triangle_1", 1.0, 0.0, 0.0, 1.0)  -- красный
        print("Треугольник 1: правый верх (4, 3, 0)")
    end
    
    local triangle2_created = createObject("triangle_2", "triangle")
    if triangle2_created then
        print("Треугольник 2 создан")
        attachShaderToObject("triangle_2", "glow")
        setObjectPosition("triangle_2", -4.0, 3.0, 0.0)  -- левый верх
        setObjectScale("triangle_2", 1.5, 1.5, 1.5)
        setObjectColor("triangle_2", 0.0, 1.0, 0.0, 1.0)  -- зеленый
        print("Треугольник 2: левый верх (-4, 3, 0)")
    end
    
    local triangle3_created = createObject("triangle_3", "triangle")
    if triangle3_created then
        print("Треугольник 3 создан")
        attachShaderToObject("triangle_3", "glow")
        setObjectPosition("triangle_3", 4.0, -3.0, 0.0)  -- правый низ
        setObjectScale("triangle_3", 1.5, 1.5, 1.5)
        setObjectColor("triangle_3", 0.0, 0.0, 1.0, 1.0)  -- синий
        print("Треугольник 3: правый низ (4, -3, 0)")
    end
    
    local triangle4_created = createObject("triangle_4", "triangle")
    if triangle4_created then
        print("Треугольник 4 создан")
        attachShaderToObject("triangle_4", "glow")
        setObjectPosition("triangle_4", -4.0, -3.0, 0.0)  -- левый низ
        setObjectScale("triangle_4", 1.5, 1.5, 1.5)
        setObjectColor("triangle_4", 1.0, 1.0, 0.0, 1.0)  -- желтый
        print("Треугольник 4: левый низ (-4, -3, 0)")
    end
    
    -- Выводим список созданных объектов
    print("Список созданных объектов:")
    listObjects()
    
    initialized = true
    
    print("=== ТЕСТОВЫЕ ПОЗИЦИИ УСТАНОВЛЕНЫ ===")
    print("Прямоугольник: центр (0, 0) - белый")
    print("Треугольник 1: правый верх (4, 3) - красный")
    print("Треугольник 2: левый верх (-4, 3) - зеленый")
    print("Треугольник 3: правый низ (4, -3) - синий")
    print("Треугольник 4: левый низ (-4, -3) - желтый")
    print("Камера должна быть на позиции (0, 0, 10) для просмотра всех объектов")
    print("=====================================")
end

function update(deltaTime)
    if not initialized then
        return
    end
    
    local time = getTime()
    
    -- Простая анимация - вращение треугольников
    setObjectRotation("triangle_1", 0.0, 0.0, time * 0.5)
    setObjectRotation("triangle_2", 0.0, 0.0, -time * 0.5)
    setObjectRotation("triangle_3", 0.0, 0.0, time * 0.7)
    setObjectRotation("triangle_4", 0.0, 0.0, -time * 0.7)
    
    -- Вывод позиций каждые 5 секунд
    if math.floor(time) % 5 == 0 and math.floor(time * 10) % 10 == 0 then
        print("=== ТЕКУЩИЕ ПОЗИЦИИ ===")
        local rect_x, rect_y, rect_z = getObjectPosition("main_rectangle")
        print(string.format("Rectangle: (%.1f, %.1f, %.1f)", rect_x, rect_y, rect_z))
        
        local tri1_x, tri1_y, tri1_z = getObjectPosition("triangle_1")
        print(string.format("Triangle 1: (%.1f, %.1f, %.1f)", tri1_x, tri1_y, tri1_z))
        
        local tri2_x, tri2_y, tri2_z = getObjectPosition("triangle_2")
        print(string.format("Triangle 2: (%.1f, %.1f, %.1f)", tri2_x, tri2_y, tri2_z))
        
        local tri3_x, tri3_y, tri3_z = getObjectPosition("triangle_3")
        print(string.format("Triangle 3: (%.1f, %.1f, %.1f)", tri3_x, tri3_y, tri3_z))
        
        local tri4_x, tri4_y, tri4_z = getObjectPosition("triangle_4")
        print(string.format("Triangle 4: (%.1f, %.1f, %.1f)", tri4_x, tri4_y, tri4_z))
        print("=======================")
    end
end

function render()
    -- Дополнительная логика рендеринга
end

print("=== Test Positions Script Ready ===")