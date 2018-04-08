#include "../src/puffin.h"
#include <GLFW/glfw3.h>

#include <stdio.h>

GLFWwindow* window;

PUFcamera camera;

PUFmesh cubeMesh;
PUFshader cubeShader;
PUFtexture cubeTexture;

PUFmesh framebufferMesh;
PUFshader framebufferShader;
PUFtexture framebufferTexture;
PUFframebuffer framebuffer;

int isPaused = 0;


int width = 1280   ;
int height = 720;

void draw_immidiate_triangle()
{


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(-2.0,2.0,-2.0,2.0,0.1,10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0f,0.0f, -6.0f);

    glBegin(GL_TRIANGLES);
        glColor3f(1.0f,1.0f,1.0f);
        glVertex3f(0.0f,1.0f,0.0f);
        glVertex3f(-1.0f,-1.0f,0.0f);
        glVertex3f(1.0f,-1.0f,0.0f);
    glEnd();
}


void setup()
{


	pufCameraInit(&camera, 50.0f, 0.1f, 100.0f);
	
	pufMeshInit(&cubeMesh);
	pufMeshInit(&framebufferMesh);
	pufMeshLoadOBJ(&cubeMesh, "../solid.obj");
	pufMeshShapeQuad(&framebufferMesh);
	pufMeshBind(&cubeMesh);
	pufMeshBind(&framebufferMesh);
	pufTextureLoadBMP(&cubeTexture, "../puffin.bmp");
	pufTextureCreate(&framebufferTexture,width,height);
	
	pufMeshTranslate(&cubeMesh,0.0f,0.0f,-1.0f);
	pufMeshRotateEuler(&cubeMesh,0.0f,0.0f,-55.0f,DEGREES);

	pufShaderLoad(&cubeShader, "../meshVert.glsl", "../meshFrag.glsl");
	pufShaderLoad(&framebufferShader, "../framebufferVert.glsl", "../framebufferFrag.glsl");
	
	pufFramebufferInit(&framebuffer);
	pufFramebufferTexture(&framebuffer,&framebufferTexture);

	glClearColor(1,1,1,1); // Set the clear color white

}

void draw()
{
        //draw_immidiate_triangle();
        
        pufTextureBind(&cubeTexture);
	    
        //pufFramebufferBindAndClear(&framebuffer);

	    pufMeshRender(&cubeMesh,&camera,&cubeShader, &framebuffer);

	    //pufFramebufferUnbind();
	    
        pufTextureBind(&framebufferTexture);

        framebufferShader.uniformTime = glfwGetTime();    
        pufMeshRender(&framebufferMesh,&camera,&framebufferShader, NULL);
        pufMeshRotate(&cubeMesh, -0.5,1.0,0.0,0.0, DEGREES);
	    /*
        pufUpdate(&window);

	    if (!isPaused)
		    pufMeshRotate(&cubeMesh, -0.5,1.0,0.0,0.0, DEGREES);
        */
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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);

    window = glfwCreateWindow(width, height, "My Title", NULL, NULL);

    if (!window)
    {/* error */}
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);
    glewInit();

    //glViewport(0,0,width,height);



    setup();

    while (!glfwWindowShouldClose(window))
    {
        glfwGetFramebufferSize(window, &width, &height);
    
        //draw_immidiate_triangle();
        draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    }

    glfwDestroyWindow(window);
    glfwTerminate();


}