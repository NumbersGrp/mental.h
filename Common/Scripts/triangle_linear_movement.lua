-- Скрипт треугольника с линейным движением влево-вправо (туда-сюда)

print("=== Triangle Linear Movement Script Loaded ===")

local initialized = false
local movement_speed = 2.0  -- Скорость движения
local movement_range = 3.0  -- Диапазон движения
local base_x = 0.0          -- Базовая позиция X
local base_y = 0.0          -- Базовая позиция Y
local base_z = 0.0          -- Базовая позиция Z
local direction = 1         -- Направление движения (1 = вправо, -1 = влево)
local current_offset = 0.0  -- Текущее смещение от базовой позиции

function init()
    print("Инициализация треугольника с линейным движением")
    
    -- Получаем текущую позицию как базовую
    local current_x, current_y, current_z = getPosition()
    base_x = current_x
    base_y = current_y
    base_z = current_z
    
    print(string.format("Базовая позиция: (%.1f, %.1f, %.1f)", base_x, base_y, base_z))
    print(string.format("Диапазон движения: %.1f единиц влево-вправо (линейно)", movement_range))
    
    initialized = true
    print("Треугольник с линейным движением инициализирован")
end

function update(deltaTime)
    if not initialized then
        return
    end
    
    -- Обновляем позицию
    current_offset = current_offset + (direction * movement_speed * deltaTime)
    
    -- Проверяем границы и меняем направление
    if current_offset >= movement_range then
        current_offset = movement_range
        direction = -1  -- Начинаем двигаться влево
    elseif current_offset <= -movement_range then
        current_offset = -movement_range
        direction = 1   -- Начинаем двигаться вправо
    end
    
    -- Устанавливаем новую позицию
    local new_x = base_x + current_offset
    setPosition(new_x, base_y, base_z)
    
    -- Небольшое покачивание для визуального эффекта
    local time = getTime()
    local wobble = sin(time * 8.0) * 0.1
    setPosition(new_x, base_y + wobble, base_z)
end

function render()
    -- Дополнительная логика рендеринга
end

print("=== Triangle Linear Movement Script Ready ===")