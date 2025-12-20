--[[
    Movement Script для Mental Engine
    
    Демонстрирует синусоидальное движение объекта с изменением цвета и вращением.
    Этот скрипт показывает основные возможности Lua API для управления объектами.
--]]

-- ============================================================================
-- Настройки анимации
-- ============================================================================

local config = {
    amplitude = 0.5,        -- Амплитуда движения
    frequency = 2.0,        -- Частота движения
    colorSpeed = 1.0,       -- Скорость изменения цвета
    rotationSpeed = 45.0    -- Скорость вращения (градусы в секунду)
}

-- ============================================================================
-- Переменные состояния
-- ============================================================================

local state = {
    startTime = 0,
    initialized = false
}

-- ============================================================================
-- Основные функции скрипта
-- ============================================================================

function init()
    print("=== Movement Script Initialized ===")
    
    state.startTime = getTime()
    state.initialized = true
    
    -- Устанавливаем начальные параметры объекта
    setPosition(0, 0, 0)
    setScale(1, 1, 1)
    setSpeed(1.0)
    setVisible(true)
    
    print("Object initialized with movement script")
    print("Animation settings:")
    print("  Amplitude: " .. config.amplitude)
    print("  Frequency: " .. config.frequency)
    print("  Color Speed: " .. config.colorSpeed)
end

function update(deltaTime)
    if not state.initialized then
        return
    end
    
    local currentTime = getTime() - state.startTime
    
    -- Синусоидальное движение по X и Y
    local x = config.amplitude * sin(currentTime * config.frequency)
    local y = config.amplitude * cos(currentTime * config.frequency * 0.5)
    
    setPosition(x, y, 0)
    
    -- Плавное изменение цвета со временем
    local r = (sin(currentTime * config.colorSpeed) + 1) * 0.5
    local g = (cos(currentTime * config.colorSpeed * 1.3) + 1) * 0.5
    local b = (sin(currentTime * config.colorSpeed * 0.7) + 1) * 0.5
    
    setColor(r, g, b, 1.0)
    
    -- Вращение объекта
    local rotation = currentTime * config.rotationSpeed
    setRotation(0, 0, rotation)
end

function render()
    -- Дополнительная логика рендеринга (если необходимо)
end

-- ============================================================================
-- Пользовательские функции для настройки
-- ============================================================================

function setMovementAmplitude(newAmplitude)
    if newAmplitude and newAmplitude > 0 then
        config.amplitude = newAmplitude
        print("Movement amplitude set to: " .. newAmplitude)
    end
end

function setMovementFrequency(newFrequency)
    if newFrequency and newFrequency > 0 then
        config.frequency = newFrequency
        print("Movement frequency set to: " .. newFrequency)
    end
end

function setColorChangeSpeed(newSpeed)
    if newSpeed and newSpeed > 0 then
        config.colorSpeed = newSpeed
        print("Color change speed set to: " .. newSpeed)
    end
end

function setRotationSpeed(newSpeed)
    if newSpeed then
        config.rotationSpeed = newSpeed
        print("Rotation speed set to: " .. newSpeed)
    end
end

function stopMovement()
    setPosition(0, 0, 0)
    setColor(1, 1, 1, 1)
    setRotation(0, 0, 0)
    print("Movement stopped, object reset to center")
end

function randomMovement()
    local x = (math.random() - 0.5) * 2
    local y = (math.random() - 0.5) * 2
    setPosition(x, y, 0)
    
    local r = math.random()
    local g = math.random()
    local b = math.random()
    setColor(r, g, b, 1.0)
    
    print("Random movement applied")
end

-- ============================================================================
-- Информационные функции
-- ============================================================================

function getConfig()
    return config
end

function getState()
    return state
end

print("Movement script loaded successfully")