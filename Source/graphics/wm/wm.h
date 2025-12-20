#ifndef MENTAL_WINDOW_MANAGER_H
#define MENTAL_WINDOW_MANAGER_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct GLFWwindow GLFWwindow;
typedef struct MentalOGLRRenderPool MentalOGLRRenderPool;

/**
 * @brief Типы рендереров
 */
typedef enum MentalRenderer {
    MENTAL_RENDERER_VULKAN = 0,
    MENTAL_RENDERER_OPENGL = 1,
} MentalRenderer;

/**
 * @brief Информация для создания менеджера окон
 */
typedef struct MentalWindowManagerInfo {
    int aSizes[2];              // Размеры окна [ширина, высота]
    MentalRenderer eRenderer;   // Тип рендерера
} MentalWindowManagerInfo;

/**
 * @brief Менеджер окон Mental Engine
 */
typedef struct MentalWindowManager {
    MentalWindowManagerInfo* pInfo;
    GLFWwindow* pWindow;
} MentalWindowManager;

/**
 * @brief Создает новый менеджер окон
 * @param pInfo Информация для создания менеджера
 * @return Указатель на созданный менеджер или NULL при ошибке
 */
MentalWindowManager* mentalWindowManagerCreate(MentalWindowManagerInfo* pInfo);

/**
 * @brief Запускает главный цикл менеджера окон
 * @param pManager Менеджер окон
 */
void mentalWindowManagerRun(MentalWindowManager* pManager);

/**
 * @brief Уничтожает менеджер окон и освобождает память
 * @param pManager Менеджер окон для уничтожения
 */
void mentalWindowManagerDestroy(MentalWindowManager* pManager);

/**
 * @brief Рендерит кадр
 * @param pManager Менеджер окон
 * @param pPool Пул объектов для рендеринга
 */
void mentalWindowManagerRender(MentalWindowManager* pManager, MentalOGLRRenderPool* pPool);

#ifdef __cplusplus
}
#endif

#endif // MENTAL_WINDOW_MANAGER_H