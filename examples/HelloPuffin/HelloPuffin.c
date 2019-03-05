#include "../../src/puffin.h"
#include <GLFW/glfw3.h>

#include <stdio.h>

GLFWwindow* window;

PUFcamera camera;

PUFmesh objectMesh;
PUFshader objectShader;
PUFtexture objectTexture;

PUFmesh framebufferMesh;
PUFshader framebufferShader;
PUFtexture framebufferTexture;
PUFframebuffer framebuffer;

int isPaused = 0;

int width = 640;
int height = 360;



void setup() // setup() is run once at program start
{
    printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));

	camera = pufCameraInit(50.0f, 0.1f, 100.0f);
	
    // from a .obj file, load a 3D object we can render and create a quad for displaying the framebuffer texture we will render to
	objectMesh = pufMeshLoadOBJ("../../mesh/solid.obj");
	framebufferMesh = pufMeshShapeQuad();
	
    // bind both meshes, ie send them to the GPU
    pufMeshBind(&objectMesh);
	pufMeshBind(&framebufferMesh);
	
    // move and rotate our object a bit
	pufMeshTranslate(&objectMesh,0.0f,0.0f,-1.0f);
	pufMeshRotateEuler(&objectMesh,0.0f,0.0f,-55.0f,DEGREES);

    // load a .bmp file for texturing our object and create an empty texture for rendering the framebuffer
    pufTextureLoadBMP(&objectTexture, "../../texture/puffin.bmp");
	pufTextureCreate(&framebufferTexture,width,height);
	
    // load some shader code for our object and framebuffer
	pufShaderLoad(&objectShader, "../../shader/vsMeshBasic.glsl", "../../shader/fsMeshTextureLitDiffuse.glsl");
	pufShaderLoad(&framebufferShader, "../../shader/vsFrameBasic.glsl", "../../shader/fsFrameFilmic.glsl");

    // set some shader parameters
    pufShaderUniform4fSet(&objectShader, "baseColor", 1.0f, 1.0f, 1.0f, 1.0f);
    pufShaderUniform1fSet(&objectShader, "lightMultiplier", 0.2f);
    pufShaderUniform3fSet(&objectShader, "lightColor", 0.9f, 0.6f, 0.0f);

    // init our framebuffer and give it the texture we want it to render to
	pufFramebufferInit(&framebuffer);
	pufFramebufferTexture(&framebuffer,&framebufferTexture);

    // set the clear color white
	glClearColor(1,1,1,1); 

}

void draw() // draw() is run every frame
{
    // make object texture active and draw the object to our framebuffer
    pufTextureBind(&objectTexture);
    pufMeshRender(&objectMesh,&camera,&objectShader, &framebuffer);    
    
    // make framebuffer texture active
    pufTextureBind(&framebufferTexture);

    // update framebuffer shader time variable
    framebufferShader.uniformTime = glfwGetTime();

    // draw our framebuffer texture to the screen
    pufMeshRender(&framebufferMesh,&camera,&framebufferShader, NULL);
    
    // rotate the object for the next frame
    pufMeshRotate(&objectMesh, -0.5,1.0,0.0,0.0, DEGREES);
}

void handleErrors(int error, const char* description)
{
    fprintf(stderr, "error: %s\n", description);
}

void handleKeys(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(int argc, char** argv)
{
    glfwSetErrorCallback(handleErrors);

    if (glfwInit())
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        window = glfwCreateWindow(width, height, "Hello Puffin!", NULL, NULL);

        if (window)
        {
            glfwMakeContextCurrent(window);
            glfwSwapInterval(1);
            glfwSetKeyCallback(window, handleKeys);
            
            #ifdef _WIN32
            glewInit();
            #endif

            setup();

            while (!glfwWindowShouldClose(window))
            {
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
        }
        else printf("Failed to create GLFW window");
        glfwTerminate();
    }
    else printf("Failed to initialize GLFW");


}