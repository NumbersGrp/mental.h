-- Скрипт для одиночного светящегося треугольника

print("=== Single Glowing Triangle Script Loaded ===")

local initialized = false
local rotation_speed = 1.5
local scale_pulse_speed = 2.0
local color_change_speed = 0.8

function init()
    print("Инициализация одиночного светящегося треугольника")
    
    -- Устанавливаем начальные параметры (дальше от прямоугольника)
    setPosition(2.0, 1.0, 0)  -- Дальше вправо и выше от прямоугольника
    setRotation(0, 0, 0)
    setScale(1.0, 1.0, 1.0)
    setColor(1.0, 1.0, 1.0, 1.0)
    setVisible(true)
    
    initialized = true
    print("Одиночный светящийся треугольник инициализирован")
    print("Позиция треугольника: 2.0, 1.0, 0")
end

function update(deltaTime)
    if not initialized then
        return
    end
    
    local time = getTime()
    
    -- Вращение треугольника
    local rotation_z = time * rotation_speed * 57.2958 -- Конвертируем в градусы
    setRotation(0, 0, rotation_z)
    
    -- Пульсирующий размер
    local scale_factor = 1.0 + 0.3 * sin(time * scale_pulse_speed)
    setScale(scale_factor, scale_factor, 1.0)
    
    -- Изменение цвета для усиления светящегося эффекта
    local red = 0.8 + 0.2 * sin(time * color_change_speed)
    local green = 0.6 + 0.4 * sin(time * color_change_speed * 1.3)
    local blue = 0.4 + 0.6 * sin(time * color_change_speed * 0.7)
    
    setColor(red, green, blue, 1.0)
end

function render()
    -- Дополнительная логика рендеринга (если нужна)
end

print("=== Single Glowing Triangle Script Ready ===")