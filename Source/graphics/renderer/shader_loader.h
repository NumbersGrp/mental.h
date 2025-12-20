#ifndef MENTAL_SHADER_LOADER_H
#define MENTAL_SHADER_LOADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>

#ifdef __cplusplus
extern "C" {
#endif

// Функция для чтения файла шейдера
static char* loadShaderSource(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "Failed to open shader file: %s\n", filepath);
        return NULL;
    }
    
    // Определяем размер файла
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Выделяем память и читаем файл
    char* source = (char*)malloc(length + 1);
    if (!source) {
        fclose(file);
        return NULL;
    }
    
    fread(source, 1, length, file);
    source[length] = '\0';
    fclose(file);
    
    return source;
}

// Компиляция шейдера
static GLuint compileShaderFromFile(GLenum type, const char* filepath) {
    char* source = loadShaderSource(filepath);
    if (!source) {
        return 0;
    }
    
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, (const char**)&source, NULL);
    glCompileShader(shader);
    
    // Проверка ошибок компиляции
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Shader compilation failed (%s): %s\n", filepath, infoLog);
        glDeleteShader(shader);
        free(source);
        return 0;
    }
    
    free(source);
    return shader;
}

// Создание программы шейдеров из файлов
static GLuint createShaderProgramFromFiles(const char* vertexPath, const char* fragmentPath) {
    GLuint vertexShader = compileShaderFromFile(GL_VERTEX_SHADER, vertexPath);
    GLuint fragmentShader = compileShaderFromFile(GL_FRAGMENT_SHADER, fragmentPath);
    
    if (!vertexShader || !fragmentShader) {
        if (vertexShader) glDeleteShader(vertexShader);
        if (fragmentShader) glDeleteShader(fragmentShader);
        return 0;
    }
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    // Проверка ошибок линковки
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        fprintf(stderr, "Shader program linking failed: %s\n", infoLog);
        glDeleteProgram(program);
        program = 0;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}

#ifdef __cplusplus
}
#endif

#endif // MENTAL_SHADER_LOADER_H