-- Скрипт управления камерой через Lua

print("=== Camera Control Script Loaded ===")

-- Переменные состояния
local initialized = false
local time_offset = 0
local camera_speed = 2.0
local rotation_speed = 1.0

function init()
    print("Инициализация камеры")
    
    -- Устанавливаем начальную позицию камеры
    setCameraPosition(0, 0, 5)
    setCameraTarget(0, 0, 0)
    setCameraUp(0, 1, 0)
    
    -- Настраиваем перспективу
    setPerspective(45.0, 16.0/9.0, 0.1, 100.0)
    
    time_offset = getTime()
    initialized = true
    
    print("Камера инициализирована")
    print("Позиция камеры:", getCameraPosition())
    print("Цель камеры:", getCameraTarget())
end

function update(deltaTime)
    if not initialized then
        return
    end
    
    local current_time = getTime() - time_offset
    
    -- Автоматическое вращение камеры вокруг объекта
    local radius = 5.0
    local height = 2.0 + sin(current_time * 0.5) * 1.0
    
    local x = cos(current_time * 0.3) * radius
    local z = sin(current_time * 0.3) * radius
    
    -- Устанавливаем позицию камеры по кругу
    setCameraPosition(x, height, z)
    
    -- Всегда смотрим на центр
    setCameraTarget(0, 0, 0)
    
    -- Небольшое покачивание FOV для эффекта
    local fov = 45.0 + sin(current_time * 2.0) * 5.0
    setFOV(fov)
    
    -- Можно добавить управление с клавиатуры (когда будет реализовано)
    -- if isKeyPressed(KEY_W()) then
    --     moveForward(camera_speed * deltaTime)
    -- end
    -- if isKeyPressed(KEY_S()) then
    --     moveBackward(camera_speed * deltaTime)
    -- end
    -- if isKeyPressed(KEY_A()) then
    --     moveLeft(camera_speed * deltaTime)
    -- end
    -- if isKeyPressed(KEY_D()) then
    --     moveRight(camera_speed * deltaTime)
    -- end
end

function render()
    -- Дополнительная логика рендеринга камеры (если нужна)
end

print("=== Camera Control Script Ready ===")