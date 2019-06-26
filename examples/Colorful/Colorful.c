#include "../../src/puffin.h"
#include "../../extra/palette.c"
#include <GLFW/glfw3.h>

#include <stdio.h>

GLFWwindow* window;

PUFcamera camera;

PUFmesh textureQuad;
PUFtexture texture;
PUFtexture palette;
PUFshader textureShader;

PUFcolor currentColor;
PUFcolor paletteColor;

const int width = 640;
const int height = 360;

#define scaling 20
#define fadeRate 0.01

const char* windowTitle = "Window title";

void setup() // setup() is run once at program start
{
    printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));

    camera = pufCameraInit(0.0f, 0.1f, 10.0f);

    textureQuad = pufMeshShapeQuad();
    textureShader = pufShaderLoad("../../shader/vsFrameBasic.glsl", "../../shader/fsMeshTexture.glsl");
    texture = pufTextureCreate(width/scaling, height/scaling);
    palette = pufPaletteKorruptor();

    pufMeshBind(&textureQuad);
    pufTextureBind(&texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

}

void draw() // draw() is run every frame
{
    for (int y = 0; y < texture.height; y++)
    {
        for (int x = 0; x < texture.width; x++)
        {
            currentColor = pufTexturePixelGet(&texture, x, y);
            //color = pufColorFromRGBA(((float)x/(float)texture.width), ((float)y/(float)texture.height), 0.0f, 1.0f);
            if (currentColor.A != 0.0f)
            {
                int index = (int)((1.0 - currentColor.A)/fadeRate);
                paletteColor = pufTexturePixelGetByIndex(&palette, index);
                currentColor.R = paletteColor.R;
                currentColor.G = paletteColor.G;
                currentColor.B = paletteColor.B;
                currentColor.A -= fadeRate;
                pufTexturePixelSet(&texture,x, y, currentColor);
            }
        }
    }
    pufTextureUpdate(&texture);
    pufTextureBind(&texture);
    pufMeshRender(&textureQuad, &camera, &textureShader, NULL);
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

void handleMouse(GLFWwindow* window, double xpos, double ypos)
{
int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
if (state == GLFW_PRESS)
    {
        currentColor = pufColorFromRGBA(1.0f, 1.0f, 1.0f, 1.0f);
        pufTexturePixelSet(&texture, (int)(xpos/scaling), texture.height-(int)(ypos/scaling), currentColor);
    }
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
            glfwSetCursorPosCallback(window, handleMouse);
            
            #ifdef _WIN32
            glewInit();
            #endif

            setup();

            while (!glfwWindowShouldClose(window))
            {

                draw();
                glfwPollEvents();
                glfwSwapBuffers(window);
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