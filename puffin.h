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
    GLuint width;
    GLuint height;
    GLint  pixelBytes;    
    GLuint pixelBuffer;
    GLubyte* pixels;
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

typedef struct
{
    float time;
    float value;
    
    float handle1time;
    float handle1value;
    float handle2time;
    float handle2value;
}
PUFkeyframe;

typedef struct
{
    PUFkeyframe* keys;
    int keyCount;
}
PUFanimation;


void pufInit(int windowWidth, int windowHeight, int framerate);
void pufUpdate();

float pufGetStats(int type);

void pufWindowResize(int windowWidth, int windowHeight);

void pufCameraInit(PUFcamera* camera, float fov, float nearClip, float farClip);
void pufCameraTranslate(PUFcamera* camera, float X, float Y, float Z);
void pufCameraRotate(PUFcamera* camera, float angle, float X, float Y, float Z, bool degrees);

void pufMeshInit(PUFmesh* mesh);
void pufMeshShapeQuad(PUFmesh* mesh);
void pufMeshLoadOBJ(PUFmesh* mesh, char const* file);
void pufMeshBind(PUFmesh* mesh);
void pufMeshDraw(PUFmesh* mesh, PUFcamera* camera, PUFshader* shader);

void pufMeshTranslate(PUFmesh* mesh, float X, float Y, float Z);
void pufMeshRotate(PUFmesh* mesh, float angle, float X, float Y, float Z, bool);
void pufMeshScale(PUFmesh* mesh, float X,float Y,float Z);

void pufMeshDestroy(PUFmesh* mesh);

void pufShaderLoad(PUFshader* shader,char const* vertexShaderSourceFile,char const* fragmentShaderSourceFile);

void pufTextureLoadBMP(PUFtexture* texture, char const* file);
void pufTextureOldLoadBMP(PUFtexture*, char const*);
void pufTextureCreateRGBA(PUFtexture* texture, GLuint width, GLuint height);
void pufTextureCreateRGB(PUFtexture* texture, GLuint width, GLuint height);
void pufTextureClear(PUFtexture* texture);
void pufTextureUpdate(PUFtexture* texture);
void pufTextureBind(PUFtexture* texture);

void pufTextureDestroy(PUFtexture* texture);

#endif
