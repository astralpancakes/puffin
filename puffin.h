#ifndef PUFFIN_H
#define PUFFIN_H
#include <stdbool.h>
#include <SDL/SDL.h>
#ifdef PUFFIN_GLEWDOTFRAMEWORK
#include <GLEW/glew.h>
#else
#include <GL/glew.h>
#endif

typedef struct
{
    GLfloat position[3];
    GLfloat color[3];
    GLfloat texture[3];
    GLfloat normal[3];
    GLfloat meta3f[3];
    GLfloat meta1f;
}
PUFvertex;


typedef struct
{
    PUFvertex* verts;
    int vertexCount;
    GLuint vertexBuffer;
    GLfloat modelView[16];
}
PUFmesh;

typedef struct
{
    GLuint shaderProgram;
}
PUFshader;

typedef struct
{
    GLuint textureId;
    //GLenum textureUnit;
    GLulong width;
    GLulong height;
    GLint  pixelBytes;    
    GLuint pixelBuffer;
    GLchar* pixels;
    GLenum textureFormat;

}
PUFtexture;

typedef struct
{
    GLfloat projectionMatrix[16];
    GLfloat cameraMatrix[16];
    int width;
    int height;
	float nearClip;
	float farClip;
}
PUFcamera;

void pufInit(int, int, int);
void pufUpdate();

float pufGetStats(int);

void pufWindowResize(int, int);


void pufCameraInit(PUFcamera*, float, float, float);
void pufCameraTranslate(PUFcamera*, float, float, float);
void pufCameraRotate(PUFcamera*, float, float, float, float, bool);

void pufMeshInit(PUFmesh*);
void pufMeshShapeQuad(PUFmesh*);
void pufMeshLoadOBJ(PUFmesh*, char const*);
void pufMeshBind(PUFmesh*);
void pufMeshDraw(PUFmesh*, PUFcamera*, PUFshader*);

void pufMeshTranslate(PUFmesh*, float, float, float);
void pufMeshRotate(PUFmesh*, float, float, float, float, bool);
void pufMeshScale(PUFmesh*, float,float,float);

void pufMeshDestroy(PUFmesh*);

void pufShaderLoad(PUFshader*,char const*,char const*);

void pufTextureLoadBMP(PUFtexture*, char const*);
void pufTextureOldLoadBMP(PUFtexture*, char const*);
void pufTextureCreateRGBA(PUFtexture*, GLulong, GLulong);
void pufTextureUpdate(PUFtexture*);
void pufTextureBind(PUFtexture*);

void pufTextureDestroy(PUFtexture*);

#endif
