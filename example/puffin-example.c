#include "../src/puffin.h"
#include <GLFW/glfw3.h>

#include <stdio.h>

GLFWwindow* window;

PUFcamera camera;

PUFmesh cubeMesh;
PUFshader cubeShader;
PUFtexture cubeTexture;

PUFshader shader;

PUFmesh framebufferMesh;
PUFshader framebufferShader;
PUFtexture framebufferTexture;
PUFframebuffer framebuffer;

int isPaused = 0;


int width = 640;
int height = 360;

/*
void MessageCallback( GLenum source,
                      GLenum type,
                      GLuint id,
                      GLenum severity,
                      GLsizei length,
                      const GLchar* message,
                      const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}
*/

void setup()
{
printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));

	// During init, enable debug output
	//glEnable              ( GL_DEBUG_OUTPUT );
	//glDebugMessageCallback( (GLDEBUGPROC) MessageCallback, 0 );


	pufCameraInit(&camera, 50.0f, 0.1f, 100.0f);
	
	pufMeshInit(&cubeMesh);
	pufMeshInit(&framebufferMesh);
	pufMeshLoadOBJ(&cubeMesh, "../mesh/solid.obj");
	pufMeshShapeQuad(&framebufferMesh);
	pufMeshBind(&cubeMesh);
	pufMeshBind(&framebufferMesh);
	pufTextureLoadBMP(&cubeTexture, "../texture/puffin.bmp");
	pufTextureCreate(&framebufferTexture,width,height);
	
	pufMeshTranslate(&cubeMesh,0.0f,0.0f,-1.0f);
	pufMeshRotateEuler(&cubeMesh,0.0f,0.0f,-55.0f,DEGREES);

	pufShaderLoad(&cubeShader, "../shader/vsMeshBasic.glsl", "../shader/fsMeshTexturedLit.glsl");
	pufShaderLoad(&framebufferShader, "../shader/vsFrameBasic.glsl", "../shader/fsFrameFilmic.glsl");
	
	pufFramebufferInit(&framebuffer);
	pufFramebufferTexture(&framebuffer,&framebufferTexture);

	glClearColor(1,1,1,1); // Set the clear color white

}

void draw()
{

        pufShaderUniform1fSet(&cubeShader, "lightMultiplier", 0.2f);
        pufShaderUniform3fSet(&cubeShader, "lightColor", 0.9f, 0.6f, 0.0f);
        
        pufTextureBind(&cubeTexture);
	    
	    pufMeshRender(&cubeMesh,&camera,&cubeShader, &framebuffer);
	    
        pufTextureBind(&framebufferTexture);

        framebufferShader.uniformTime = glfwGetTime();    
        pufMeshRender(&framebufferMesh,&camera,&framebufferShader, NULL);
        pufMeshRotate(&cubeMesh, -0.5,1.0,0.0,0.0, DEGREES);

}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(int argc, char** argv)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {/* error */}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(width, height, "My Title", NULL, NULL);

    if (!window)
    {/* error */}
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);
    
    #ifdef _WIN32
    glewInit();
    #endif


    setup();

    while (!glfwWindowShouldClose(window))
    {
        glfwGetFramebufferSize(window, &width, &height);
    
        draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
		
	    // check OpenGL error
	    GLenum err;
	    while ((err = glGetError()) != GL_NO_ERROR) {
	        printf("OpenGL error: %d\n", err);
	    }
    }

    glfwDestroyWindow(window);
    glfwTerminate();


}