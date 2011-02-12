#include <SDL/SDL.h>
#include <GL/glew.h>
#include "puffin.h"
#include "loadobj.h"
#include "matrix.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

void pfnViewInit(PFNview* scene,int width, int height, float fov)
{
    if (height == 0)
        height = 1;
    scene->width = width;
    scene->height = height;
    scene->screen = SDL_SetVideoMode(width,height,24,SDL_OPENGL|SDL_GL_DOUBLEBUFFER|SDL_RESIZABLE);
    glViewport(0,0, width,height);
    scene->projectionMatrix = pfnMatrixProject(fov, (float)width/height, 0.1f, 100.0f,true);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

}

void pfnViewResize(PFNview* scene, int width, int height)
{
    if (height == 0)
        height = 1;
    scene->width = width;
    scene->height = height;
    scene->screen = SDL_SetVideoMode(width,height,24,SDL_OPENGL|SDL_GL_DOUBLEBUFFER|SDL_RESIZABLE);
    glViewport(0,0, width,height);
    scene->projectionMatrix = pfnMatrixProject(25.0, (float)width/height, 0.1f, 100.0f,true);
}


void pfnMeshInit(PFNmesh* mesh) // zero the modelview matrix of the mesh
{
    for (int i=0;i < 16; i++)
        mesh->modelView[i] = 0.0f;
    mesh->modelView[0] = 1.0f;
    mesh->modelView[5] = 1.0f;
    mesh->modelView[10] = 1.0f;
    mesh->modelView[15] = 1.0f;

}

void pfnMeshLoadOBJ(PFNmesh* mesh, char const* name)
{
    GLfloat* obj = loadOBJ(name, &mesh->vertexCount);
    mesh->verts = (GLfloat*)malloc(sizeof(PFNvertex)*mesh->vertexCount);


    //srand(125); // XD

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

void pfnMeshBind(PFNmesh* mesh)
{
    glGenBuffers(1, &mesh->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PFNvertex)*mesh->vertexCount, NULL,GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(PFNvertex)*mesh->vertexCount, mesh->verts);

}

void pfnMeshDraw(PFNmesh* mesh, PFNview* view, PFNshader* shader)
{

        glUseProgram(shader->shaderProgram);


        GLint uniformModelviewProjection = glGetUniformLocation(shader->shaderProgram, "modelviewProjection");




        GLfloat modelviewProjectionMatrix[16];
        pfnMatrixMult(view->projectionMatrix, mesh->modelView, modelviewProjectionMatrix);
        glUniformMatrix4fv(uniformModelviewProjection, 1, GL_FALSE, modelviewProjectionMatrix);

        GLint uniformNormalMatrix = glGetUniformLocation(shader->shaderProgram, "modelviewMatrix");
        glUniformMatrix4fv(uniformNormalMatrix, 1, GL_FALSE, mesh->modelView);

        //shaderProgram = 666;
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


        glVertexAttribPointer(vertexPosition, 3, GL_FLOAT, 1, sizeof(PFNvertex), (void*)0);
        glVertexAttribPointer(vertexColor, 3, GL_FLOAT, 0, sizeof(PFNvertex), ((char*)NULL) + (sizeof(GLfloat)*3));
        glVertexAttribPointer(vertexTexture, 2, GL_FLOAT, 0 , sizeof(PFNvertex), ((char*)NULL) + (sizeof(GLfloat)*6));
        glVertexAttribPointer(vertexNormal, 3, GL_FLOAT, 0, sizeof(PFNvertex), ((char*)NULL) + (sizeof(GLfloat)*9));
        glVertexAttribPointer(vertexMeta3f, 3, GL_FLOAT, 0, sizeof(PFNvertex), ((char*)NULL) + (sizeof(GLfloat)*12));
        glVertexAttribPointer(vertexMeta1f, 1, GL_FLOAT, 0, sizeof(PFNvertex), ((char*)NULL) + (sizeof(GLfloat)*15));

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

void pfnMeshTranslate(PFNmesh* mesh, float X, float Y, float Z)
{
        GLfloat temp[16];
        pfnMatrixMult(memcpy(temp,mesh->modelView,sizeof(GLfloat)*16),pfnMatrixTranslate(X,Y,Z),mesh->modelView);

}


void pfnMeshRotate(PFNmesh* mesh, float ang, float X, float Y, float Z, bool degrees)
{
    GLfloat temp[16];
    pfnMatrixMult(memcpy(temp,mesh->modelView,sizeof(GLfloat)*16),pfnMatrixRotate(ang,X,Y,Z,degrees),mesh->modelView);
}

void pfnMeshDestroy(PFNmesh* mesh)
{
    free((void*)mesh->verts);
}

void pfnShaderLoad(PFNshader* shader, char const* vertexShaderSourceFile, char const* fragmentShaderSourceFile)
{

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    const GLchar* vertexShaderSource = readFile(vertexShaderSourceFile);
    glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
    glCompileShader(vertexShader);
    free((void*)vertexShaderSource);


    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* fragmentShaderSource = readFile(fragmentShaderSourceFile);
    glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
    glCompileShader(fragmentShader);
    free((void*)fragmentShaderSource);

    shader->shaderProgram = glCreateProgram();
    glAttachShader(shader->shaderProgram,vertexShader);
    glAttachShader(shader->shaderProgram,fragmentShader);
    glLinkProgram(shader->shaderProgram);
}


void pfnTextureLoadBMP(PFNtexture* texture, char const* name)
{
SDL_Surface* surface;

surface = SDL_LoadBMP(name);

GLenum texture_format;
GLint  nOfColors;

	// Check that the image's width is a power of 2
	if ( (surface->w & (surface->w - 1)) != 0 ) {
//		printf("warning: image.bmp's width is not a power of 2\n");
	}

	// Also check if the height is a power of 2
	if ( (surface->h & (surface->h - 1)) != 0 ) {
//		printf("warning: image.bmp's height is not a power of 2\n");
	}

        // get the number of channels in the SDL surface
        nOfColors = surface->format->BytesPerPixel;
        if (nOfColors == 4)     // contains an alpha channel
        {
                if (surface->format->Rmask == 0x000000ff)
                        texture_format = GL_RGBA;
                else
                        texture_format = GL_BGRA;
        } else if (nOfColors == 3)     // no alpha channel
        {
                if (surface->format->Rmask == 0x000000ff)
                        texture_format = GL_RGB;
                else
                        texture_format = GL_BGR;
        } else {
//                printf("warning: the image is not truecolor..  this will probably break\n");
//                // this error should not go unhandled
        }

glGenTextures(1,&(texture->texture));
glBindTexture(GL_TEXTURE_2D, texture->texture);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

glTexImage2D(GL_TEXTURE_2D,0,nOfColors,surface->w,surface->h,0,texture_format,GL_UNSIGNED_BYTE,surface->pixels);

if(surface)
SDL_FreeSurface(surface);
}
