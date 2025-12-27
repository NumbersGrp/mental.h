# Mental Engine Script System

## Обзор

Система скриптов Mental Engine позволяет прикреплять Lua скрипты к объектам для управления их поведением во время рендеринга. Каждый скрипт может содержать функции `init` и `update`, которые автоматически вызываются движком, а также функции для возврата трансформаций объекта.

## Жизненный цикл скрипта

1. **Загрузка**: Скрипт загружается и выполняется один раз для определения функций
2. **Инициализация**: Функция `init()` вызывается один раз при первом рендере объекта
3. **Обновление**: Функция `update()` вызывается каждый кадр во время рендеринга
4. **Получение трансформаций**: Функции `getRotation()`, `getPosition()`, `getScale()` вызываются для получения новых значений трансформации

## Структура скрипта

### Базовая структура с вращением

```lua
local rotationY = 0.0

function init()
    print("Объект инициализирован!")
    rotationY = 0.0
end

function update()
    rotationY = rotationY + 0.02  -- Увеличиваем вращение
end

function getRotation()
    return rotationY  -- Возвращаем текущее вращение по Y
end
```

### Полная структура с позицией, вращением и масштабом

```lua
local position = {x = 0.0, y = 0.0, z = 0.0}
local rotation = {x = 0.0, y = 0.0, z = 0.0}
local scale = {x = 1.0, y = 1.0, z = 1.0}

function init()
    -- Инициализация
end

function update()
    -- Обновление трансформаций
    rotation.y = rotation.y + 0.02
end

function getRotation()
    return rotation.y  -- Возвращает только Y вращение
end

function getPosition()
    return position.x, position.y, position.z
end

function getScale()
    return scale.x, scale.y, scale.z
end
```

## Примеры использования

### 1. Простое вращение

```lua
-- simple_rotate.lua
local rotationY = 0.0

function init()
    print("Начинаем вращение!")
    rotationY = 0.0
end

function update()
    rotationY = rotationY + 0.02
    if rotationY > 6.28 then  -- 2 * PI
        rotationY = 0.0
    end
end

function getRotation()
    return rotationY
end
```

### 2. Комплексная анимация

```lua
-- full_transform_script.lua
local time = 0.0

function init()
    time = 0.0
end

function update()
    time = time + 0.016  -- ~60 FPS
end

function getRotation()
    return time * 1.0  -- 1 радиан в секунду
end

function getPosition()
    local x = math.sin(time * 0.5) * 2.0
    local z = math.cos(time * 0.5) * 2.0
    return x, 0.0, z
end

function getScale()
    local s = 1.0 + 0.5 * math.sin(time * 2.0)
    return s, s, s
end
```

## Доступные функции

### Обязательные функции (вызываются автоматически)

- `init()` - инициализация скрипта (вызывается один раз)
- `update()` - обновление логики (вызывается каждый кадр)

### Функции возврата трансформаций (опциональные)

- `getRotation()` - возвращает вращение по Y оси в радианах
- `getPosition()` - возвращает позицию (x, y, z)
- `getScale()` - возвращает масштаб (x, y, z)

### Глобальные переменные (доступны в скрипте)

- `position` - текущая позиция объекта (таблица с x, y, z)
- `rotation` - текущее вращение объекта (таблица с x, y, z)
- `scale` - текущий масштаб объекта (таблица с x, y, z)

## Подключение скрипта к объекту

```cpp
// Создание объекта
auto triangle = CMentalObject::createTriangle("MyTriangle");

// Подключение шейдера
triangle->connectShader("vertex.glsl", "fragment.glsl");

// Подключение скрипта
triangle->connectScript("common/Scripts/simple_rotate.lua");

// Объект будет автоматически вращаться!
```

## Готовые скрипты

- `simple_rotate.lua` - простое вращение вокруг Y оси
- `rotate_script.lua` - базовый скрипт вращения с отладочным выводом
- `full_transform_script.lua` - полная анимация с позицией, вращением и масштабом
- `demo_script.lua` - демонстрационный скрипт с комментариями

## Отладка

```lua
function update()
    -- Выводить информацию каждые 60 кадров
    if frameCount % 60 == 0 then
        print("Rotation: " .. string.format("%.2f", rotationY) .. " rad")
    end
end
```

## Важные замечания

- Функция `getRotation()` должна возвращать значение в радианах
- Функции `getPosition()` и `getScale()` должны возвращать 3 значения (x, y, z)
- Если функция не определена, соответствующая трансформация не изменяется
- Скрипты выполняются каждый кадр, поэтому избегайте тяжелых вычислений