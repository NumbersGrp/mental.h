/**
 * @file example_usage.c
 * @brief Примеры использования системы объектов и материалов Mental Engine
 */

#include "graphics/renderer/opengl.h"
#include "graphics/objects/material.h"
#include "graphics/objects/objects.h"
#include "graphics/objects/lua_object.h"
#include <stdio.h>
#include <stdlib.h>

// ============================================================================
// Создание обычных объектов
// ============================================================================

/**
 * @brief Создает пользовательские объекты (расширенный способ)
 * @param pPool Пул рендеринга для добавления объектов
 */
void createUserObjects(MentalOGLRRenderPool* pPool) {
    if (!pPool) {
        fprintf(stderr, "Render pool is NULL\n");
        return;
    }
    
    printf("Creating user objects (advanced method)...\n");
    
    // 1. Создание информации о материале
    MentalMaterialInfo* materialInfo = (MentalMaterialInfo*)malloc(sizeof(MentalMaterialInfo));
    if (!materialInfo) {
        fprintf(stderr, "Failed to allocate memory for material info\n");
        return;
    }
    
    materialInfo->path_to_vertex = "Common/Shaders/Rainbow/vertex_shader.glsl";
    materialInfo->path_to_fragment = "Common/Shaders/Rainbow/fragment_shader.glsl";
    materialInfo->eMaterialType = MENTAL_MATERIAL_TYPE_SHADER_GLSL;
    
    // 2. Создание объекта (материал создастся автоматически)
    MentalObjectInfo* objectInfo = (MentalObjectInfo*)malloc(sizeof(MentalObjectInfo));
    if (!objectInfo) {
        fprintf(stderr, "Failed to allocate memory for object info\n");
        free(materialInfo);
        return;
    }
    
    objectInfo->eType = MENTAL_OBJECT_TYPE_RECTANGLE;
    objectInfo->pMaterialInfo = materialInfo;  // Передаем информацию о материале
    
    MentalObject* object = mentalObjectCreate(objectInfo);
    if (object) {
        // 3. Добавление объекта в RenderPool
        mentalRendererOpenGLRenderPoolAddObject(pPool, object);
        printf("User object created with auto-generated material and added to render pool\n");
    } else {
        fprintf(stderr, "Failed to create user object\n");
        free(objectInfo);
        free(materialInfo);
    }
}

/**
 * @brief Создает пользовательские объекты (простой способ)
 * @param pPool Пул рендеринга для добавления объектов
 */
void createUserObjectsSimple(MentalOGLRRenderPool* pPool) {
    if (!pPool) {
        fprintf(stderr, "Render pool is NULL\n");
        return;
    }
    
    printf("Creating user objects (simple method)...\n");
    
    // 1. Создание информации о материале через helper функцию
    MentalMaterialInfo* materialInfo = mentalRendererCreateRainbowMaterialInfo();
    if (!materialInfo) {
        fprintf(stderr, "Failed to create rainbow material info\n");
        return;
    }
    
    // 2. Создание объекта через helper функцию (материал создастся автоматически)
    MentalObject* object = mentalRendererCreateRectangle(materialInfo);
    if (object) {
        // 3. Добавление в RenderPool
        mentalRendererOpenGLRenderPoolAddObject(pPool, object);
        printf("Simple user object created with auto-generated material and added to render pool\n");
    } else {
        fprintf(stderr, "Failed to create simple user object\n");
        free(materialInfo);
    }
}

// ============================================================================
// Создание Lua объектов
// ============================================================================

/**
 * @brief Создает объекты с Lua скриптами
 * @param pPool Пул рендеринга для добавления объектов
 */
void createLuaObjects(MentalOGLRRenderPool* pPool) {
    if (!pPool) {
        fprintf(stderr, "Render pool is NULL\n");
        return;
    }
    
    printf("Creating Lua-controlled objects...\n");
    
    // 1. Создание объекта с движущимся скриптом
    MentalMaterialInfo* materialInfo1 = mentalRendererCreateRainbowMaterialInfo();
    if (materialInfo1) {
        MentalObjectInfo* objectInfo1 = (MentalObjectInfo*)malloc(sizeof(MentalObjectInfo));
        if (objectInfo1) {
            objectInfo1->eType = MENTAL_OBJECT_TYPE_RECTANGLE;
            objectInfo1->pMaterialInfo = materialInfo1;
            
            // Создаем Lua объект с скриптом движения
            MentalLuaObject* luaObject1 = mentalLuaObjectCreate(objectInfo1, "Common/Scripts/movement.lua");
            if (luaObject1) {
                mentalRendererOpenGLRenderPoolAddLuaObject(pPool, luaObject1);
                printf("Lua object with movement script created\n");
            } else {
                fprintf(stderr, "Failed to create Lua object with movement script\n");
                free(objectInfo1);
                free(materialInfo1);
            }
        } else {
            free(materialInfo1);
        }
    }
    
    // 2. Создание объекта с пульсирующим скриптом
    MentalMaterialInfo* materialInfo2 = mentalRendererCreateRainbowMaterialInfo();
    if (materialInfo2) {
        MentalObjectInfo* objectInfo2 = (MentalObjectInfo*)malloc(sizeof(MentalObjectInfo));
        if (objectInfo2) {
            objectInfo2->eType = MENTAL_OBJECT_TYPE_RECTANGLE;
            objectInfo2->pMaterialInfo = materialInfo2;
            
            // Создаем Lua объект с скриптом пульсации
            MentalLuaObject* luaObject2 = mentalLuaObjectCreate(objectInfo2, "Common/Scripts/pulse.lua");
            if (luaObject2) {
                // Устанавливаем начальную позицию для второго объекта
                mentalLuaObjectSetPosition(luaObject2, 1.0f, 0.0f, 0.0f);
                mentalRendererOpenGLRenderPoolAddLuaObject(pPool, luaObject2);
                printf("Lua object with pulse script created\n");
            } else {
                fprintf(stderr, "Failed to create Lua object with pulse script\n");
                free(objectInfo2);
                free(materialInfo2);
            }
        } else {
            free(materialInfo2);
        }
    }
    
    printf("Lua objects creation completed\n");
}

// ============================================================================
// Создание объектов с управлением клавиатурой
// ============================================================================

/**
 * @brief Создает объекты с управлением клавиатурой
 * @param pPool Пул рендеринга для добавления объектов
 */
void createKeyboardControlledObjects(MentalOGLRRenderPool* pPool) {
    if (!pPool) {
        fprintf(stderr, "Render pool is NULL\n");
        return;
    }
    
    printf("Creating keyboard-controlled objects...\n");
    
    // 1. Объект с простым управлением клавиатурой
    MentalMaterialInfo* materialInfo1 = mentalRendererCreateRainbowMaterialInfo();
    if (materialInfo1) {
        MentalObjectInfo* objectInfo1 = (MentalObjectInfo*)malloc(sizeof(MentalObjectInfo));
        if (objectInfo1) {
            objectInfo1->eType = MENTAL_OBJECT_TYPE_RECTANGLE;
            objectInfo1->pMaterialInfo = materialInfo1;
            
            MentalLuaObject* keyboardObject = mentalLuaObjectCreate(objectInfo1, "Common/Scripts/keyboard_control.lua");
            if (keyboardObject) {
                mentalRendererOpenGLRenderPoolAddLuaObject(pPool, keyboardObject);
                printf("Keyboard-controlled object created (use arrow keys or WASD)\n");
            } else {
                fprintf(stderr, "Failed to create keyboard-controlled object\n");
                free(objectInfo1);
                free(materialInfo1);
            }
        } else {
            free(materialInfo1);
        }
    }
    
    // 2. Объект с плавным управлением и физикой
    MentalMaterialInfo* materialInfo2 = mentalRendererCreateRainbowMaterialInfo();
    if (materialInfo2) {
        MentalObjectInfo* objectInfo2 = (MentalObjectInfo*)malloc(sizeof(MentalObjectInfo));
        if (objectInfo2) {
            objectInfo2->eType = MENTAL_OBJECT_TYPE_RECTANGLE;
            objectInfo2->pMaterialInfo = materialInfo2;
            
            MentalLuaObject* smoothObject = mentalLuaObjectCreate(objectInfo2, "Common/Scripts/smooth_control.lua");
            if (smoothObject) {
                // Размещаем справа от первого объекта
                mentalLuaObjectSetPosition(smoothObject, 2.0f, 0.0f, 0.0f);
                mentalRendererOpenGLRenderPoolAddLuaObject(pPool, smoothObject);
                printf("Smooth-controlled object created (use arrow keys)\n");
            } else {
                fprintf(stderr, "Failed to create smooth-controlled object\n");
                free(objectInfo2);
                free(materialInfo2);
            }
        } else {
            free(materialInfo2);
        }
    }
    
    printf("Keyboard-controlled objects creation completed\n");
}

/**
 * @brief Создает демонстрационный объект с управлением клавиатурой
 * @param pPool Пул рендеринга для добавления объектов
 */
void createDemoKeyboardObject(MentalOGLRRenderPool* pPool) {
    if (!pPool) {
        fprintf(stderr, "Render pool is NULL\n");
        return;
    }
    
    printf("Creating demo keyboard object...\n");
    
    MentalMaterialInfo* materialInfo = mentalRendererCreateRainbowMaterialInfo();
    if (materialInfo) {
        MentalObjectInfo* objectInfo = (MentalObjectInfo*)malloc(sizeof(MentalObjectInfo));
        if (objectInfo) {
            objectInfo->eType = MENTAL_OBJECT_TYPE_RECTANGLE;
            objectInfo->pMaterialInfo = materialInfo;
            
            MentalLuaObject* demoObject = mentalLuaObjectCreate(objectInfo, "Common/Scripts/demo_keyboard.lua");
            if (demoObject) {
                mentalRendererOpenGLRenderPoolAddLuaObject(pPool, demoObject);
                printf("Demo keyboard object created successfully\n");
                printf("Controls: Arrow keys or WASD to move, colors indicate direction\n");
            } else {
                fprintf(stderr, "Failed to create demo keyboard object\n");
                free(objectInfo);
                free(materialInfo);
            }
        } else {
            free(materialInfo);
        }
    }
}

// ============================================================================
// Функция для обновления Lua объектов (для совместимости)
// ============================================================================

/**
 * @brief Обновляет Lua объекты в пуле (вызывается в главном цикле)
 * @param pPool Пул рендеринга
 * @param deltaTime Время с предыдущего кадра
 */
void updateLuaObjects(MentalOGLRRenderPool* pPool, float deltaTime) {
    if (!pPool) {
        return;
    }
    
    // Эта функция теперь реализована в renderer
    mentalRendererOpenGLRenderPoolUpdate(pPool, deltaTime);
}