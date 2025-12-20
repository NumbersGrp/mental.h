-- Продвинутое управление с плавным движением и инерцией
-- Стрелочки для управления, плавное торможение

-- Настройки физики
local acceleration = 8.0     -- Ускорение при нажатии клавиш
local friction = 6.0         -- Трение (замедление)
local maxSpeed = 4.0         -- Максимальная скорость

-- Состояние объекта
local position = {0, 0, 0}
local velocity = {0, 0}      -- Скорость по X и Y
local rotation = 0           -- Угол поворота

-- Цвета
local idleColor = {0.3, 0.3, 0.8, 1.0}    -- Синий в покое
local moveColor = {0.8, 0.3, 0.3, 1.0}    -- Красный при движении

-- Коды клавиш
local keyUp, keyDown, keyLeft, keyRight

function init()
    print("Smooth control script initialized")
    
    -- Получаем коды клавиш
    keyUp = KEY_UP()
    keyDown = KEY_DOWN()
    keyLeft = KEY_LEFT()
    keyRight = KEY_RIGHT()
    
    -- Начальные параметры
    setPosition(0, 0, 0)
    setColor(idleColor[1], idleColor[2], idleColor[3], idleColor[4])
    setScale(1.0, 1.0, 1.0)
    setRotation(0, 0, 0)
    setVisible(true)
    
    print("Use arrow keys for smooth movement with physics")
end

function update(deltaTime)
    -- Вектор ускорения
    local accelX = 0
    local accelY = 0
    
    -- Проверяем нажатые клавиши
    if isKeyPressed(keyUp) then
        accelY = accelY + acceleration
    end
    if isKeyPressed(keyDown) then
        accelY = accelY - acceleration
    end
    if isKeyPressed(keyLeft) then
        accelX = accelX - acceleration
    end
    if isKeyPressed(keyRight) then
        accelX = accelX + acceleration
    end
    
    -- Применяем ускорение к скорости
    velocity[1] = velocity[1] + accelX * deltaTime
    velocity[2] = velocity[2] + accelY * deltaTime
    
    -- Применяем трение если нет ускорения
    if accelX == 0 then
        if velocity[1] > 0 then
            velocity[1] = velocity[1] - friction * deltaTime
            if velocity[1] < 0 then velocity[1] = 0 end
        elseif velocity[1] < 0 then
            velocity[1] = velocity[1] + friction * deltaTime
            if velocity[1] > 0 then velocity[1] = 0 end
        end
    end
    
    if accelY == 0 then
        if velocity[2] > 0 then
            velocity[2] = velocity[2] - friction * deltaTime
            if velocity[2] < 0 then velocity[2] = 0 end
        elseif velocity[2] < 0 then
            velocity[2] = velocity[2] + friction * deltaTime
            if velocity[2] > 0 then velocity[2] = 0 end
        end
    end
    
    -- Ограничиваем максимальную скорость
    local speed = math.sqrt(velocity[1] * velocity[1] + velocity[2] * velocity[2])
    if speed > maxSpeed then
        local scale = maxSpeed / speed
        velocity[1] = velocity[1] * scale
        velocity[2] = velocity[2] * scale
    end
    
    -- Обновляем позицию
    position[1] = position[1] + velocity[1] * deltaTime
    position[2] = position[2] + velocity[2] * deltaTime
    
    -- Ограничиваем движение в пределах экрана
    local maxX = 3.5
    local maxY = 2.5
    
    if position[1] > maxX then
        position[1] = maxX
        velocity[1] = 0  -- Останавливаем при столкновении со стеной
    elseif position[1] < -maxX then
        position[1] = -maxX
        velocity[1] = 0
    end
    
    if position[2] > maxY then
        position[2] = maxY
        velocity[2] = 0
    elseif position[2] < -maxY then
        position[2] = -maxY
        velocity[2] = 0
    end
    
    -- Устанавливаем позицию
    setPosition(position[1], position[2], position[3])
    
    -- Поворачиваем объект в направлении движения
    if velocity[1] ~= 0 or velocity[2] ~= 0 then
        rotation = math.atan2(velocity[2], velocity[1]) * 180 / math.pi
        setRotation(0, 0, rotation)
    end
    
    -- Меняем цвет в зависимости от скорости
    local currentSpeed = math.sqrt(velocity[1] * velocity[1] + velocity[2] * velocity[2])
    local speedRatio = currentSpeed / maxSpeed
    
    if speedRatio > 0.1 then
        -- Интерполируем между цветами в зависимости от скорости
        local r = idleColor[1] + (moveColor[1] - idleColor[1]) * speedRatio
        local g = idleColor[2] + (moveColor[2] - idleColor[2]) * speedRatio
        local b = idleColor[3] + (moveColor[3] - idleColor[3]) * speedRatio
        setColor(r, g, b, 1.0)
    else
        setColor(idleColor[1], idleColor[2], idleColor[3], idleColor[4])
    end
    
    -- Масштабируем объект в зависимости от скорости
    local scale = 1.0 + speedRatio * 0.3
    setScale(scale, scale, 1.0)
end

function render()
    -- Дополнительная логика рендеринга
end

-- Функции настройки
function setPhysicsParams(accel, fric, maxSpd)
    acceleration = accel or acceleration
    friction = fric or friction
    maxSpeed = maxSpd or maxSpeed
    print("Physics updated: accel=" .. acceleration .. ", friction=" .. friction .. ", maxSpeed=" .. maxSpeed)
end

function resetPhysics()
    velocity = {0, 0}
    position = {0, 0, 0}
    rotation = 0
    setPosition(0, 0, 0)
    setRotation(0, 0, 0)
    setScale(1, 1, 1)
    print("Physics reset")
end