-- Скрипт треугольника с горизонтальным движением влево-вправо

print("=== Triangle Horizontal Movement Script Loaded ===")

local initialized = false
local movement_speed = 2.0  -- Скорость движения
local movement_range = 3.0  -- Диапазон движения (от -range до +range)
local base_x = 0.0          -- Базовая позиция X
local base_y = 0.0          -- Базовая позиция Y
local base_z = 0.0          -- Базовая позиция Z

function init()
    print("Инициализация треугольника с горизонтальным движением")
    
    -- Получаем текущую позицию как базовую
    local current_x, current_y, current_z = getPosition()
    base_x = current_x
    base_y = current_y
    base_z = current_z
    
    print(string.format("Базовая позиция: (%.1f, %.1f, %.1f)", base_x, base_y, base_z))
    print(string.format("Диапазон движения: %.1f единиц влево-вправо", movement_range))
    
    initialized = true
    print("Треугольник с горизонтальным движением инициализирован")
end

function update(deltaTime)
    if not initialized then
        return
    end
    
    local time = getTime()
    
    -- Вычисляем новую позицию X используя синус для плавного движения
    local offset_x = sin(time * movement_speed) * movement_range
    local new_x = base_x + offset_x
    
    -- Устанавливаем новую позицию (Y и Z остаются неизменными)
    setPosition(new_x, base_y, base_z)
    
    -- Опционально: небольшое вращение для визуального эффекта
    local rotation = time * 0.5
    setRotation(0.0, 0.0, rotation)
end

function render()
    -- Дополнительная логика рендеринга
end

print("=== Triangle Horizontal Movement Script Ready ===")