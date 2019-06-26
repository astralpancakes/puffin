#include "../../src/puffin.h"
#include <GLFW/glfw3.h>

#include <stdio.h>

GLFWwindow* window;

int width = 640;
int height = 360;

const char* windowTitle = "Window title";

void setup() // setup() is run once at program start
{
    printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));

}

void draw() // draw() is run every frame
{

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

        window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

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