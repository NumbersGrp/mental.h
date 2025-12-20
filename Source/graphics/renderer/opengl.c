#include "opengl.h"
#include <stdlib.h>
#include <stdio.h>

// ============================================================================
// Инициализация рендерера
// ============================================================================

void mentalRendererOpenGLInit(void) {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return;
    }
    
    printf("OpenGL Renderer initialized successfully\n");
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

// ============================================================================
// Управление пулом рендеринга
// ============================================================================

MentalOGLRRenderPool* mentalRendererOpenGLRenderPoolInit(void) {
    MentalOGLRRenderPool* pPool = (MentalOGLRRenderPool*)malloc(sizeof(MentalOGLRRenderPool));
    if (!pPool) {
        fprintf(stderr, "Failed to allocate memory for render pool\n");
        return NULL;
    }
    
    // Инициализируем все указатели как NULL
    for (int i = 0; i < MENTAL_RENDER_POOL_MAX_OBJECTS; i++) {
        pPool->pObject[i] = NULL;
    }
    
    for (int i = 0; i < MENTAL_RENDER_POOL_MAX_LUA_OBJECTS; i++) {
        pPool->pLuaObject[i] = NULL;
    }
    
    pPool->objectCount = 0;
    pPool->luaObjectCount = 0;
    
    printf("Render pool initialized (max objects: %d, max Lua objects: %d)\n", 
           MENTAL_RENDER_POOL_MAX_OBJECTS, MENTAL_RENDER_POOL_MAX_LUA_OBJECTS);
    
    return pPool;
}

void mentalRendererOpenGLRenderPoolDestroy(MentalOGLRRenderPool* pPool) {
    if (!pPool) {
        return;
    }
    
    printf("Destroying render pool...\n");
    
    // Уничтожаем обычные объекты
    for (int i = 0; i < MENTAL_RENDER_POOL_MAX_OBJECTS; i++) {
        if (pPool->pObject[i]) {
            mentalObjectDestroy(pPool->pObject[i]);
            pPool->pObject[i] = NULL;
        }
    }
    
    // Уничтожаем Lua объекты
    for (int i = 0; i < MENTAL_RENDER_POOL_MAX_LUA_OBJECTS; i++) {
        if (pPool->pLuaObject[i]) {
            mentalLuaObjectDestroy(pPool->pLuaObject[i]);
            pPool->pLuaObject[i] = NULL;
        }
    }
    
    free(pPool);
    printf("Render pool destroyed\n");
}

// ============================================================================
// Добавление объектов в пул
// ============================================================================

void mentalRendererOpenGLRenderPoolAddObject(MentalOGLRRenderPool* pPool, MentalObject* pObject) {
    if (!pPool || !pObject) {
        fprintf(stderr, "Invalid parameters for adding object to render pool\n");
        return;
    }
    
    // Ищем свободное место
    for (int i = 0; i < MENTAL_RENDER_POOL_MAX_OBJECTS; i++) {
        if (!pPool->pObject[i]) {
            pPool->pObject[i] = pObject;
            pPool->objectCount++;
            printf("Added object to render pool (slot %d, total: %d)\n", i, pPool->objectCount);
            return;
        }
    }
    
    fprintf(stderr, "Render pool is full, cannot add more objects\n");
}

void mentalRendererOpenGLRenderPoolAddLuaObject(MentalOGLRRenderPool* pPool, MentalLuaObject* pLuaObject) {
    if (!pPool || !pLuaObject) {
        fprintf(stderr, "Invalid parameters for adding Lua object to render pool\n");
        return;
    }
    
    // Ищем свободное место
    for (int i = 0; i < MENTAL_RENDER_POOL_MAX_LUA_OBJECTS; i++) {
        if (!pPool->pLuaObject[i]) {
            pPool->pLuaObject[i] = pLuaObject;
            pPool->luaObjectCount++;
            printf("Added Lua object to render pool (slot %d, total: %d)\n", i, pPool->luaObjectCount);
            return;
        }
    }
    
    fprintf(stderr, "Lua object pool is full, cannot add more Lua objects\n");
}

// ============================================================================
// Обновление и рендеринг
// ============================================================================

void mentalRendererOpenGLRenderPoolUpdate(MentalOGLRRenderPool* pPool, float deltaTime) {
    if (!pPool) {
        return;
    }
    
    // Обновляем все Lua объекты
    for (int i = 0; i < MENTAL_RENDER_POOL_MAX_LUA_OBJECTS; i++) {
        if (pPool->pLuaObject[i]) {
            mentalLuaObjectUpdate(pPool->pLuaObject[i], deltaTime);
        }
    }
}

void mentalRendererOpenGLRenderPoolRender(MentalOGLRRenderPool* pPool) {
    if (!pPool) {
        return;
    }
    
    // Рендерим обычные объекты
    for (int i = 0; i < MENTAL_RENDER_POOL_MAX_OBJECTS; i++) {
        if (pPool->pObject[i]) {
            mentalObjectRender(pPool->pObject[i]);
        }
    }
    
    // Рендерим Lua объекты
    for (int i = 0; i < MENTAL_RENDER_POOL_MAX_LUA_OBJECTS; i++) {
        if (pPool->pLuaObject[i]) {
            mentalLuaObjectRender(pPool->pLuaObject[i]);
        }
    }
}

// ============================================================================
// Вспомогательные функции для создания объектов
// ============================================================================

MentalMaterialInfo* mentalRendererCreateRainbowMaterialInfo(void) {
    MentalMaterialInfo* materialInfo = (MentalMaterialInfo*)malloc(sizeof(MentalMaterialInfo));
    if (!materialInfo) {
        fprintf(stderr, "Failed to allocate memory for material info\n");
        return NULL;
    }
    
    materialInfo->path_to_vertex = "Common/Shaders/Rainbow/vertex_shader.glsl";
    materialInfo->path_to_fragment = "Common/Shaders/Rainbow/fragment_shader.glsl";
    materialInfo->eMaterialType = MENTAL_MATERIAL_TYPE_SHADER_GLSL;
    
    return materialInfo;
}

MentalObject* mentalRendererCreateRectangle(MentalMaterialInfo* pMaterialInfo) {
    if (!pMaterialInfo) {
        fprintf(stderr, "Material info is required to create rectangle\n");
        return NULL;
    }
    
    MentalObjectInfo* objectInfo = (MentalObjectInfo*)malloc(sizeof(MentalObjectInfo));
    if (!objectInfo) {
        fprintf(stderr, "Failed to allocate memory for object info\n");
        return NULL;
    }
    
    objectInfo->eType = MENTAL_OBJECT_TYPE_RECTANGLE;
    objectInfo->pMaterialInfo = pMaterialInfo;
    
    MentalObject* object = mentalObjectCreate(objectInfo);
    if (!object) {
        fprintf(stderr, "Failed to create rectangle object\n");
        free(objectInfo);
        return NULL;
    }
    
    return object;
}