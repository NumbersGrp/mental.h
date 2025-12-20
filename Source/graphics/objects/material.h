#ifndef MENTAL_MATERIAL_H
#define MENTAL_MATERIAL_H

#include <GL/glew.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum MentalMaterialType
{
    MENTAL_MATERIAL_TYPE_SHADER_GLSL = 0,
} MentalMaterialType;

typedef struct MentalMaterialInfo {
    char* path_to_vertex;
    char* path_to_fragment;
    MentalMaterialType eMaterialType;
} MentalMaterialInfo;

typedef struct MentalMaterial {
    MentalMaterialInfo* pInfo;
    GLuint shaderProgram;
    GLint timeLocation;
    GLint modelLocation;
    GLint viewLocation;
    GLint projectionLocation;
} MentalMaterial;

// Function declarations
MentalMaterial* mentalMaterialCreate(MentalMaterialInfo* pInfo);
void mentalMaterialDestroy(MentalMaterial* pMaterial);
void mentalMaterialBind(MentalMaterial* pMaterial);
void mentalMaterialSetFloat(MentalMaterial* pMaterial, const char* name, float value);
void mentalMaterialSetInt(MentalMaterial* pMaterial, const char* name, int value);

#ifdef __cplusplus
}
#endif

#endif // MENTAL_MATERIAL_H