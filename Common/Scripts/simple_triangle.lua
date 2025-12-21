-- Простой скрипт треугольника без установки позиции
-- Позиция управляется из root_scene.lua

print("=== Simple Triangle Script Loaded ===")

local initialized = false

function init()
    print("Инициализация простого треугольника")
    initialized = true
    print("Простой треугольник инициализирован")
end

function update(deltaTime)
    if not initialized then
        return
    end
    
    -- Здесь можно добавить простую анимацию, но БЕЗ изменения позиции
    -- Позиция контролируется из root_scene.lua
end

function render()
    -- Дополнительная логика рендеринга
end

print("=== Simple Triangle Script Ready ===")