-- Простой тестовый скрипт для проверки Lua интеграции

print("=== Lua Test Script Loaded ===")

function init()
    print("Инициализация тестового объекта")
    setPosition(0, 0, 0)
    setColor(0.8, 0.2, 0.2, 1.0)  -- Красный цвет
    setScale(1.2, 1.2, 1.0)       -- Немного увеличен
    print("Тестовый объект инициализирован")
end

function update(deltaTime)
    -- Простое движение вверх-вниз
    local time = getTime()
    local y = sin(time * 2) * 0.3
    setPosition(0, y, 0)
    
    -- Изменение цвета
    local intensity = (cos(time * 3) + 1) * 0.5
    setColor(intensity, 0.2, 0.2, 1.0)
end

function render()
    -- Дополнительная логика рендеринга
end

print("=== Lua Test Script Functions Defined ===")