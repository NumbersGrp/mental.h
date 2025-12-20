--[[
    Pulse Script для Mental Engine
    
    Демонстрирует пульсирующий эффект с изменением размера и прозрачности.
    Показывает работу с масштабированием и альфа-каналом.
--]]

-- ============================================================================
-- Настройки пульсации
-- ============================================================================

local config = {
    pulseSpeed = 3.0,           -- Скорость пульсации
    minScale = 0.5,             -- Минимальный размер
    maxScale = 1.5,             -- Максимальный размер
    minAlpha = 0.3,             -- Минимальная прозрачность
    maxAlpha = 1.0,             -- Максимальная прозрачность
    rotationSpeed = 30.0,       -- Скорость вращения (градусы в секунду)
    baseColor = {1.0, 0.5, 0.2} -- Базовый цвет (оранжевый)
}

-- ============================================================================
-- Переменные состояния
-- ============================================================================

local state = {
    startTime = 0,
    initialized = false
}

-- ============================================================================
-- Вспомогательные функции
-- ============================================================================

local function lerp(a, b, t)
    return a + (b - a) * t
end

local function normalizedSin(time, speed)
    return (sin(time * speed) + 1) * 0.5
end

local function normalizedCos(time, speed)
    return (cos(time * speed) + 1) * 0.5
end

-- ============================================================================
-- Основные функции скрипта
-- ============================================================================

function init()
    print("=== Pulse Script Initialized ===")
    
    state.startTime = getTime()
    state.initialized = true
    
    -- Устанавливаем начальные параметры
    setPosition(0, 0, 0)
    setVisible(true)
    
    print("Pulse object initialized")
    print("Pulse settings:")
    print("  Speed: " .. config.pulseSpeed)
    print("  Scale range: " .. config.minScale .. " - " .. config.maxScale)
    print("  Alpha range: " .. config.minAlpha .. " - " .. config.maxAlpha)
end

function update(deltaTime)
    if not state.initialized then
        return
    end
    
    local currentTime = getTime() - state.startTime
    
    -- Пульсирующий размер
    local scaleT = normalizedSin(currentTime, config.pulseSpeed)
    local scaleValue = lerp(config.minScale, config.maxScale, scaleT)
    setScale(scaleValue, scaleValue, 1.0)
    
    -- Пульсирующая прозрачность (с другой фазой)
    local alphaT = normalizedCos(currentTime, config.pulseSpeed * 1.5)
    local alpha = lerp(config.minAlpha, config.maxAlpha, alphaT)
    setColor(config.baseColor[1], config.baseColor[2], config.baseColor[3], alpha)
    
    -- Медленное вращение
    local rotation = currentTime * config.rotationSpeed
    setRotation(0, 0, rotation)
end

function render()
    -- Дополнительная логика рендеринга (если необходимо)
end

-- ============================================================================
-- Пользовательские функции для настройки
-- ============================================================================

function setPulseSpeed(speed)
    if speed and speed > 0 then
        config.pulseSpeed = speed
        print("Pulse speed set to: " .. speed)
    end
end

function setPulseRange(minScale, maxScale)
    if minScale and maxScale and minScale > 0 and maxScale > minScale then
        config.minScale = minScale
        config.maxScale = maxScale
        print("Pulse range set to: " .. minScale .. " - " .. maxScale)
    end
end

function setAlphaRange(minAlpha, maxAlpha)
    if minAlpha and maxAlpha and minAlpha >= 0 and maxAlpha <= 1 and minAlpha < maxAlpha then
        config.minAlpha = minAlpha
        config.maxAlpha = maxAlpha
        print("Alpha range set to: " .. minAlpha .. " - " .. maxAlpha)
    end
end

function setBaseColor(r, g, b)
    if r and g and b and r >= 0 and g >= 0 and b >= 0 and r <= 1 and g <= 1 and b <= 1 then
        config.baseColor[1] = r
        config.baseColor[2] = g
        config.baseColor[3] = b
        print("Base color updated to: " .. r .. ", " .. g .. ", " .. b)
    end
end

function setRotationSpeed(speed)
    if speed then
        config.rotationSpeed = speed
        print("Rotation speed set to: " .. speed)
    end
end

function stopPulse()
    setScale(1, 1, 1)
    setColor(config.baseColor[1], config.baseColor[2], config.baseColor[3], 1.0)
    setRotation(0, 0, 0)
    print("Pulse stopped, object reset to normal state")
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

print("Pulse script loaded successfully")