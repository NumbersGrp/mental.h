-- Тестовый скрипт для проверки ввода

print("=== Input Test Script Loaded ===")

local initialized = false

function init()
    print("Инициализация тестирования ввода")
    
    -- Устанавливаем начальную позицию камеры
    setCameraPosition(0, 0, 3)
    setCameraTarget(0, 0, 0)
    setCameraUp(0, 1, 0)
    setPerspective(45.0, 16.0/9.0, 0.1, 100.0)
    
    initialized = true
    
    print("Тест ввода готов")
    print("Нажимайте клавиши для тестирования:")
    print("W, A, S, D, стрелочки")
end

function update(deltaTime)
    if not initialized then
        return
    end
    
    -- Тестируем каждую клавишу и выводим сообщение при нажатии
    if isKeyPressed(KEY_W()) then
        print("Нажата клавиша W")
        moveForward(1.0 * deltaTime)
    end
    if isKeyPressed(KEY_S()) then
        print("Нажата клавиша S")
        moveBackward(1.0 * deltaTime)
    end
    if isKeyPressed(KEY_A()) then
        print("Нажата клавиша A")
        moveLeft(1.0 * deltaTime)
    end
    if isKeyPressed(KEY_D()) then
        print("Нажата клавиша D")
        moveRight(1.0 * deltaTime)
    end
    
    if isKeyPressed(KEY_UP()) then
        print("Нажата стрелочка ВВЕРХ")
        rotatePitch(-0.5 * deltaTime)
    end
    if isKeyPressed(KEY_DOWN()) then
        print("Нажата стрелочка ВНИЗ")
        rotatePitch(0.5 * deltaTime)
    end
    if isKeyPressed(KEY_LEFT()) then
        print("Нажата стрелочка ВЛЕВО")
        rotateYaw(-0.5 * deltaTime)
    end
    if isKeyPressed(KEY_RIGHT()) then
        print("Нажата стрелочка ВПРАВО")
        rotateYaw(0.5 * deltaTime)
    end
end

function render()
    -- Дополнительная логика рендеринга
end

print("=== Input Test Script Ready ===")