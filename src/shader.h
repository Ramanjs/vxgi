#ifndef SHADER_H
#define SHADER_H

#include "utils.h"

enum class uniformType {
    i1,
    fv3,
    fv4,
    f1,
    mat4x4,
    mat3x3,
};

class Shader {
    private:
        GLuint vs, fs, gs;
        GLuint program;

    public:
        Shader(const char* vsPath, const char* fsPath, const char* gsPath = 0);
        void use();
        void setUniform(uniformType type, void* param, char* name);

    private:
        void printLog(GLuint object);
        char* loadFile(const char* filename);
        GLuint initShader(const char* filename, GLenum type); 
        void attachAndLinkProgram();
};

#endif /* ifndef SHADER_H */
