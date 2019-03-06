/* loads shader from GLSL source files, compiles and links shader program */
PUFshader pufShaderLoad(char const* vertexShaderSourceFile, char const* fragmentShaderSourceFile)
{
    PUFshader shader;

		GLint vertexShaderCompileSuccess = 0; 
        GLint fragmentShaderCompileSuccess = 0;
        GLint shaderLinkSuccess = 0;

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
                GLint maxLength = 0;
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
                GLint maxLength = 0;
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
			shader.shaderProgram = glCreateProgram();
			glAttachShader(shader.shaderProgram,vertexShader);
			glAttachShader(shader.shaderProgram,fragmentShader);
			glLinkProgram(shader.shaderProgram);
			glGetProgramiv(shader.shaderProgram, GL_LINK_STATUS, &shaderLinkSuccess);
			if (shaderLinkSuccess == GL_FALSE)
			{
				printf("Shader program linking of %s and %s failed:", vertexShaderSourceFile, fragmentShaderSourceFile);
				GLint maxLength = 0;
				glGetProgramiv(shader.shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
				GLchar* errorLog = (GLchar*)malloc(maxLength*sizeof(GLchar));
				glGetProgramInfoLog(shader.shaderProgram, maxLength, &maxLength, errorLog);
				printf("%s", errorLog);
				glDeleteProgram(shader.shaderProgram);
			}
		}
		
		// these aren't really needed because the unlinked shaders are on the stack but whatevs...
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		
        // zero shader uniforms
        shader.uniformTime = 0.0f;
        shader.uniformTimeDelta = 0.0f;
        shader.uniformFrame = 0.0f;

    return shader;
} 

PUFshader pufShaderCreate(char const* vertexShaderSource, char const* fragmentShaderSource)
{

    // Only ever used this for ruling out file read errors, needs error handling for serious use.
    PUFshader shader;

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	
	glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
	glCompileShader(vertexShader);
	
	glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
	glCompileShader(fragmentShader);
	
	shader.shaderProgram = glCreateProgram();
	glAttachShader(shader.shaderProgram,vertexShader);
	glAttachShader(shader.shaderProgram,fragmentShader);
	glLinkProgram(shader.shaderProgram);
	
    return shader;
}

#define TOKEN(...) __VA_ARGS__

#define MAKE_pufShaderUniformSet(T, T_PARAMS, T_ARGS) void pufShaderUniform##T##Set(PUFshader* shader, const char* uniformName, T_PARAMS) \
{ \
    GLint uniformLocation = glGetUniformLocation(shader->shaderProgram, uniformName); \
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
//makes pufShaderUniform2fSet(PUFshader* shader, const char* uniformName, float v1, float v2)
MAKE_pufShaderUniformSet(2f, TOKEN(float v1, float v2), TOKEN(v1, v2))
//makes pufShaderUniform3fSet(PUFshader* shader, const char* uniformName, float v1, float v2, float v3)
MAKE_pufShaderUniformSet(3f, TOKEN(float v1, float v2, float v3), TOKEN(v1, v2, v3))
//makes pufShaderUniform4fSet(PUFshader* shader, const char* uniformName, float v1, float v2, float v3, float v4)
MAKE_pufShaderUniformSet(4f, TOKEN(float v1, float v2, float v3, float v4), TOKEN(v1, v2, v3, v4))