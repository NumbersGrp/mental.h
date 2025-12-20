#include "wm.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "../renderer/opengl.h"
#include "../objects/lua_object.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Константы
// ============================================================================

static const float CLEAR_COLOR_R = 0.2F;
static const float CLEAR_COLOR_G = 0.3F;
static const float CLEAR_COLOR_B = 0.3F;
static const float CLEAR_COLOR_A = 1.0F;

// ============================================================================
// Создание и уничтожение менеджера окон
// ============================================================================

MentalWindowManager* mentalWindowManagerCreate(MentalWindowManagerInfo* pInfo) {
    if (!pInfo) {
        return NULL;
    }
    
    MentalWindowManager* pManager = (MentalWindowManager*)malloc(sizeof(MentalWindowManager));
    if (!pManager) {
        return NULL;
    }
    
    pManager->pInfo = pInfo;

    // Инициализация GLFW
    if (!glfwInit()) {
        free(pManager);
        return NULL;
    }

    // Настройка контекста в зависимости от рендерера
    if (pInfo->eRenderer == MENTAL_RENDERER_VULKAN) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    } else if (pInfo->eRenderer == MENTAL_RENDERER_OPENGL) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    }
    
    // Создание окна
    pManager->pWindow = glfwCreateWindow(
        pInfo->aSizes[0], 
        pInfo->aSizes[1], 
        "Mental Engine", 
        NULL, 
        NULL
    );
    
    if (!pManager->pWindow) {
        glfwTerminate();
        free(pManager);
        return NULL;
    }
    
    glfwMakeContextCurrent(pManager->pWindow);
    
    // Инициализация рендерера
    if (pInfo->eRenderer == MENTAL_RENDERER_OPENGL) {
        mentalRendererOpenGLInit();
    }
    
    // Устанавливаем окно для Lua системы ввода
    mentalLuaInputSetWindow(pManager->pWindow);

    return pManager;
}

void mentalWindowManagerDestroy(MentalWindowManager* pManager) {
    if (!pManager) {
        return;
    }
    
    if (pManager->pWindow) {
        glfwDestroyWindow(pManager->pWindow);
    }
    
    glfwTerminate();
    free(pManager);
}

// ============================================================================
// Рендеринг
// ============================================================================

void mentalWindowManagerRender(MentalWindowManager* pManager, MentalOGLRRenderPool* pPool) {
    if (!pManager || !pManager->pWindow) {
        return;
    }
    
    // Очистка буфера
    glClearColor(CLEAR_COLOR_R, CLEAR_COLOR_G, CLEAR_COLOR_B, CLEAR_COLOR_A);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Рендеринг объектов
    if (pPool) {
        mentalRendererOpenGLRenderPoolRender(pPool);
    }
    
    // Обмен буферов
    glfwSwapBuffers(pManager->pWindow);
}

// ============================================================================
// Главный цикл
// ============================================================================

void mentalWindowManagerRun(MentalWindowManager* pManager) {
    if (!pManager || !pManager->pWindow) {
        return;
    }
    
    // Создание пула рендеринга
    MentalOGLRRenderPool* pPool = mentalRendererOpenGLRenderPoolInit();
    if (!pPool) {
        printf("Failed to initialize render pool\n");
        return;
    }

    // Создание тестового объекта
    MentalMaterialInfo rectangleMaterialInfo = {
        .eMaterialType = MENTAL_MATERIAL_TYPE_SHADER_GLSL,
        .path_to_vertex = "Common/Shaders/Rainbow/vertex_shader.glsl",
        .path_to_fragment = "Common/Shaders/Rainbow/fragment_shader.glsl",
    };
    
    MentalObjectInfo rectangleInfo = {
        .eType = MENTAL_OBJECT_TYPE_RECTANGLE,
        .pMaterialInfo = &rectangleMaterialInfo,
    };
    
    MentalObject* rectangle = mentalObjectCreate(&rectangleInfo);
    if (rectangle) {
        mentalRendererOpenGLRenderPoolAddObject(pPool, rectangle);
    }

    // Переменные для расчета deltaTime
    double lastTime = glfwGetTime();
    
    // Главный цикл
    while (!glfwWindowShouldClose(pManager->pWindow)) {
        // Расчет deltaTime
        double currentTime = glfwGetTime();
        float deltaTime = (float)(currentTime - lastTime);
        lastTime = currentTime;
        
        // Обработка событий
        glfwPollEvents();
        
        // Обновление Lua объектов
        mentalRendererOpenGLRenderPoolUpdate(pPool, deltaTime);
        
        // Рендеринг
        mentalWindowManagerRender(pManager, pPool);
    }
    
    // Очистка ресурсов
    mentalRendererOpenGLRenderPoolDestroy(pPool);
}

#ifdef __cplusplus
}
#endif