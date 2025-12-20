#include "lua_object.h"
#include "objects.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GLFW/glfw3.h>

// ============================================================================
// Глобальные переменные
// ============================================================================

// Глобальная переменная для текущего объекта (для C API функций)
static MentalLuaObject* g_currentLuaObject = NULL;

// Глобальная переменная для окна (для обработки ввода)
static GLFWwindow* g_window = NULL;

// ============================================================================
// Lua API функции для управления объектом
// ============================================================================

static int lua_object_setPosition(lua_State* L) {
    if (!g_currentLuaObject) {
        return 0;
    }
    
    float x = (float)luaL_checknumber(L, 1);
    float y = (float)luaL_checknumber(L, 2);
    float z = (float)luaL_optnumber(L, 3, 0.0f);
    
    mentalLuaObjectSetPosition(g_currentLuaObject, x, y, z);
    return 0;
}

static int lua_object_getPosition(lua_State* L) {
    if (!g_currentLuaObject) {
        return 0;
    }
    
    float x, y, z;
    mentalLuaObjectGetPosition(g_currentLuaObject, &x, &y, &z);
    
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    lua_pushnumber(L, z);
    return 3;
}

static int lua_object_setRotation(lua_State* L) {
    if (!g_currentLuaObject) {
        return 0;
    }
    
    float x = (float)luaL_checknumber(L, 1);
    float y = (float)luaL_checknumber(L, 2);
    float z = (float)luaL_checknumber(L, 3);
    
    mentalLuaObjectSetRotation(g_currentLuaObject, x, y, z);
    return 0;
}

static int lua_object_setScale(lua_State* L) {
    if (!g_currentLuaObject) {
        return 0;
    }
    
    float x = (float)luaL_checknumber(L, 1);
    float y = (float)luaL_checknumber(L, 2);
    float z = (float)luaL_optnumber(L, 3, 1.0f);
    
    mentalLuaObjectSetScale(g_currentLuaObject, x, y, z);
    return 0;
}

static int lua_object_setColor(lua_State* L) {
    if (!g_currentLuaObject) {
        return 0;
    }
    
    float r = (float)luaL_checknumber(L, 1);
    float g = (float)luaL_checknumber(L, 2);
    float b = (float)luaL_checknumber(L, 3);
    float a = (float)luaL_optnumber(L, 4, 1.0f);
    
    mentalLuaObjectSetColor(g_currentLuaObject, r, g, b, a);
    return 0;
}

static int lua_object_setSpeed(lua_State* L) {
    if (!g_currentLuaObject) {
        return 0;
    }
    
    float speed = (float)luaL_checknumber(L, 1);
    mentalLuaObjectSetSpeed(g_currentLuaObject, speed);
    return 0;
}

static int lua_object_setVisible(lua_State* L) {
    if (!g_currentLuaObject) {
        return 0;
    }
    
    int visible = lua_toboolean(L, 1);
    mentalLuaObjectSetVisible(g_currentLuaObject, visible);
    return 0;
}

// ============================================================================
// Lua API математические функции
// ============================================================================

static int lua_math_sin(lua_State* L) {
    float x = (float)luaL_checknumber(L, 1);
    lua_pushnumber(L, sin(x));
    return 1;
}

static int lua_math_cos(lua_State* L) {
    float x = (float)luaL_checknumber(L, 1);
    lua_pushnumber(L, cos(x));
    return 1;
}

static int lua_getTime(lua_State* L) {
    extern double glfwGetTime(void);
    lua_pushnumber(L, glfwGetTime());
    return 1;
}

// ============================================================================
// Lua API функции для работы с вводом
// ============================================================================

static int lua_isKeyPressed(lua_State* L) {
    if (!g_window) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    int key = (int)luaL_checknumber(L, 1);
    int state = glfwGetKey(g_window, key);
    lua_pushboolean(L, state == GLFW_PRESS || state == GLFW_REPEAT);
    return 1;
}

// Константы клавиш для Lua
static int lua_getKeyUp(lua_State* L) {
    lua_pushnumber(L, GLFW_KEY_UP);
    return 1;
}

static int lua_getKeyDown(lua_State* L) {
    lua_pushnumber(L, GLFW_KEY_DOWN);
    return 1;
}

static int lua_getKeyLeft(lua_State* L) {
    lua_pushnumber(L, GLFW_KEY_LEFT);
    return 1;
}

static int lua_getKeyRight(lua_State* L) {
    lua_pushnumber(L, GLFW_KEY_RIGHT);
    return 1;
}

static int lua_getKeyW(lua_State* L) {
    lua_pushnumber(L, GLFW_KEY_W);
    return 1;
}

static int lua_getKeyA(lua_State* L) {
    lua_pushnumber(L, GLFW_KEY_A);
    return 1;
}

static int lua_getKeyS(lua_State* L) {
    lua_pushnumber(L, GLFW_KEY_S);
    return 1;
}

static int lua_getKeyD(lua_State* L) {
    lua_pushnumber(L, GLFW_KEY_D);
    return 1;
}

// ============================================================================
// Регистрация API в Lua
// ============================================================================

void mentalLuaRegisterObjectAPI(lua_State* L, MentalLuaObject* pLuaObject) {
    if (!L || !pLuaObject) {
        return;
    }
    
    g_currentLuaObject = pLuaObject;
    
    // Регистрируем функции объекта
    lua_register(L, "setPosition", lua_object_setPosition);
    lua_register(L, "getPosition", lua_object_getPosition);
    lua_register(L, "setRotation", lua_object_setRotation);
    lua_register(L, "setScale", lua_object_setScale);
    lua_register(L, "setColor", lua_object_setColor);
    lua_register(L, "setSpeed", lua_object_setSpeed);
    lua_register(L, "setVisible", lua_object_setVisible);
    
    // Регистрируем математические функции
    lua_register(L, "sin", lua_math_sin);
    lua_register(L, "cos", lua_math_cos);
    lua_register(L, "getTime", lua_getTime);
    
    // Регистрируем функции ввода
    lua_register(L, "isKeyPressed", lua_isKeyPressed);
    
    // Регистрируем константы клавиш
    lua_register(L, "KEY_UP", lua_getKeyUp);
    lua_register(L, "KEY_DOWN", lua_getKeyDown);
    lua_register(L, "KEY_LEFT", lua_getKeyLeft);
    lua_register(L, "KEY_RIGHT", lua_getKeyRight);
    lua_register(L, "KEY_W", lua_getKeyW);
    lua_register(L, "KEY_A", lua_getKeyA);
    lua_register(L, "KEY_S", lua_getKeyS);
    lua_register(L, "KEY_D", lua_getKeyD);
}

// ============================================================================
// Создание и уничтожение Lua объектов
// ============================================================================

MentalLuaObject* mentalLuaObjectCreate(MentalObjectInfo* pInfo, const char* scriptPath) {
    if (!pInfo) {
        return NULL;
    }
    
    MentalLuaObject* pLuaObject = (MentalLuaObject*)malloc(sizeof(MentalLuaObject));
    if (!pLuaObject) {
        return NULL;
    }
    
    // Создаем базовый объект
    pLuaObject->baseObject = mentalObjectCreate(pInfo);
    if (!pLuaObject->baseObject) {
        free(pLuaObject);
        return NULL;
    }
    
    // Создаем Lua состояние
    pLuaObject->luaState = (MentalLuaObjectState*)malloc(sizeof(MentalLuaObjectState));
    if (!pLuaObject->luaState) {
        mentalObjectDestroy(pLuaObject->baseObject);
        free(pLuaObject);
        return NULL;
    }
    
    // Инициализируем Lua состояние
    pLuaObject->luaState->L = luaL_newstate();
    if (!pLuaObject->luaState->L) {
        free(pLuaObject->luaState);
        mentalObjectDestroy(pLuaObject->baseObject);
        free(pLuaObject);
        return NULL;
    }
    
    luaL_openlibs(pLuaObject->luaState->L);
    
    // Инициализируем свойства по умолчанию
    pLuaObject->luaState->position[0] = 0.0f;
    pLuaObject->luaState->position[1] = 0.0f;
    pLuaObject->luaState->position[2] = 0.0f;
    
    pLuaObject->luaState->rotation[0] = 0.0f;
    pLuaObject->luaState->rotation[1] = 0.0f;
    pLuaObject->luaState->rotation[2] = 0.0f;
    
    pLuaObject->luaState->scale[0] = 1.0f;
    pLuaObject->luaState->scale[1] = 1.0f;
    pLuaObject->luaState->scale[2] = 1.0f;
    
    pLuaObject->luaState->color[0] = 1.0f;
    pLuaObject->luaState->color[1] = 1.0f;
    pLuaObject->luaState->color[2] = 1.0f;
    pLuaObject->luaState->color[3] = 1.0f;
    
    pLuaObject->luaState->speed = MENTAL_LUA_DEFAULT_SPEED;
    pLuaObject->luaState->visible = 1;
    pLuaObject->luaState->scriptLoaded = 0;
    pLuaObject->luaState->scriptPath = NULL;
    pLuaObject->luaState->propertiesTable = NULL;
    
    // Регистрируем API
    mentalLuaRegisterObjectAPI(pLuaObject->luaState->L, pLuaObject);
    
    // Загружаем скрипт если указан
    if (scriptPath) {
        mentalLuaObjectLoadScript(pLuaObject, scriptPath);
    }
    
    return pLuaObject;
}

void mentalLuaObjectDestroy(MentalLuaObject* pLuaObject) {
    if (!pLuaObject) {
        return;
    }
    
    if (pLuaObject->luaState) {
        if (pLuaObject->luaState->L) {
            lua_close(pLuaObject->luaState->L);
        }
        if (pLuaObject->luaState->scriptPath) {
            free(pLuaObject->luaState->scriptPath);
        }
        free(pLuaObject->luaState);
    }
    
    if (pLuaObject->baseObject) {
        mentalObjectDestroy(pLuaObject->baseObject);
    }
    
    free(pLuaObject);
}

// ============================================================================
// Работа со скриптами
// ============================================================================

int mentalLuaObjectLoadScript(MentalLuaObject* pLuaObject, const char* scriptPath) {
    if (!pLuaObject || !pLuaObject->luaState || !scriptPath) {
        return 0;
    }
    
    // Сохраняем путь к скрипту
    if (pLuaObject->luaState->scriptPath) {
        free(pLuaObject->luaState->scriptPath);
    }
    
    size_t pathLen = strlen(scriptPath);
    pLuaObject->luaState->scriptPath = (char*)malloc(pathLen + 1);
    if (!pLuaObject->luaState->scriptPath) {
        return 0;
    }
    strcpy(pLuaObject->luaState->scriptPath, scriptPath);
    
    // Загружаем и выполняем скрипт
    if (luaL_dofile(pLuaObject->luaState->L, scriptPath) != LUA_OK) {
        printf("Error loading Lua script '%s': %s\n", 
               scriptPath, lua_tostring(pLuaObject->luaState->L, -1));
        lua_pop(pLuaObject->luaState->L, 1);
        return 0;
    }
    
    pLuaObject->luaState->scriptLoaded = 1;
    
    // Вызываем функцию инициализации если она есть
    mentalLuaObjectCallFunction(pLuaObject, "init");
    
    return 1;
}

int mentalLuaObjectReloadScript(MentalLuaObject* pLuaObject) {
    if (!pLuaObject || !pLuaObject->luaState || !pLuaObject->luaState->scriptPath) {
        return 0;
    }
    
    return mentalLuaObjectLoadScript(pLuaObject, pLuaObject->luaState->scriptPath);
}

int mentalLuaObjectCallFunction(MentalLuaObject* pLuaObject, const char* functionName) {
    if (!pLuaObject || !pLuaObject->luaState || !pLuaObject->luaState->scriptLoaded || !functionName) {
        return 0;
    }
    
    lua_State* L = pLuaObject->luaState->L;
    
    // Устанавливаем текущий объект для API функций
    g_currentLuaObject = pLuaObject;
    
    lua_getglobal(L, functionName);
    if (lua_isfunction(L, -1)) {
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            printf("Error calling Lua function '%s': %s\n", 
                   functionName, lua_tostring(L, -1));
            lua_pop(L, 1);
            return 0;
        }
        return 1;
    } else {
        lua_pop(L, 1);
        return 0;
    }
}

// ============================================================================
// Обновление и рендеринг
// ============================================================================

void mentalLuaObjectUpdate(MentalLuaObject* pLuaObject, float deltaTime) {
    if (!pLuaObject || !pLuaObject->luaState || !pLuaObject->luaState->scriptLoaded) {
        return;
    }
    
    lua_State* L = pLuaObject->luaState->L;
    g_currentLuaObject = pLuaObject;
    
    // Вызываем функцию update с deltaTime
    lua_getglobal(L, "update");
    if (lua_isfunction(L, -1)) {
        lua_pushnumber(L, deltaTime);
        if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
            printf("Error calling Lua update function: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    } else {
        lua_pop(L, 1);
    }
}

void mentalLuaObjectRender(MentalLuaObject* pLuaObject) {
    if (!pLuaObject || !pLuaObject->luaState->visible) {
        return;
    }
    
    // Вызываем функцию render в Lua если она есть
    if (pLuaObject->luaState->scriptLoaded) {
        g_currentLuaObject = pLuaObject;
        mentalLuaObjectCallFunction(pLuaObject, "render");
    }
    
    // Рендерим базовый объект
    mentalObjectRender(pLuaObject->baseObject);
}

// ============================================================================
// Функции для установки/получения свойств
// ============================================================================

void mentalLuaObjectSetPosition(MentalLuaObject* pLuaObject, float x, float y, float z) {
    if (!pLuaObject || !pLuaObject->luaState) {
        return;
    }
    
    pLuaObject->luaState->position[0] = x;
    pLuaObject->luaState->position[1] = y;
    pLuaObject->luaState->position[2] = z;
}

void mentalLuaObjectGetPosition(MentalLuaObject* pLuaObject, float* x, float* y, float* z) {
    if (!pLuaObject || !pLuaObject->luaState) {
        return;
    }
    
    if (x) *x = pLuaObject->luaState->position[0];
    if (y) *y = pLuaObject->luaState->position[1];
    if (z) *z = pLuaObject->luaState->position[2];
}

void mentalLuaObjectSetRotation(MentalLuaObject* pLuaObject, float x, float y, float z) {
    if (!pLuaObject || !pLuaObject->luaState) {
        return;
    }
    
    pLuaObject->luaState->rotation[0] = x;
    pLuaObject->luaState->rotation[1] = y;
    pLuaObject->luaState->rotation[2] = z;
}

void mentalLuaObjectSetScale(MentalLuaObject* pLuaObject, float x, float y, float z) {
    if (!pLuaObject || !pLuaObject->luaState) {
        return;
    }
    
    pLuaObject->luaState->scale[0] = x;
    pLuaObject->luaState->scale[1] = y;
    pLuaObject->luaState->scale[2] = z;
}

void mentalLuaObjectSetColor(MentalLuaObject* pLuaObject, float r, float g, float b, float a) {
    if (!pLuaObject || !pLuaObject->luaState) {
        return;
    }
    
    pLuaObject->luaState->color[0] = r;
    pLuaObject->luaState->color[1] = g;
    pLuaObject->luaState->color[2] = b;
    pLuaObject->luaState->color[3] = a;
}

void mentalLuaObjectSetSpeed(MentalLuaObject* pLuaObject, float speed) {
    if (!pLuaObject || !pLuaObject->luaState) {
        return;
    }
    
    pLuaObject->luaState->speed = speed;
}

void mentalLuaObjectSetVisible(MentalLuaObject* pLuaObject, int visible) {
    if (!pLuaObject || !pLuaObject->luaState) {
        return;
    }
    
    pLuaObject->luaState->visible = visible;
}

// ============================================================================
// Функции для работы с вводом
// ============================================================================

void mentalLuaInputSetWindow(GLFWwindow* window) {
    g_window = window;
}

int mentalLuaInputIsKeyPressed(int key) {
    if (!g_window) {
        return 0;
    }
    
    int state = glfwGetKey(g_window, key);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}