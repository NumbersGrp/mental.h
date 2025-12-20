# Lua Scripting System для Mental Engine

Эта система позволяет управлять объектами в игре с помощью Lua скриптов.

## Основные возможности

- **Управление позицией**: `setPosition(x, y, z)`, `getPosition()`
- **Управление поворотом**: `setRotation(x, y, z)`
- **Управление масштабом**: `setScale(x, y, z)`
- **Управление цветом**: `setColor(r, g, b, a)`
- **Управление скоростью**: `setSpeed(speed)`
- **Управление видимостью**: `setVisible(true/false)`

## Доступные функции в Lua

### Функции объекта
```lua
-- Установка позиции объекта
setPosition(x, y, z)

-- Получение текущей позиции
x, y, z = getPosition()

-- Установка поворота (в градусах)
setRotation(x, y, z)

-- Установка масштаба
setScale(x, y, z)

-- Установка цвета (значения от 0.0 до 1.0)
setColor(r, g, b, a)

-- Установка скорости движения
setSpeed(speed)

-- Установка видимости
setVisible(true)  -- показать объект
setVisible(false) -- скрыть объект
```

### Функции ввода

- `isKeyPressed(key)` - проверка нажатия клавиши
- `KEY_UP()`, `KEY_DOWN()`, `KEY_LEFT()`, `KEY_RIGHT()` - коды стрелочек
- `KEY_W()`, `KEY_A()`, `KEY_S()`, `KEY_D()` - коды WASD клавиш

### Функции ввода
```lua
-- Проверка нажатия клавиши
if isKeyPressed(KEY_UP()) then
    -- Клавиша стрелка вверх нажата
end

-- Получение кодов клавиш
local up = KEY_UP()
local down = KEY_DOWN()
local left = KEY_LEFT()
local right = KEY_RIGHT()
local w = KEY_W()
local a = KEY_A()
local s = KEY_S()
local d = KEY_D()
```

### Математические функции
```lua
-- Синус и косинус
result = sin(angle)
result = cos(angle)

-- Получение времени
time = getTime()
```

## Структура Lua скрипта

Каждый Lua скрипт может содержать следующие функции:

### `init()` - Инициализация
Вызывается один раз при загрузке скрипта.
```lua
function init()
    print("Скрипт инициализирован")
    setPosition(0, 0, 0)
    setColor(1, 1, 1, 1)
end
```

### `update(deltaTime)` - Обновление
Вызывается каждый кадр. `deltaTime` - время с предыдущего кадра.
```lua
function update(deltaTime)
    local time = getTime()
    local x = sin(time) * 2
    setPosition(x, 0, 0)
end
```

### `render()` - Рендеринг
Вызывается перед отрисовкой объекта.
```lua
function render()
    -- Дополнительная логика рендеринга
end
```

## Примеры скриптов

### 1. movement.lua - Движущийся объект
Объект движется по синусоиде и меняет цвет.

### 2. pulse.lua - Пульсирующий объект
Объект изменяет размер и прозрачность.

### 3. keyboard_control.lua - Управление клавиатурой
Демонстрирует управление объектом с клавиатуры:
- Движение стрелочками или WASD
- Изменение цвета при движении
- Ограничение движения в пределах экрана

### 4. smooth_control.lua - Плавное управление
Демонстрирует продвинутое управление с физикой:
- Плавное ускорение и торможение
- Инерция движения
- Поворот объекта в направлении движения
- Изменение размера в зависимости от скорости

## Создание объекта с Lua скриптом

```c
// Создание информации о материале
MentalMaterialInfo* materialInfo = mentalRendererCreateRainbowMaterialInfo();

// Создание информации об объекте
MentalObjectInfo* objectInfo = (MentalObjectInfo*)malloc(sizeof(MentalObjectInfo));
objectInfo->eType = MENTAL_OBJECT_TYPE_RECTANGLE;
objectInfo->pMaterialInfo = materialInfo;

// Создание Lua объекта с скриптом
MentalLuaObject* luaObject = mentalLuaObjectCreate(objectInfo, "Common/Scripts/movement.lua");

// Добавление в пул рендеринга
mentalRendererOpenGLRenderPoolAddLuaObject(pPool, luaObject);
```

## Обновление объектов в главном цикле

```c
// В главном цикле игры
float deltaTime = calculateDeltaTime();
mentalRendererOpenGLRenderPoolUpdate(pPool, deltaTime);
mentalRendererOpenGLRenderPoolRender(pPool);
```

## Перезагрузка скриптов

```c
// Перезагрузка скрипта во время выполнения
mentalLuaObjectReloadScript(luaObject);
```

## Вызов пользовательских функций

```c
// Вызов функции из Lua скрипта
mentalLuaObjectCallFunction(luaObject, "stopMovement");
```

## Советы по написанию скриптов

1. **Используйте локальные переменные** для лучшей производительности
2. **Кэшируйте вычисления** которые не изменяются каждый кадр
3. **Проверяйте границы** при движении объектов
4. **Используйте deltaTime** для плавной анимации независимо от FPS

## Отладка

Используйте `print()` для вывода отладочной информации:
```lua
function update(deltaTime)
    print("Delta time:", deltaTime)
    print("Current position:", getPosition())
end
```