-- Скрипт для светящегося треугольника (позиция снизу слева)

print("=== Glowing Triangle Bottom Script Loaded ===")

local initialized = false
local rotation_speed = 1.8
local scale_pulse_speed = 2.5
local color_change_speed = 1.2

function init()
    print("Инициализация светящегося треугольника (снизу слева)")
    
    -- Устанавливаем начальные параметры (дальше снизу слева от прямоугольника)
    setPosition(-2.0, -1.5, 0)  -- Дальше влево и ниже от прямоугольника
    setRotation(0, 0, 0)
    setScale(0.8, 0.8, 1.0)
    setColor(1.0, 1.0, 1.0, 1.0)
    setVisible(true)
    
    initialized = true
    print("Светящийся треугольник инициализирован")
    print("Позиция треугольника: -2.0, -1.5, 0")
end

function update(deltaTime)
    if not initialized then
        return
    end
    
    local time = getTime()
    
    -- Вращение треугольника (немного быстрее)
    local rotation_z = time * rotation_speed * 57.2958 -- Конвертируем в градусы
    setRotation(0, 0, rotation_z)
    
    -- Пульсирующий размер (более интенсивный)
    local scale_factor = 0.8 + 0.4 * sin(time * scale_pulse_speed)
    setScale(scale_factor, scale_factor, 1.0)
    
    -- Изменение цвета для усиления светящегося эффекта (более динамичное)
    local red = 0.9 + 0.1 * sin(time * color_change_speed)
    local green = 0.4 + 0.6 * sin(time * color_change_speed * 1.5)
    local blue = 0.2 + 0.8 * sin(time * color_change_speed * 0.8)
    
    setColor(red, green, blue, 1.0)
end

function render()
    -- Дополнительная логика рендеринга (если нужна)
end

print("=== Glowing Triangle Bottom Script Ready ===")