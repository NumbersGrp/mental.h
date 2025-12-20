--[[
    Keyboard Control Script для Mental Engine
    
    Демонстрирует простое управление объектом с помощью клавиатуры.
    Поддерживает стрелочки и WASD для движения с визуальной обратной связью.
--]]

-- ============================================================================
-- Настройки управления
-- ============================================================================

local config = {
    moveSpeed = 2.0,                            -- Скорость движения (единиц в секунду)
    maxX = 3.0,                                 -- Максимальная позиция по X
    maxY = 2.0,                                 -- Максимальная позиция по Y
    baseColor = {0.2, 0.8, 0.2, 1.0},         -- Зеленый цвет в покое
    moveColor = {0.8, 0.2, 0.2, 1.0}          -- Красный цвет при движении
}

-- ============================================================================
-- Переменные состояния
-- ============================================================================

local state = {
    position = {0, 0, 0},
    isMoving = false,
    initialized = false
}

-- Кэшированные коды клавиш
local keys = {}

-- ============================================================================
-- Вспомогательные функции
-- ============================================================================

local function clampPosition()
    -- Ограничиваем движение в пределах экрана
    if state.position[1] > config.maxX then
        state.position[1] = config.maxX
    elseif state.position[1] < -config.maxX then
        state.position[1] = -config.maxX
    end
    
    if state.position[2] > config.maxY then
        state.position[2] = config.maxY
    elseif state.position[2] < -config.maxY then
        state.position[2] = -config.maxY
    end
end

local function updateColor()
    if state.isMoving then
        setColor(config.moveColor[1], config.moveColor[2], config.moveColor[3], config.moveColor[4])
    else
        setColor(config.baseColor[1], config.baseColor[2], config.baseColor[3], config.baseColor[4])
    end
end

-- ============================================================================
-- Основные функции скрипта
-- ============================================================================

function init()
    print("=== Keyboard Control Script Initialized ===")
    
    -- Получаем коды клавиш
    keys.up = KEY_UP()
    keys.down = KEY_DOWN()
    keys.left = KEY_LEFT()
    keys.right = KEY_RIGHT()
    keys.w = KEY_W()
    keys.a = KEY_A()
    keys.s = KEY_S()
    keys.d = KEY_D()
    
    -- Устанавливаем начальные параметры
    state.position = {0, 0, 0}
    setPosition(0, 0, 0)
    setScale(1.0, 1.0, 1.0)
    setVisible(true)
    updateColor()
    
    state.initialized = true
    
    print("Keyboard control initialized")
    print("Controls:")
    print("  Arrow Keys - Move object")
    print("  WASD - Alternative movement")
    print("  Green = idle, Red = moving")
    print("Settings:")
    print("  Move Speed: " .. config.moveSpeed)
    print("  Boundaries: ±" .. config.maxX .. " (X), ±" .. config.maxY .. " (Y)")
end

function update(deltaTime)
    if not state.initialized then
        return
    end
    
    local dx = 0
    local dy = 0
    state.isMoving = false
    
    -- Проверяем стрелочки
    if isKeyPressed(keys.up) then
        dy = dy + config.moveSpeed * deltaTime
        state.isMoving = true
    end
    if isKeyPressed(keys.down) then
        dy = dy - config.moveSpeed * deltaTime
        state.isMoving = true
    end
    if isKeyPressed(keys.left) then
        dx = dx - config.moveSpeed * deltaTime
        state.isMoving = true
    end
    if isKeyPressed(keys.right) then
        dx = dx + config.moveSpeed * deltaTime
        state.isMoving = true
    end
    
    -- Проверяем WASD
    if isKeyPressed(keys.w) then
        dy = dy + config.moveSpeed * deltaTime
        state.isMoving = true
    end
    if isKeyPressed(keys.s) then
        dy = dy - config.moveSpeed * deltaTime
        state.isMoving = true
    end
    if isKeyPressed(keys.a) then
        dx = dx - config.moveSpeed * deltaTime
        state.isMoving = true
    end
    if isKeyPressed(keys.d) then
        dx = dx + config.moveSpeed * deltaTime
        state.isMoving = true
    end
    
    -- Обновляем позицию если есть движение
    if dx ~= 0 or dy ~= 0 then
        state.position[1] = state.position[1] + dx
        state.position[2] = state.position[2] + dy
        clampPosition()
        setPosition(state.position[1], state.position[2], state.position[3])
    end
    
    -- Обновляем цвет
    updateColor()
end

function render()
    -- Дополнительная логика рендеринга (если необходимо)
end

-- ============================================================================
-- Пользовательские функции для настройки
-- ============================================================================

function setMoveSpeed(speed)
    if speed and speed > 0 then
        config.moveSpeed = speed
        print("Move speed set to: " .. speed)
    end
end

function setBoundaries(maxX, maxY)
    if maxX and maxY and maxX > 0 and maxY > 0 then
        config.maxX = maxX
        config.maxY = maxY
        print("Boundaries set to: ±" .. maxX .. " (X), ±" .. maxY .. " (Y)")
    end
end

function resetPosition()
    state.position = {0, 0, 0}
    setPosition(0, 0, 0)
    print("Position reset to center")
end

function setBaseColor(r, g, b, a)
    if r and g and b then
        config.baseColor = {r, g, b, a or 1.0}
        print("Base color updated")
        if not state.isMoving then
            updateColor()
        end
    end
end

function setMoveColor(r, g, b, a)
    if r and g and b then
        config.moveColor = {r, g, b, a or 1.0}
        print("Move color updated")
        if state.isMoving then
            updateColor()
        end
    end
end

function teleportTo(x, y)
    if x and y then
        state.position[1] = x
        state.position[2] = y
        clampPosition()
        setPosition(state.position[1], state.position[2], state.position[3])
        print("Teleported to: " .. state.position[1] .. ", " .. state.position[2])
    end
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

function getPosition()
    return state.position[1], state.position[2], state.position[3]
end

print("Keyboard control script loaded successfully")