# CMentalRootNode - Руководство по использованию

## Обзор

CMentalRootNode - это корневой узел сцены, который позволяет полностью управлять созданием объектов, шейдеров и их связыванием через Lua скрипты. Это дает полный контроль над сценой из скриптов.

## Основные возможности

### 1. Создание объектов из Lua
```lua
-- Создание прямоугольника
local success = createObject("my_rectangle", "rectangle")

-- Создание треугольника  
local success = createObject("my_triangle", "triangle")
```

### 2. Создание шейдеров из Lua
```lua
-- Создание шейдера из файлов
local success = createShader("my_shader", "path/to/vertex.glsl", "path/to/fragment.glsl")
```

### 3. Подключение шейдеров к объектам
```lua
-- Подключение шейдера к объекту
local success = attachShaderToObject("my_rectangle", "my_shader")
```

### 4. Подключение скриптов к объектам
```lua
-- Подключение Lua скрипта к объекту для анимации
local success = attachScriptToObject("my_triangle", "Common/Scripts/animation.lua")
```

### 5. Управление объектами
```lua
-- Получение объекта по имени
local exists = getObjectByName("my_rectangle")

-- Удаление объекта
local success = removeObject("my_rectangle")

-- Список всех объектов
listObjects()
```

## Пример использования

### Создание корневого скрипта сцены

```lua
-- Common/Scripts/my_scene.lua

function init()
    print("Инициализация сцены")
    
    -- Создаем шейдеры
    createShader("rainbow", "Common/Shaders/Rainbow/vertex_shader.glsl", 
                           "Common/Shaders/Rainbow/fragment_shader.glsl")
    createShader("glow", "Common/Shaders/Glow/vertex_shader.glsl", 
                         "Common/Shaders/Glow/fragment_shader.glsl")
    
    -- Создаем объекты
    createObject("background", "rectangle")
    createObject("player", "triangle")
    createObject("enemy", "triangle")
    
    -- Подключаем шейдеры
    attachShaderToObject("background", "rainbow")
    attachShaderToObject("player", "glow")
    attachShaderToObject("enemy", "glow")
    
    -- Подключаем скрипты поведения
    attachScriptToObject("player", "Common/Scripts/player_control.lua")
    attachScriptToObject("enemy", "Common/Scripts/enemy_ai.lua")
    
    -- Выводим список созданных объектов
    listObjects()
end

function update(deltaTime)
    -- Глобальная логика сцены
end

function render()
    -- Глобальные эффекты рендеринга
end
```

### Использование в C++

```cpp
#include "mentalsdk.hpp"

int main() {
    // Создание окна и рендерера
    mentalsdk::CMentalWindowManager<> windowManager;
    auto* pipeline = windowManager.GetRenderer()->GetRenderPipeline();
    auto rootNode = pipeline->GetRootNode();
    
    // Загрузка корневого скрипта сцены
    rootNode->LoadLuaScript("Common/Scripts/my_scene.lua");
    
    // Создание и настройка камеры
    auto camera = std::make_shared<mentalsdk::CMentalCamera>();
    camera->LoadLuaScript("Common/Scripts/camera_control.lua");
    rootNode->SetCamera(camera);
    
    // Основной цикл
    while (!windowManager.ShouldClose()) {
        windowManager.Clear();
        pipeline->UpdateAll(deltaTime);
        pipeline->RenderAll();
        windowManager.SwapBuffers();
        windowManager.PollEvents();
    }
    
    return 0;
}
```

## API функции для Lua

### Создание объектов
- `createObject(name, type)` - создает объект с именем и типом ("rectangle" или "triangle")
- `removeObject(name)` - удаляет объект по имени
- `getObjectByName(name)` - проверяет существование объекта

### Создание шейдеров
- `createShader(name, vertex_path, fragment_path)` - создает шейдер из файлов

### Связывание
- `attachShaderToObject(object_name, shader_name)` - подключает шейдер к объекту
- `attachScriptToObject(object_name, script_path)` - подключает Lua скрипт к объекту

### Отладка
- `listObjects()` - выводит список всех объектов и шейдеров

## Архитектура

```
CMentalRootNode
├── Lua Script (корневой скрипт сцены)
├── Objects (именованные объекты)
│   ├── CMentalObject + CMentalShaderMaterial + CMentalLuaScript
│   └── ...
├── Shaders (именованные шейдеры)
│   ├── CMentalShaderMaterial
│   └── ...
└── Camera (камера с Lua скриптом)
```

## Преимущества

1. **Полный контроль из Lua** - вся сцена создается и управляется скриптами
2. **Именованные объекты** - легкий доступ к объектам по имени
3. **Динамическое создание** - объекты можно создавать и удалять во время выполнения
4. **Модульность** - каждый объект может иметь свой собственный скрипт поведения
5. **Централизованное управление** - все объекты управляются через один корневой узел

## Примеры сценариев использования

1. **Игровая сцена** - создание игроков, врагов, фона через скрипты
2. **Визуализация данных** - динамическое создание графиков и диаграмм
3. **Интерактивные демо** - создание объектов по пользовательскому вводу
4. **Процедурная генерация** - создание сцен по алгоритмам в Lua

Теперь у вас есть полный контроль над сценой через Lua скрипты!