#ifndef MENTAL_OBJECTS_H
#define MENTAL_OBJECTS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "material.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Типы объектов в Mental Engine
 */
typedef enum MentalObjectType {
    MENTAL_OBJECT_TYPE_ROOT_NODE = 0,
    MENTAL_OBJECT_TYPE_RECTANGLE = 1,
} MentalObjectType;

/**
 * @brief Информация для создания объекта
 */
typedef struct MentalObjectInfo {
    MentalObjectType eType;
    MentalMaterialInfo* pMaterialInfo;
} MentalObjectInfo;

/**
 * @brief Основная структура объекта Mental Engine
 */
typedef struct MentalObject {
    MentalObjectInfo* pInfo;
    const void* pNext;
    
    // OpenGL данные
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    GLuint shaderProgram;  // Deprecated: использовать material
    
    // Материал
    MentalMaterial* pMaterial;
    
    // Данные анимации
    float startTime;
} MentalObject;

/**
 * @brief Создает новый объект
 * @param pInfo Информация для создания объекта
 * @return Указатель на созданный объект или NULL при ошибке
 */
MentalObject* mentalObjectCreate(MentalObjectInfo* pInfo);

/**
 * @brief Рендерит объект
 * @param pObject Объект для рендеринга
 */
void mentalObjectRender(MentalObject* pObject);

/**
 * @brief Уничтожает объект и освобождает память
 * @param pObject Объект для уничтожения
 */
void mentalObjectDestroy(MentalObject* pObject);

#ifdef __cplusplus
}
#endif

#endif // MENTAL_OBJECTS_H