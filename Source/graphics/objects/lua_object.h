#ifndef MENTAL_LUA_OBJECT_H
#define MENTAL_LUA_OBJECT_H

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct GLFWwindow GLFWwindow;
typedef struct MentalObject MentalObject;
typedef struct MentalObjectInfo MentalObjectInfo;

// Константы
#define MENTAL_LUA_MAX_SCRIPT_PATH 256
#define MENTAL_LUA_DEFAULT_SPEED 1.0f

/**
 * @brief Структура для хранения Lua состояния объекта
 */
typedef struct MentalLuaObjectState {
    lua_State* L;
    char* scriptPath;
    int scriptLoaded;
    
    // Позиция и трансформация
    float position[3];
    float rotation[3];
    float scale[3];
    
    // Свойства
    float color[4];
    float speed;
    int visible;
    
    // Пользовательские свойства
    lua_State* propertiesTable;
} MentalLuaObjectState;

/**
 * @brief Расширенная структура объекта с Lua поддержкой
 */
typedef struct MentalLuaObject {
    MentalObject* baseObject;
    MentalLuaObjectState* luaState;
} MentalLuaObject;

// ============================================================================
// Функции для работы с Lua объектами
// ============================================================================

/**
 * @brief Создает новый Lua объект
 * @param pInfo Информация для создания базового объекта
 * @param scriptPath Путь к Lua скрипту (может быть NULL)
 * @return Указатель на созданный Lua объект или NULL при ошибке
 */
MentalLuaObject* mentalLuaObjectCreate(MentalObjectInfo* pInfo, const char* scriptPath);

/**
 * @brief Уничтожает Lua объект и освобождает память
 * @param pLuaObject Lua объект для уничтожения
 */
void mentalLuaObjectDestroy(MentalLuaObject* pLuaObject);

/**
 * @brief Обновляет Lua объект (вызывает update функцию в скрипте)
 * @param pLuaObject Lua объект для обновления
 * @param deltaTime Время с предыдущего кадра
 */
void mentalLuaObjectUpdate(MentalLuaObject* pLuaObject, float deltaTime);

/**
 * @brief Рендерит Lua объект
 * @param pLuaObject Lua объект для рендеринга
 */
void mentalLuaObjectRender(MentalLuaObject* pLuaObject);

// ============================================================================
// Функции для работы со скриптами
// ============================================================================

/**
 * @brief Загружает Lua скрипт для объекта
 * @param pLuaObject Lua объект
 * @param scriptPath Путь к скрипту
 * @return 1 при успехе, 0 при ошибке
 */
int mentalLuaObjectLoadScript(MentalLuaObject* pLuaObject, const char* scriptPath);

/**
 * @brief Перезагружает текущий скрипт
 * @param pLuaObject Lua объект
 * @return 1 при успехе, 0 при ошибке
 */
int mentalLuaObjectReloadScript(MentalLuaObject* pLuaObject);

/**
 * @brief Вызывает функцию в Lua скрипте
 * @param pLuaObject Lua объект
 * @param functionName Имя функции для вызова
 * @return 1 при успехе, 0 при ошибке
 */
int mentalLuaObjectCallFunction(MentalLuaObject* pLuaObject, const char* functionName);

// ============================================================================
// Функции для установки/получения свойств из C кода
// ============================================================================

/**
 * @brief Устанавливает позицию объекта
 */
void mentalLuaObjectSetPosition(MentalLuaObject* pLuaObject, float x, float y, float z);

/**
 * @brief Получает позицию объекта
 */
void mentalLuaObjectGetPosition(MentalLuaObject* pLuaObject, float* x, float* y, float* z);

/**
 * @brief Устанавливает поворот объекта
 */
void mentalLuaObjectSetRotation(MentalLuaObject* pLuaObject, float x, float y, float z);

/**
 * @brief Устанавливает масштаб объекта
 */
void mentalLuaObjectSetScale(MentalLuaObject* pLuaObject, float x, float y, float z);

/**
 * @brief Устанавливает цвет объекта
 */
void mentalLuaObjectSetColor(MentalLuaObject* pLuaObject, float r, float g, float b, float a);

/**
 * @brief Устанавливает скорость объекта
 */
void mentalLuaObjectSetSpeed(MentalLuaObject* pLuaObject, float speed);

/**
 * @brief Устанавливает видимость объекта
 */
void mentalLuaObjectSetVisible(MentalLuaObject* pLuaObject, int visible);

// ============================================================================
// Функции для работы с вводом
// ============================================================================

/**
 * @brief Проверяет нажатие клавиши
 * @param key Код клавиши (GLFW константа)
 * @return 1 если клавиша нажата, 0 иначе
 */
int mentalLuaInputIsKeyPressed(int key);

/**
 * @brief Устанавливает окно для обработки ввода
 * @param window GLFW окно
 */
void mentalLuaInputSetWindow(GLFWwindow* window);

// ============================================================================
// Функции для регистрации C API в Lua
// ============================================================================

/**
 * @brief Регистрирует все API функции в Lua состоянии
 * @param L Lua состояние
 * @param pLuaObject Lua объект для привязки
 */
void mentalLuaRegisterObjectAPI(lua_State* L, MentalLuaObject* pLuaObject);

#ifdef __cplusplus
}
#endif

#endif // MENTAL_LUA_OBJECT_H