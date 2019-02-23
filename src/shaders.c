/* loads shader from GLSL source files, compiles and links shader program */
void pufShaderLoad(PUFshader* shader, char const* vertexShaderSourceFile, char const* fragmentShaderSourceFile)
{

		int32_t vertexShaderCompileSuccess, fragmentShaderCompileSuccess, shaderLinkSuccess;

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		const GLchar* vertexShaderSource = pufReadFile(vertexShaderSourceFile);
		//printf("vertexShaderSource: %s",vertexShaderSource);
		if (vertexShaderSource != 0)
        {
            glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
            glCompileShader(vertexShader);
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexShaderCompileSuccess);
            if (vertexShaderCompileSuccess == GL_FALSE)
            {
                printf("Compilation of vertex shader source file %s failed:\n", vertexShaderSourceFile);
                int32_t maxLength = 0;
                glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
                GLchar *errorLog = (GLchar *)malloc(maxLength * sizeof(GLchar));
                glGetShaderInfoLog(vertexShader, maxLength, &maxLength, errorLog);
                printf("%s", errorLog);
                free((void *)errorLog);
            }
                free((void*)vertexShaderSource);
        }
		
		
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		
		const GLchar* fragmentShaderSource = pufReadFile(fragmentShaderSourceFile);
		//printf("fragmentShaderSource: %s",fragmentShaderSource);
        if (fragmentShaderSource != 0)
        {
            glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
            glCompileShader(fragmentShader);
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentShaderCompileSuccess);
            if (fragmentShaderCompileSuccess == GL_FALSE)
            {
                printf("Compilation of fragment shader source file %s failed:\n", fragmentShaderSourceFile);
                int32_t maxLength = 0;
                glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
                GLchar *errorLog = (GLchar *)malloc(maxLength * sizeof(GLchar));
                glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, errorLog);
                printf("%s", errorLog);
                free((void *)errorLog);
            }
            	free((void*)fragmentShaderSource);
        }

		
		if (fragmentShaderCompileSuccess && vertexShaderCompileSuccess)
		{
			shader->shaderProgram = glCreateProgram();
			glAttachShader(shader->shaderProgram,vertexShader);
			glAttachShader(shader->shaderProgram,fragmentShader);
			glLinkProgram(shader->shaderProgram);
			glGetProgramiv(shader->shaderProgram, GL_LINK_STATUS, &shaderLinkSuccess);
			if (shaderLinkSuccess == GL_FALSE)
			{
				printf("Shader program linking of %s and %s failed:", vertexShaderSourceFile, fragmentShaderSourceFile);
				int32_t maxLength = 0;
				glGetProgramiv(shader->shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
				GLchar* errorLog = (GLchar*)malloc(maxLength*sizeof(GLchar));
				glGetProgramInfoLog(shader->shaderProgram, maxLength, &maxLength, errorLog);
				printf("%s", errorLog);
				glDeleteProgram(shader->shaderProgram);
			}
		}
		
		// these aren't really needed because the unlinked shaders are on the stack but whatevs...
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		
        // zero shader uniforms
        shader->uniformTime = 0.0f;
        shader->uniformTimeDelta = 0.0f;
        shader->uniformFrame = 0.0f;
} 

void pufShaderCreate(PUFshader* shader, char const* vertexShaderSource, char const* fragmentShaderSource)
{

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	
	glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
	glCompileShader(vertexShader);
	
	glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
	glCompileShader(fragmentShader);
	
	shader->shaderProgram = glCreateProgram();
	glAttachShader(shader->shaderProgram,vertexShader);
	glAttachShader(shader->shaderProgram,fragmentShader);
	glLinkProgram(shader->shaderProgram);
	
}

#define TOKEN(...) __VA_ARGS__

#define MAKE_pufShaderUniformSet(T, T_PARAMS, T_ARGS) void pufShaderUniform##T##Set(PUFshader* shader, const char* uniformName, T_PARAMS) \
{ \
    int32_t uniformLocation = glGetUniformLocation(shader->shaderProgram, uniformName); \
    if (uniformLocation != -1) { \
        glUseProgram(shader->shaderProgram); \
        glUniform##T(uniformLocation, T_ARGS); \
        glUseProgram(0); \
    } \
    else \
    printf("Invalid shader uniform name %s\n", uniformName); \
}

//makes pufShaderUniform1fSet(PUFshader* shader, const char* uniformName, float value)
MAKE_pufShaderUniformSet(1f,float value, value)
//makes pufShaderUniform3fSet(PUFshader* shader, const char* uniformName, float v1, float v2, float v3)
MAKE_pufShaderUniformSet(3f, TOKEN(float v1, float v2, float v3), TOKEN(v1, v2, v3))