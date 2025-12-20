-- Демонстрационный скрипт для показа возможностей управления клавиатурой
-- Простое управление стрелочками с визуальными эффектами

-- Настройки
local speed = 3.0
local position = {0, 0, 0}
local trail = {}  -- Массив для следа
local maxTrailLength = 10

-- Цвета для разных направлений
local colors = {
    idle = {0.5, 0.5, 0.5, 1.0},    -- Серый в покое
    up = {0.2, 0.8, 0.2, 1.0},      -- Зеленый вверх
    down = {0.8, 0.2, 0.2, 1.0},    -- Красный вниз
    left = {0.2, 0.2, 0.8, 1.0},    -- Синий влево
    right = {0.8, 0.8, 0.2, 1.0}    -- Желтый вправо
}

-- Коды клавиш
local keys = {}

function init()
    print("=== Keyboard Demo Started ===")
    print("Controls:")
    print("  Arrow Keys - Move object")
    print("  WASD - Alternative movement")
    print("  Object changes color based on direction")
    
    -- Получаем коды клавиш
    keys.up = KEY_UP()
    keys.down = KEY_DOWN()
    keys.left = KEY_LEFT()
    keys.right = KEY_RIGHT()
    keys.w = KEY_W()
    keys.a = KEY_A()
    keys.s = KEY_S()
    keys.d = KEY_D()
    
    -- Начальные параметры
    setPosition(0, 0, 0)
    setColor(colors.idle[1], colors.idle[2], colors.idle[3], colors.idle[4])
    setScale(1.2, 1.2, 1.0)
    setVisible(true)
end

function update(deltaTime)
    local dx = 0
    local dy = 0
    local currentColor = colors.idle
    local isMoving = false
    
    -- Проверяем движение вверх
    if isKeyPressed(keys.up) or isKeyPressed(keys.w) then
        dy = speed * deltaTime
        currentColor = colors.up
        isMoving = true
    end
    
    -- Проверяем движение вниз
    if isKeyPressed(keys.down) or isKeyPressed(keys.s) then
        dy = -speed * deltaTime
        currentColor = colors.down
        isMoving = true
    end
    
    -- Проверяем движение влево
    if isKeyPressed(keys.left) or isKeyPressed(keys.a) then
        dx = -speed * deltaTime
        currentColor = colors.left
        isMoving = true
    end
    
    -- Проверяем движение вправо
    if isKeyPressed(keys.right) or isKeyPressed(keys.d) then
        dx = speed * deltaTime
        currentColor = colors.right
        isMoving = true
    end
    
    -- Обновляем позицию
    if dx ~= 0 or dy ~= 0 then
        position[1] = position[1] + dx
        position[2] = position[2] + dy
        
        -- Ограничиваем движение
        local maxX = 4.0
        local maxY = 3.0
        
        if position[1] > maxX then position[1] = maxX end
        if position[1] < -maxX then position[1] = -maxX end
        if position[2] > maxY then position[2] = maxY end
        if position[2] < -maxY then position[2] = -maxY end
        
        setPosition(position[1], position[2], position[3])
    end
    
    -- Устанавливаем цвет
    setColor(currentColor[1], currentColor[2], currentColor[3], currentColor[4])
    
    -- Эффект пульсации при движении
    if isMoving then
        local time = getTime()
        local pulse = 1.0 + sin(time * 10) * 0.2
        setScale(1.2 * pulse, 1.2 * pulse, 1.0)
    else
        setScale(1.2, 1.2, 1.0)
    end
end

function render()
    -- Дополнительная логика рендеринга
end

print("Demo keyboard script loaded successfully!")