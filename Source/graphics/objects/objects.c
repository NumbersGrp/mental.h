#include "objects.h"
#include "material.h"
#include <stdlib.h>
#include <GL/glew.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

MentalObject* mentalObjectCreate(MentalObjectInfo* pInfo) {
    MentalObject* pObject = (MentalObject*)malloc(sizeof(MentalObject));
    if (!pObject) return NULL;
    pObject->pInfo = pInfo;
    pObject->pNext = NULL;
    pObject->pMaterial = NULL;
    
    // Создаем материал из информации о материале
    if (pInfo->pMaterialInfo) {
        pObject->pMaterial = mentalMaterialCreate(pInfo->pMaterialInfo);
        if (!pObject->pMaterial) {
            printf("Failed to create material for object\n");
            free(pObject);
            return NULL;
        }
    }
    
    if (pInfo->eType == MENTAL_OBJECT_TYPE_RECTANGLE) {
        // Create rectangle vertices (position + color)
        float vertices[] = {
            // positions        // colors
            -0.5f, -0.5f,      1.0f, 0.0f, 0.0f,  // bottom left - red
             0.5f, -0.5f,      0.0f, 1.0f, 0.0f,  // bottom right - green
             0.5f,  0.5f,      0.0f, 0.0f, 1.0f,  // top right - blue
            -0.5f,  0.5f,      1.0f, 1.0f, 0.0f   // top left - yellow
        };
        
        unsigned int indices[] = {
            0, 1, 2,  // first triangle
            2, 3, 0   // second triangle
        };
        
        // Generate and bind VAO
        GLuint VAO, VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        
        // Position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        glBindVertexArray(0);
        
        // Store OpenGL data in the object
        pObject->VAO = VAO;
        pObject->VBO = VBO;
        pObject->EBO = EBO;
        pObject->shaderProgram = pObject->pMaterial ? pObject->pMaterial->shaderProgram : 0;
        pObject->startTime = 0.0f; // Initialize animation time
    }
    
    return pObject;
}

void mentalObjectDestroy(MentalObject* pObject) {
    if (pObject) {
        if (pObject->pInfo && pObject->pInfo->eType == MENTAL_OBJECT_TYPE_RECTANGLE) {
            glDeleteVertexArrays(1, &pObject->VAO);
            glDeleteBuffers(1, &pObject->VBO);
            glDeleteBuffers(1, &pObject->EBO);
        }
        
        // Удаляем материал, созданный в mentalObjectCreate
        if (pObject->pMaterial) {
            mentalMaterialDestroy(pObject->pMaterial);
        }
        
        free(pObject);
    }
}

void mentalObjectRender(MentalObject* pObject) {
    if (!pObject || !pObject->pInfo) return;
    
    if (pObject->pInfo->eType == MENTAL_OBJECT_TYPE_RECTANGLE) {
        if (pObject->pMaterial) {
            // Use material system
            mentalMaterialBind(pObject->pMaterial);
            
            // Set time uniform for animation
            static float startTime = -1.0f;
            if (startTime < 0.0f) {
                startTime = (float)glfwGetTime();
            }
            float currentTime = (float)glfwGetTime() - startTime;
            
            if (pObject->pMaterial->timeLocation != -1) {
                glUniform1f(pObject->pMaterial->timeLocation, currentTime);
            }
        } else if (pObject->shaderProgram) {
            // Fallback to old shader system
            glUseProgram(pObject->shaderProgram);
            
            static float startTime = -1.0f;
            if (startTime < 0.0f) {
                startTime = (float)glfwGetTime();
            }
            float currentTime = (float)glfwGetTime() - startTime;
            
            GLint timeLocation = glGetUniformLocation(pObject->shaderProgram, "time");
            if (timeLocation != -1) {
                glUniform1f(timeLocation, currentTime);
            }
        }
        
        glBindVertexArray(pObject->VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}
