#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "../src/puffin.h"

PUFwindow window;

PUFcamera camera;

PUFmesh cubeMesh;
PUFshader cubeShader;
PUFtexture cubeTexture;

PUFmesh framebufferMesh;
PUFshader framebufferShader;
PUFtexture framebufferTexture;
PUFframebuffer framebuffer;

int isPaused = 0;

void keyboard(unsigned char key, int specialKey, int x, int y)
{

	// check keys by ASCII code

	switch (key)
	{
		case 32:
		{
			if (isPaused)
				isPaused = 0;
			else
				isPaused = 1;
		} 
	}

	// check special keys by GLUT's GLUT_KEY_* enums

	switch (specialKey)
	{
		case GLUT_KEY_F1:
		{
			if (isPaused)
				isPaused = 0;
			else
				isPaused = 1;
			break;
		}
		
		case GLUT_KEY_UP:
		{
			pufCameraTranslate(&camera, 0.0f,0.0f,-0.15f);
			break;
		}
		case GLUT_KEY_DOWN:
		{
			pufCameraTranslate(&camera, 0.0f,0.0f,0.15f);
			break;
		}
		case GLUT_KEY_LEFT:
		{
			pufCameraTranslate(&camera, -0.15f,0.0f,0.0f);
			break;
		}
		case GLUT_KEY_RIGHT:
		{
			pufCameraTranslate(&camera, 0.15f,0.0f,0.0f);
			break;
		}
	}

	// for modifier keys use glutGetModifiers

}

int main(int argc, char** argv)
{
	pufRun();
	return 0;
}


void setup()
{
	window = pufInit(1280,720,60,"Puffin Example");
	pufCameraInit(&window, &camera, 50.0f, 0.1f, 100.0f);
	
	pufMeshInit(&cubeMesh);
	pufMeshInit(&framebufferMesh);
	pufMeshLoadOBJ(&cubeMesh, "../solid.obj");
	pufMeshShapeQuad(&framebufferMesh);
	pufMeshBind(&cubeMesh);
	pufMeshBind(&framebufferMesh);
	pufTextureLoadBMP(&cubeTexture, "../puffin.bmp");
	pufTextureCreate(&framebufferTexture,1280,720);
	
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
	
	pufTextureBind(&cubeTexture);
	pufFramebufferBindAndClear(&framebuffer);
	pufMeshRender(&cubeMesh,&camera,&cubeShader);
	pufFramebufferUnbind();
	
	pufTextureBind(&framebufferTexture);
	pufMeshRender(&framebufferMesh,&camera,&framebufferShader);
	pufUpdate(&window);

	if (!isPaused)
		pufMeshRotate(&cubeMesh, -0.5,1.0,0.0,0.0, DEGREES);
}