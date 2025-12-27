-- Демонстрационный скрипт для Mental Engine
-- Показывает использование функций init и update

local frameCount = 0
local rotation = 0.0
local lastPrintFrame = 0

function init()
    print("=== ДЕМО СКРИПТ ЗАПУЩЕН ===")
    print("Инициализация объекта...")
    
    -- Инициализация переменных
    frameCount = 0
    rotation = 0.0
    lastPrintFrame = 0
    
    print("Объект готов к анимации!")
    print("Скрипт будет выводить информацию каждые 120 кадров")
end

function update()
    -- Увеличиваем счетчик кадров
    frameCount = frameCount + 1
    
    -- Обновляем вращение (примерно 1 оборот за 5 секунд при 60 FPS)
    rotation = rotation + 0.02
    
    -- Сбрасываем вращение при полном обороте
    if rotation >= 2 * math.pi then
        rotation = rotation - 2 * math.pi
        print("Полный оборот завершен! Кадр: " .. frameCount)
    end
    
    -- Выводим статистику каждые 120 кадров (примерно каждые 2 секунды)
    if frameCount - lastPrintFrame >= 120 then
        local degrees = math.deg(rotation)
        print(string.format("Кадр: %d | Поворот: %.1f° (%.3f рад)", 
              frameCount, degrees, rotation))
        lastPrintFrame = frameCount
    end
end

-- Дополнительная функция (не вызывается автоматически, но может быть добавлена в будущем)
function cleanup()
    print("Очистка ресурсов скрипта")
    print("Всего обработано кадров: " .. frameCount)
end