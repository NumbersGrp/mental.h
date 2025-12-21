-- Простой скрипт для тестирования Lua интеграции с прямоугольником

print("=== Simple Rectangle Script Loaded ===")

-- Переменные состояния
local time_offset = 0
local initialized = false

function init()
    print("Инициализация простого прямоугольника")
    
    -- Устанавливаем начальные параметры
    setPosition(0, 0, 0)
    setScale(1.0, 1.0, 1.0)
    setColor(1.0, 0.5, 0.2, 1.0)  -- Оранжевый цвет
    setVisible(true)
    
    time_offset = getTime()
    initialized = true
    
    print("Простой прямоугольник инициализирован")
end

function update(deltaTime)
    if not initialized then
        return
    end
    
    local current_time = getTime() - time_offset
    
    -- Простое движение по синусоиде
    local x = sin(current_time) * 0.3
    local y = cos(current_time * 0.7) * 0.2
    
    setPosition(x, y, 0)
    
    -- Изменение цвета
    local r = (sin(current_time * 2) + 1) * 0.5
    local g = (cos(current_time * 1.5) + 1) * 0.5
    local b = (sin(current_time * 0.8) + 1) * 0.5
    
    setColor(r, g, b, 1.0)
    
    -- Изменение размера
    local scale = 0.8 + sin(current_time * 3) * 0.2
    setScale(scale, scale, 1.0)
end

function render()
    -- Дополнительная логика рендеринга (если нужна)
end

print("=== Simple Rectangle Script Ready ===")