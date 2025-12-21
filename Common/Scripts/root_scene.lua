-- Скрипт корневого узла сцены
-- Здесь создаются все объекты, шейдеры и настраивается сцена
-- 🔥 HOT RELOAD TEST: Файл изменен в реальном времени!
-- Время изменения: 15:25 - ДОБАВЛЕНЫ СКРИПТЫ ГОРИЗОНТАЛЬНОГО ДВИЖЕНИЯ!

print("=== Root Scene Script Loaded ===")
print("🔥 HOT RELOAD TEST: Скрипт перезагружен в 15:25 - ТРЕУГОЛЬНИКИ ДВИГАЮТСЯ!")

local initialized = false

function init()
    print("Инициализация корневой сцены")
    
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
    
    -- Создаем объекты
    print("Создание объектов...")
    
    -- Создаем прямоугольник
    local rect_created = createObject("main_rectangle", "rectangle")
    if rect_created then
        print("Прямоугольник создан")
        -- Подключаем шейдер к прямоугольнику
        if attachShaderToObject("main_rectangle", "rainbow") then
            print("Rainbow шейдер подключен к прямоугольнику")
        end
        -- Подключаем скрипт к прямоугольнику
        if attachScriptToObject("main_rectangle", "Common/Scripts/simple_rectangle.lua") then
            print("Скрипт подключен к прямоугольнику")
        end
    else
        print("ОШИБКА: Не удалось создать прямоугольник!")
    end
    
    -- Создаем первый треугольник
    local triangle1_created = createObject("triangle_1", "triangle")
    if triangle1_created then
        print("Треугольник 1 создан")
        -- Подключаем glow шейдер
        if attachShaderToObject("triangle_1", "glow") then
            print("Glow шейдер подключен к треугольнику 1")
        end
        -- Подключаем скрипт горизонтального движения
        if attachScriptToObject("triangle_1", "Common/Scripts/triangle_horizontal_movement.lua") then
            print("Скрипт горизонтального движения подключен к треугольнику 1")
        end
    else
        print("ОШИБКА: Не удалось создать треугольник 1!")
    end
    
    -- Создаем второй треугольник
    local triangle2_created = createObject("triangle_2", "triangle")
    if triangle2_created then
        print("Треугольник 2 создан")
        -- Подключаем glow шейдер
        if attachShaderToObject("triangle_2", "glow") then
            print("Glow шейдер подключен к треугольнику 2")
        end
        -- Подключаем скрипт быстрого горизонтального движения
        if attachScriptToObject("triangle_2", "Common/Scripts/triangle_fast_horizontal.lua") then
            print("Скрипт быстрого горизонтального движения подключен к треугольнику 2")
        end
    else
        print("ОШИБКА: Не удалось создать треугольник 2!")
    end
    
    -- Создаем третий треугольник
    local triangle3_created = createObject("triangle_3", "triangle")
    if triangle3_created then
        print("Треугольник 3 создан")
        -- Подключаем glow шейдер
        if attachShaderToObject("triangle_3", "glow") then
            print("Glow шейдер подключен к треугольнику 3")
        end
        -- Подключаем скрипт линейного движения
        if attachScriptToObject("triangle_3", "Common/Scripts/triangle_linear_movement.lua") then
            print("Скрипт линейного движения подключен к треугольнику 3")
        end
    else
        print("ОШИБКА: Не удалось создать треугольник 3!")
    end
    
    -- Выводим список созданных объектов
    print("Список созданных объектов:")
    listObjects()
    
    -- Создаем четвертый треугольник для тестирования hot reload
    local triangle4_created = createObject("triangle_4", "triangle")
    if triangle4_created then
        print("🔥 HOT RELOAD TEST: Треугольник 4 создан!")
        -- Подключаем glow шейдер
        if attachShaderToObject("triangle_4", "glow") then
            print("🔥 HOT RELOAD TEST: Glow шейдер подключен к треугольнику 4")
        end
        -- Подключаем скрипт горизонтального движения
        if attachScriptToObject("triangle_4", "Common/Scripts/triangle_horizontal_movement.lua") then
            print("🔥 HOT RELOAD TEST: Скрипт горизонтального движения подключен к треугольнику 4")
        end
    else
        print("❌ ОШИБКА: Не удалось создать треугольник 4!")
    end
    
    -- ВАЖНО: Ждем немного, чтобы индивидуальные скрипты объектов завершили инициализацию
    -- Затем переопределяем позиции, масштабы и цвета
    
    -- Демонстрация новых функций управления объектами
    print("\n=== ДЕМОНСТРАЦИЯ УПРАВЛЕНИЯ ОБЪЕКТАМИ ===")
    
    -- Устанавливаем начальные позиции объектов (более разнесенные)
    print("Переопределяем позиции объектов...")
    setObjectPosition("main_rectangle", 0.0, 0.0, 0.0)      -- центр
    setObjectPosition("triangle_1", 3.0, 2.0, 0.0)          -- правый верх
    setObjectPosition("triangle_2", -3.0, -2.0, 0.0)        -- левый низ
    setObjectPosition("triangle_3", 3.0, -2.0, 0.0)         -- правый низ
    if getObjectByName("triangle_4") then
        setObjectPosition("triangle_4", -3.0, 2.0, 0.0)     -- левый верх
    end
    
    -- Устанавливаем начальные масштабы (более заметные)
    print("Устанавливаем масштабы...")
    setObjectScale("main_rectangle", 1.0, 1.0, 1.0)
    setObjectScale("triangle_1", 1.5, 1.5, 1.5)             -- больше
    setObjectScale("triangle_2", 1.3, 1.3, 1.3)             -- больше
    setObjectScale("triangle_3", 1.4, 1.4, 1.4)             -- больше
    if getObjectByName("triangle_4") then
        setObjectScale("triangle_4", 1.2, 1.2, 1.2)         -- больше
    end
    
    -- Устанавливаем начальные цвета (более контрастные)
    print("Устанавливаем цвета...")
    setObjectColor("main_rectangle", 1.0, 1.0, 1.0, 1.0)    -- белый
    setObjectColor("triangle_1", 1.0, 0.0, 0.0, 1.0)        -- красный
    setObjectColor("triangle_2", 0.0, 1.0, 0.0, 1.0)        -- зеленый
    setObjectColor("triangle_3", 0.0, 0.0, 1.0, 1.0)        -- синий
    if getObjectByName("triangle_4") then
        setObjectColor("triangle_4", 1.0, 1.0, 0.0, 1.0)    -- желтый
    end
    
    -- Проверяем и выводим информацию об объектах
    print("Информация об объектах:")
    local rect_x, rect_y, rect_z = getObjectPosition("main_rectangle")
    print(string.format("  Rectangle: pos(%.1f, %.1f, %.1f)", rect_x, rect_y, rect_z))
    
    local tri1_x, tri1_y, tri1_z = getObjectPosition("triangle_1")
    print(string.format("  Triangle 1: pos(%.1f, %.1f, %.1f)", tri1_x, tri1_y, tri1_z))
    
    local tri2_x, tri2_y, tri2_z = getObjectPosition("triangle_2")
    print(string.format("  Triangle 2: pos(%.1f, %.1f, %.1f)", tri2_x, tri2_y, tri2_z))
    
    local tri3_x, tri3_y, tri3_z = getObjectPosition("triangle_3")
    print(string.format("  Triangle 3: pos(%.1f, %.1f, %.1f)", tri3_x, tri3_y, tri3_z))
    
    if getObjectByName("triangle_4") then
        local tri4_x, tri4_y, tri4_z = getObjectPosition("triangle_4")
        print(string.format("  Triangle 4: pos(%.1f, %.1f, %.1f)", tri4_x, tri4_y, tri4_z))
    end
    
    -- Проверяем видимость всех объектов
    print("Видимость объектов:")
    print(string.format("  Rectangle: %s", isObjectVisible("main_rectangle") and "видимый" or "скрытый"))
    print(string.format("  Triangle 1: %s", isObjectVisible("triangle_1") and "видимый" or "скрытый"))
    print(string.format("  Triangle 2: %s", isObjectVisible("triangle_2") and "видимый" or "скрытый"))
    print(string.format("  Triangle 3: %s", isObjectVisible("triangle_3") and "видимый" or "скрытый"))
    if getObjectByName("triangle_4") then
        print(string.format("  Triangle 4: %s", isObjectVisible("triangle_4") and "видимый" or "скрытый"))
    end
    
    print("==========================================")
    
    initialized = true
    
    print("Корневая сцена инициализирована успешно!")
    print("=== HOT RELOAD АКТИВЕН ===")
    print("🔥 HOT RELOAD TEST: Файл был изменен!")
    print("Вы можете редактировать этот файл (root_scene.lua) и изменения")
    print("будут применены автоматически без перезапуска приложения!")
    print("\nУправление:")
    print("  WASD - движение камеры")
    print("  Левая кнопка мыши + движение - поворот камеры")
    print("  Стрелочки - управление треугольником 1")
    print("  Клавиши 1-4 - переключение видимости объектов")
    print("  Редактируйте этот скрипт для изменения сцены!")
    print("\nАнимации объектов:")
    print("  • Прямоугольник - движется по синусоиде (управляется root_scene)")
    print("  • Треугольник 1 - плавное горизонтальное движение влево-вправо")
    print("  • Треугольник 2 - быстрое горизонтальное движение влево-вправо")
    print("  • Треугольник 3 - линейное движение туда-сюда с покачиванием")
    print("  • Треугольник 4 - плавное горизонтальное движение влево-вправо")
    print("\nДоступные функции управления объектами:")
    print("  setObjectPosition(name, x, y, z)")
    print("  getObjectPosition(name) -> x, y, z")
    print("  setObjectRotation(name, x, y, z)")
    print("  getObjectRotation(name) -> x, y, z")
    print("  setObjectScale(name, x, y, z)")
    print("  getObjectScale(name) -> x, y, z")
    print("  setObjectColor(name, r, g, b, a)")
    print("  getObjectColor(name) -> r, g, b, a")
    print("  setObjectVisible(name, visible)")
    print("  isObjectVisible(name) -> boolean")
    print("==========================")
end

function update(deltaTime)
    if not initialized then
        return
    end
    
    -- Демонстрация управления объектами через новые API функции
    local time = getTime()
    
    -- Анимация позиции прямоугольника (остается под управлением root_scene)
    local rect_x = sin(time * 0.5) * 0.5
    local rect_y = cos(time * 0.3) * 0.3
    setObjectPosition("main_rectangle", rect_x, rect_y, 0.0)
    
    -- Треугольники теперь управляются своими собственными скриптами движения
    -- triangle_1 - обычное горизонтальное движение
    -- triangle_2 - быстрое горизонтальное движение  
    -- triangle_3 - линейное движение туда-сюда
    -- triangle_4 - обычное горизонтальное движение
    
    -- Интерактивное управление видимостью объектов клавишами 1-4
    if isKeyPressed(49) then -- клавиша '1'
        local visible = isObjectVisible("main_rectangle")
        setObjectVisible("main_rectangle", not visible)
    end
    if isKeyPressed(50) then -- клавиша '2'
        local visible = isObjectVisible("triangle_1")
        setObjectVisible("triangle_1", not visible)
    end
    if isKeyPressed(51) then -- клавиша '3'
        local visible = isObjectVisible("triangle_2")
        setObjectVisible("triangle_2", not visible)
    end
    if isKeyPressed(52) then -- клавиша '4'
        local visible = isObjectVisible("triangle_3")
        setObjectVisible("triangle_3", not visible)
    end
    
    -- Вывод информации о позициях всех объектов (каждые 5 секунд)
    if math.floor(time) % 5 == 0 and math.floor(time * 10) % 10 == 0 then
        print("=== ПОЗИЦИИ ОБЪЕКТОВ ===")
        local rect_x, rect_y, rect_z = getObjectPosition("main_rectangle")
        print(string.format("Rectangle: (%.2f, %.2f, %.2f)", rect_x, rect_y, rect_z))
        
        local tri1_x, tri1_y, tri1_z = getObjectPosition("triangle_1")
        print(string.format("Triangle 1 (horizontal): (%.2f, %.2f, %.2f)", tri1_x, tri1_y, tri1_z))
        
        local tri2_x, tri2_y, tri2_z = getObjectPosition("triangle_2")
        print(string.format("Triangle 2 (fast horizontal): (%.2f, %.2f, %.2f)", tri2_x, tri2_y, tri2_z))
        
        local tri3_x, tri3_y, tri3_z = getObjectPosition("triangle_3")
        print(string.format("Triangle 3 (linear): (%.2f, %.2f, %.2f)", tri3_x, tri3_y, tri3_z))
        
        if getObjectByName("triangle_4") then
            local tri4_x, tri4_y, tri4_z = getObjectPosition("triangle_4")
            print(string.format("Triangle 4 (horizontal): (%.2f, %.2f, %.2f)", tri4_x, tri4_y, tri4_z))
        end
        print("========================")
    end
end

function render()
    if not initialized then
        return
    end
    
    -- Здесь можно добавить глобальную логику рендеринга
    -- Например, пост-обработку, глобальные эффекты и т.д.
end

print("=== Root Scene Script Ready ===")