#include "material.h"
#include "../renderer/shader_loader.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

MentalMaterial* mentalMaterialCreate(MentalMaterialInfo* pInfo) {
    if (!pInfo) return NULL;
    
    MentalMaterial* pMaterial = (MentalMaterial*)malloc(sizeof(MentalMaterial));
    if (!pMaterial) return NULL;
    
    pMaterial->pInfo = pInfo;
    pMaterial->shaderProgram = 0;
    pMaterial->timeLocation = -1;
    pMaterial->modelLocation = -1;
    pMaterial->viewLocation = -1;
    pMaterial->projectionLocation = -1;
    
    if (pInfo->eMaterialType == MENTAL_MATERIAL_TYPE_SHADER_GLSL) {
        if (pInfo->path_to_vertex && pInfo->path_to_fragment) {
            pMaterial->shaderProgram = createShaderProgramFromFiles(
                pInfo->path_to_vertex,
                pInfo->path_to_fragment
            );
            
            if (pMaterial->shaderProgram) {
                // Cache common uniform locations
                pMaterial->timeLocation = glGetUniformLocation(pMaterial->shaderProgram, "time");
                pMaterial->modelLocation = glGetUniformLocation(pMaterial->shaderProgram, "model");
                pMaterial->viewLocation = glGetUniformLocation(pMaterial->shaderProgram, "view");
                pMaterial->projectionLocation = glGetUniformLocation(pMaterial->shaderProgram, "projection");
                
                printf("Material created successfully with shader program ID: %u\n", pMaterial->shaderProgram);
            } else {
                printf("Failed to create shader program for material\n");
                free(pMaterial);
                return NULL;
            }
        } else {
            printf("Invalid shader paths provided\n");
            free(pMaterial);
            return NULL;
        }
    }
    
    return pMaterial;
}

void mentalMaterialDestroy(MentalMaterial* pMaterial) {
    if (pMaterial) {
        if (pMaterial->shaderProgram) {
            glDeleteProgram(pMaterial->shaderProgram);
        }
        free(pMaterial);
    }
}

void mentalMaterialBind(MentalMaterial* pMaterial) {
    if (pMaterial && pMaterial->shaderProgram) {
        glUseProgram(pMaterial->shaderProgram);
    }
}

void mentalMaterialSetFloat(MentalMaterial* pMaterial, const char* name, float value) {
    if (!pMaterial || !pMaterial->shaderProgram || !name) return;
    
    GLint location = glGetUniformLocation(pMaterial->shaderProgram, name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void mentalMaterialSetInt(MentalMaterial* pMaterial, const char* name, int value) {
    if (!pMaterial || !pMaterial->shaderProgram || !name) return;
    
    GLint location = glGetUniformLocation(pMaterial->shaderProgram, name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}