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
    // shader
    // asdf
}
PUFmesh;

typedef struct
{
    GLuint shaderProgram;
}
PUFshader;

typedef struct
{
    GLuint texture;
	GLenum textureFormat;
	GLint  colorCount;
	GLuint pixelBuffer;
}
PUFtexture;

typedef struct
{
    GLfloat projectionMatrix[16];
    int width;
    int height;
	float nearClip;
	float farClip;
}
PUFview;

void pufInit(int, int);

void pufWindowResize(int, int);

void pufViewInit(PUFview*, float, float, float);

void pufMeshInit(PUFmesh*);
void pufMeshShapeQuad(PUFmesh*);
void pufMeshLoadOBJ(PUFmesh*, char const*);
void pufMeshBind(PUFmesh*);
void pufMeshDraw(PUFmesh*, PUFview*, PUFshader*);

void pufMeshTranslate(PUFmesh*, float, float, float);
void pufMeshRotate(PUFmesh*, float, float, float, float, bool);
void pufMeshScale(PUFmesh*, float,float,float);

void pufMeshDestroy(PUFmesh*);

void pufShaderLoad(PUFshader*,char const*,char const*);
void pufShaderDestroy(PUFshader*);

void pufTextureLoadBMP(PUFtexture*, char const*);

#endif
