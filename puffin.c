#include <SDL/SDL.h>
#ifdef PUFFIN_GLEWDOTFRAMEWORK
#include <GLEW/glew.h>
#else
#include <GL/glew.h>
#endif
#include "puffin.h"
#include "loadobj.h"
#include "matrix.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>


//globals

struct {
	int windowHeight;
	int windowWidth;
	SDL_Surface* windowSurface;
	int glMajorVersion;
	int glMinorVersion;
} puffin;

void pufInit(int width, int height)
{
	if (height == 0)	// prevent divide by zero etc
        height = 1;
	puffin.windowWidth = width;
	puffin.windowHeight = height;
	SDL_Init(SDL_INIT_EVERYTHING);
	puffin.windowSurface = SDL_SetVideoMode(width,height,24,SDL_OPENGL|SDL_GL_DOUBLEBUFFER|SDL_RESIZABLE);
	glewInit();

	//get OpenGL version
	
	puffin.glMajorVersion = atoi(&glGetString(GL_VERSION)[0]);
	puffin.glMinorVersion = atoi(&glGetString(GL_VERSION)[2]);
}

void pufWindowResize(int width, int height)
{
    puffin.windowSurface = SDL_SetVideoMode(width,height,24,SDL_OPENGL|SDL_GL_DOUBLEBUFFER|SDL_RESIZABLE);
}

void pufViewInit(PUFview* view, float fov, float nearClip, float farClip)
{
    view->width = puffin.windowWidth;
    view->height = puffin.windowHeight;
    glViewport(0,0, view->width,view->height);
	glShadeModel(GL_SMOOTH);
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    pufMatrixProject(fov, view->width, view->height, nearClip, farClip,true, view->projectionMatrix);	
}

void pufMeshInit(PUFmesh* mesh) // zero the modelview matrix of the mesh
{
    for (int i=0;i < 15; i++)
		mesh->modelView[i] = 0.0f;
	
    mesh->modelView[0] = 1.0f;
    mesh->modelView[5] = 1.0f;
    mesh->modelView[10] = 1.0f;
    mesh->modelView[15] = 1.0f;
}

void pufMeshLoadOBJ(PUFmesh* mesh, char const* name)
{
    GLfloat* obj = pufLoadOBJ(name, &mesh->vertexCount);
    mesh->verts = (GLfloat*)malloc(sizeof(PUFvertex)*mesh->vertexCount);

    for (int i=0;i<mesh->vertexCount;++i)
    {
        mesh->verts[i].position[0] = obj[i*9+0];
        mesh->verts[i].position[1] = obj[i*9+1];
        mesh->verts[i].position[2] = obj[i*9+2];
        mesh->verts[i].color[0] = ((float)(rand() % 100))/100;
        mesh->verts[i].color[1] = (float)(rand() % 100)/100;
        mesh->verts[i].color[2] = (float)(rand() % 100)/100;
        mesh->verts[i].texture[0] = obj[i*9+3];
        mesh->verts[i].texture[1] = obj[i*9+4];
        mesh->verts[i].texture[2] = obj[i*9+5];
        mesh->verts[i].normal[0] = obj[i*9+6];
        mesh->verts[i].normal[1] = obj[i*9+7];
        mesh->verts[i].normal[2] = obj[i*9+8];
    }
    free((void*)obj);
}

void pufMeshBind(PUFmesh* mesh)
{
	if (puffin.glMajorVersion == 1)
	{
		glGenBuffersARB(1, &mesh->vertexBuffer);
		glBindBufferARB(GL_ARRAY_BUFFER, mesh->vertexBuffer);
		glBufferDataARB(GL_ARRAY_BUFFER, sizeof(PUFvertex)*mesh->vertexCount, NULL,GL_STATIC_DRAW);
		glBufferSubDataARB(GL_ARRAY_BUFFER, 0, sizeof(PUFvertex)*mesh->vertexCount, mesh->verts);
	}
	else {
		glGenBuffers(1, &mesh->vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(PUFvertex)*mesh->vertexCount, NULL,GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(PUFvertex)*mesh->vertexCount, mesh->verts);
	}

	
}

void pufMeshDraw(PUFmesh* mesh, PUFview* view, PUFshader* shader)
{
	if (puffin.glMajorVersion == 1)
	{
		glUseProgramObjectARB(shader->shaderProgram);
	
		GLint uniformModelviewProjection = glGetUniformLocationARB(shader->shaderProgram, "modelviewProjection");
		GLfloat modelviewProjectionMatrix[16];
	
		pufMatrixMult(view->projectionMatrix, mesh->modelView, modelviewProjectionMatrix);
		glUniformMatrix4fvARB(uniformModelviewProjection, 1, GL_FALSE, modelviewProjectionMatrix);

		GLint uniformNormalMatrix = glGetUniformLocationARB(shader->shaderProgram, "modelviewMatrix");
		glUniformMatrix4fvARB(uniformNormalMatrix, 1, GL_FALSE, mesh->modelView);

		GLint vertexPosition = glGetAttribLocationARB(shader->shaderProgram, "vertexPosition");
		GLint vertexColor = glGetAttribLocationARB(shader->shaderProgram, "vertexColor");
		GLint vertexTexture = glGetAttribLocationARB(shader->shaderProgram, "vertexTexture");
		GLint vertexNormal = glGetAttribLocationARB(shader->shaderProgram, "vertexNormal");
		GLint vertexMeta3f = glGetAttribLocationARB(shader->shaderProgram, "inMeta3f");
		GLint vertexMeta1f = glGetAttribLocationARB(shader->shaderProgram, "inMeta1f");

		glEnableVertexAttribArrayARB(vertexPosition);
		glEnableVertexAttribArrayARB(vertexColor);
		glEnableVertexAttribArrayARB(vertexTexture);
		glEnableVertexAttribArrayARB(vertexNormal);
		glEnableVertexAttribArrayARB(vertexMeta3f);
		glEnableVertexAttribArrayARB(vertexMeta1f);


		glBindBufferARB(GL_ARRAY_BUFFER, mesh->vertexBuffer);


		glVertexAttribPointerARB(vertexPosition, 3, GL_FLOAT, 1, sizeof(PUFvertex), (void*)0);
		glVertexAttribPointerARB(vertexColor, 3, GL_FLOAT, 0, sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*3));
		glVertexAttribPointerARB(vertexTexture, 2, GL_FLOAT, 0 , sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*6));
		glVertexAttribPointerARB(vertexNormal, 3, GL_FLOAT, 0, sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*9));
		glVertexAttribPointerARB(vertexMeta3f, 3, GL_FLOAT, 0, sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*12));
		glVertexAttribPointerARB(vertexMeta1f, 1, GL_FLOAT, 0, sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*15));

		glDrawArrays(GL_TRIANGLES,0,mesh->vertexCount);

		glBindBufferARB(GL_ARRAY_BUFFER, 0);

		glDisableVertexAttribArrayARB(vertexMeta1f);
		glDisableVertexAttribArrayARB(vertexMeta3f);
		glDisableVertexAttribArrayARB(vertexNormal);
		glDisableVertexAttribArrayARB(vertexTexture);
		glDisableVertexAttribArrayARB(vertexColor);
		glDisableVertexAttribArrayARB(vertexPosition);
		glUseProgramObjectARB(0);
	}
	else 
	{
		glUseProgram(shader->shaderProgram);
		GLint uniformModelviewProjection = glGetUniformLocation(shader->shaderProgram, "modelviewProjection");
		GLfloat modelviewProjectionMatrix[16];
	
		pufMatrixMult(view->projectionMatrix, mesh->modelView, modelviewProjectionMatrix);
		glUniformMatrix4fv(uniformModelviewProjection, 1, GL_FALSE, modelviewProjectionMatrix);
	
		GLint uniformNormalMatrix = glGetUniformLocation(shader->shaderProgram, "modelviewMatrix");
		glUniformMatrix4fv(uniformNormalMatrix, 1, GL_FALSE, mesh->modelView);
	
		GLint vertexPosition = glGetAttribLocation(shader->shaderProgram, "vertexPosition");
		GLint vertexColor = glGetAttribLocation(shader->shaderProgram, "vertexColor");
		GLint vertexTexture = glGetAttribLocation(shader->shaderProgram, "vertexTexture");
		GLint vertexNormal = glGetAttribLocation(shader->shaderProgram, "vertexNormal");
		GLint vertexMeta3f = glGetAttribLocation(shader->shaderProgram, "inMeta3f");
		GLint vertexMeta1f = glGetAttribLocation(shader->shaderProgram, "inMeta1f");
	
		glEnableVertexAttribArray(vertexPosition);
		glEnableVertexAttribArray(vertexColor);
		glEnableVertexAttribArray(vertexTexture);
		glEnableVertexAttribArray(vertexNormal);
		glEnableVertexAttribArray(vertexMeta3f);
		glEnableVertexAttribArray(vertexMeta1f);
	
	
		glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	
	
		glVertexAttribPointer(vertexPosition, 3, GL_FLOAT, 1, sizeof(PUFvertex), (void*)0);
		glVertexAttribPointer(vertexColor, 3, GL_FLOAT, 0, sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*3));
		glVertexAttribPointer(vertexTexture, 2, GL_FLOAT, 0 , sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*6));
		glVertexAttribPointer(vertexNormal, 3, GL_FLOAT, 0, sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*9));
		glVertexAttribPointer(vertexMeta3f, 3, GL_FLOAT, 0, sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*12));
		glVertexAttribPointer(vertexMeta1f, 1, GL_FLOAT, 0, sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*15));
	
		glDrawArrays(GL_TRIANGLES,0,mesh->vertexCount);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	
		glDisableVertexAttribArray(vertexMeta1f);
		glDisableVertexAttribArray(vertexMeta3f);
		glDisableVertexAttribArray(vertexNormal);
		glDisableVertexAttribArray(vertexTexture);
		glDisableVertexAttribArray(vertexColor);
		glDisableVertexAttribArray(vertexPosition);
		glUseProgram(0);
	}

}

void pufMeshTranslate(PUFmesh* mesh, float X, float Y, float Z)
{
	//copy the existing mesh modelview matrix into tempModelView, ask for a new matrix for tempMatrix, then multiply them and put into mesh modelview
	GLfloat tempModelView[16];
	GLfloat tempMatrix[16];
	pufMatrixMult(memcpy(tempModelView,mesh->modelView,sizeof(GLfloat)*16),pufMatrixTranslate(X,Y,Z,tempMatrix),mesh->modelView);
}


void pufMeshRotate(PUFmesh* mesh, float ang, float X, float Y, float Z, bool degrees)
{
    GLfloat tempModelView[16];
	GLfloat tempMatrix[16];
    pufMatrixMult(memcpy(tempModelView,mesh->modelView,sizeof(GLfloat)*16),pufMatrixRotate(ang,X,Y,Z,degrees,tempMatrix),mesh->modelView);
}

void pufMeshScale(PUFmesh* mesh, float X, float Y, float Z)
{
	GLfloat tempModelView[16];
	GLfloat tempMatrix[16];
    pufMatrixMult(memcpy(tempModelView,mesh->modelView,sizeof(GLfloat)*16),pufMatrixScale(X,Y,Z,tempMatrix),mesh->modelView);
}

void pufMeshDestroy(PUFmesh* mesh)
{
    free((void*)mesh->verts);
	free(mesh);
}

void pufShaderLoad(PUFshader* shader, char const* vertexShaderSourceFile, char const* fragmentShaderSourceFile)
{
	if (puffin.glMajorVersion == 1)
	{
		GLuint vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER);
		const GLchar* vertexShaderSource = pufReadFile(vertexShaderSourceFile);
		glShaderSourceARB(vertexShader,1,&vertexShaderSource,NULL);
		glCompileShaderARB(vertexShader);
		free((void*)vertexShaderSource);


		GLuint fragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER);

		const GLchar* fragmentShaderSource = pufReadFile(fragmentShaderSourceFile);
		glShaderSourceARB(fragmentShader,1,&fragmentShaderSource,NULL);
		glCompileShaderARB(fragmentShader);
		free((void*)fragmentShaderSource);

		shader->shaderProgram = glCreateProgramObjectARB();
		glAttachObjectARB(shader->shaderProgram,vertexShader);
		glAttachObjectARB(shader->shaderProgram,fragmentShader);
		glLinkProgramARB(shader->shaderProgram);
	}
	else 
	{
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		const GLchar* vertexShaderSource = pufReadFile(vertexShaderSourceFile);
		glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
		glCompileShader(vertexShader);
		free((void*)vertexShaderSource);
		
		
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		
		const GLchar* fragmentShaderSource = pufReadFile(fragmentShaderSourceFile);
		glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
		glCompileShader(fragmentShader);
		free((void*)fragmentShaderSource);
		
		shader->shaderProgram = glCreateProgram();
		glAttachShader(shader->shaderProgram,vertexShader);
		glAttachShader(shader->shaderProgram,fragmentShader);
		glLinkProgram(shader->shaderProgram);
	}

}

void pufShaderDestroy(PUFshader* shader)
{
	free(shader);
}


void pufTextureLoadBMP(PUFtexture* texture, char const* name)
{
SDL_Surface* surface;

surface = SDL_LoadBMP(name);

//GLenum texture_format;
//GLint  nOfColors;

#ifdef PUFFIN_SQUAWK
	printf("Loading image %s...\n", name);
	if ( (surface->w & (surface->w - 1)) != 0 ) {
		printf("Warning: Width of %s is not a power of 2\n",name);
	}


	if ( (surface->h & (surface->h - 1)) != 0 ) {
		printf("Warning: Height of %s is not a power of 2\n",name);
	}
#endif
	
		texture->colorCount = surface->format->BytesPerPixel; // get the number of channels in the SDL surface
        if (texture->colorCount == 4)     // contains an alpha channel
        {
                if (surface->format->Rmask == 0x000000ff)
                        texture->textureFormat = GL_RGBA;
                else
                        texture->textureFormat = GL_BGRA;
        } 
		else if (texture->colorCount == 3)     // no alpha channel
        {
                if (surface->format->Rmask == 0x000000ff)
                        texture->textureFormat = GL_RGB;
                else
                        texture->textureFormat = GL_BGR;
        } 
		else 
		{
                printf("warning: %s is not truecolor\n",name);
        }


	glGenBuffers(1, &texture->pixelBuffer);
	
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texture->pixelBuffer);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, surface->w*surface->h*surface->format->BytesPerPixel, NULL,GL_STATIC_DRAW);
	glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, surface->w*surface->h*surface->format->BytesPerPixel, surface->pixels);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,texture->colorCount,surface->w,surface->h,0,texture->textureFormat,GL_UNSIGNED_BYTE,NULL);
	if(surface)
		SDL_FreeSurface(surface);
}
