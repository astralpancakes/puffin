#define TOKEN(...) __VA_ARGS__

#define MAKE_pufShaderUniformSet(T, T_PARAMS, T_ARGS) void pufShaderUniform##T##Set(PUFshader* shader, const char* uniformName, T_PARAMS) \
{ \
    GLint uniformLocation = glGetUniformLocation(shader->shaderProgram, uniformName); \
    if (uniformLocation != -1) { \
        glUseProgram(shader->shaderProgram); \
        glUniform##T##(uniformLocation, T_ARGS); \
        glUseProgram(0); \
    } \
    else \
    printf("Invalid shader uniform name %s\n", uniformName); \
}

//makes pufShaderUniform1fSet(PUFshader* shader, const char* uniformName, float value)
MAKE_pufShaderUniformSet(1f,float value, value)
//makes pufShaderUniform3fSet(PUFshader* shader, const char* uniformName, float v1, float v2, float v3)
MAKE_pufShaderUniformSet(3f, TOKEN(float v1, float v2, float v3), TOKEN(v1, v2, v3))