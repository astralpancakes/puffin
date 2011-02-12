#ifndef PUFFIN_H
#define PUFFIN_H
#include <stdbool.h>
#include <SDL/SDL.h>

typedef struct
{
    GLfloat position[3];
    GLfloat color[3];
    GLfloat texture[3];
    GLfloat normal[3];
    GLfloat meta3f[3];
    GLfloat meta1f;
}
PFNvertex;


typedef struct
{
    PFNvertex* verts;
    int vertexCount;
    GLuint vertexBuffer;
    GLfloat modelView[16];
    // shader
    // asdf
}
PFNmesh;

typedef struct
{
    GLuint shaderProgram;
}
PFNshader;

typedef struct
{
    GLuint texture;
}
PFNtexture;

typedef struct
{
    GLfloat* projectionMatrix;
    SDL_Surface* screen;
    int width;
    int height;
}
PFNview;

void pfnViewInit(PFNview*,int, int, float);
void pfnViewResize(PFNview*,int, int);

void pfnMeshInit(PFNmesh*);
void pfnMeshLoadOBJ(PFNmesh*, char const*);
void pfnMeshBind(PFNmesh*);
void pfnMeshDraw(PFNmesh*, PFNview*, PFNshader*);

void pfnMeshTranslate(PFNmesh*, float, float, float);
void pfnMeshRotate(PFNmesh*, float, float, float, float, bool);

void pfnMeshDestroy(PFNmesh*);

void pfnShaderLoad(PFNshader*,char const*,char const*);

void pfnTextureLoadBMP(PFNtexture*, char const*);

#endif
