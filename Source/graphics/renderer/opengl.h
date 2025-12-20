#ifndef MENTAL_RENDERER_OPENGL_H
#define MENTAL_RENDERER_OPENGL_H

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include "../objects/objects.h"
#include "../objects/lua_object.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Константы
// ============================================================================

#define MENTAL_RENDER_POOL_MAX_OBJECTS 10
#define MENTAL_RENDER_POOL_MAX_LUA_OBJECTS 10

// ============================================================================
// Структуры
// ============================================================================

/**
 * @brief Пул объектов для рендеринга OpenGL
 */
typedef struct MentalOGLRRenderPool {
    MentalObject* pObject[MENTAL_RENDER_POOL_MAX_OBJECTS];
    MentalLuaObject* pLuaObject[MENTAL_RENDER_POOL_MAX_LUA_OBJECTS];
    int objectCount;
    int luaObjectCount;
} MentalOGLRRenderPool;

// ============================================================================
// Основные функции рендерера
// ============================================================================

/**
 * @brief Инициализирует OpenGL рендерер
 */
void mentalRendererOpenGLInit(void);

/**
 * @brief Создает новый пул рендеринга
 * @return Указатель на созданный пул или NULL при ошибке
 */
MentalOGLRRenderPool* mentalRendererOpenGLRenderPoolInit(void);

/**
 * @brief Уничтожает пул рендеринга и освобождает память
 * @param pPool Пул для уничтожения
 */
void mentalRendererOpenGLRenderPoolDestroy(MentalOGLRRenderPool* pPool);

// ============================================================================
// Функции управления объектами в пуле
// ============================================================================

/**
 * @brief Добавляет обычный объект в пул рендеринга
 * @param pPool Пул рендеринга
 * @param pObject Объект для добавления
 */
void mentalRendererOpenGLRenderPoolAddObject(MentalOGLRRenderPool* pPool, MentalObject* pObject);

/**
 * @brief Добавляет Lua объект в пул рендеринга
 * @param pPool Пул рендеринга
 * @param pLuaObject Lua объект для добавления
 */
void mentalRendererOpenGLRenderPoolAddLuaObject(MentalOGLRRenderPool* pPool, MentalLuaObject* pLuaObject);

/**
 * @brief Обновляет все объекты в пуле
 * @param pPool Пул рендеринга
 * @param deltaTime Время с предыдущего кадра
 */
void mentalRendererOpenGLRenderPoolUpdate(MentalOGLRRenderPool* pPool, float deltaTime);

/**
 * @brief Рендерит все объекты в пуле
 * @param pPool Пул рендеринга
 */
void mentalRendererOpenGLRenderPoolRender(MentalOGLRRenderPool* pPool);

// ============================================================================
// Вспомогательные функции для создания объектов
// ============================================================================

/**
 * @brief Создает информацию о материале Rainbow
 * @return Указатель на созданную информацию о материале или NULL при ошибке
 */
MentalMaterialInfo* mentalRendererCreateRainbowMaterialInfo(void);

/**
 * @brief Создает прямоугольный объект с указанным материалом
 * @param pMaterialInfo Информация о материале
 * @return Указатель на созданный объект или NULL при ошибке
 */
MentalObject* mentalRendererCreateRectangle(MentalMaterialInfo* pMaterialInfo);

#ifdef __cplusplus
}
#endif

#endif // MENTAL_RENDERER_OPENGL_H