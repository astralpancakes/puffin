#ifndef PUFFIN_H
#define PUFFIN_H

#ifdef _WIN32
    #include <GL/glew.h>
#elif __APPLE__
    #include <OpenGL/gl3.h>
#endif

#include "helpers.h"

//Generic 4D vector
typedef struct {
    double x;
    double y;
    double z;
    double w;
} PUFvec4;

typedef PUFvec4 PUFvector;

//Generic vec3 / Euler angle
typedef struct {
    double x;
    double y;
    double z;
} PUFvec3;

typedef PUFvec3 PUFangle;

typedef struct {
    float R;
    float G;
    float B;
    float A;
} PUFcolor;

typedef float PUFmat4x4[16];
typedef PUFmat4x4 PUFmatrix;

typedef enum {
    DEGREES,
    RADIANS
} PUF_ANGLE_UNITS;

typedef enum {
    OBJECT,
    WORLD
} PUF_COORDINATE_SPACE;

typedef struct {
	unsigned int windowHeight;
	unsigned int windowWidth;
    unsigned int frameRate;
    unsigned int windowID;
	int glMajorVersion;
	int glMinorVersion;
    unsigned long frameCounter;
    unsigned long frameTick; 
    unsigned long frameDelay;
} PUFwindow;

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
	GLfloat meshScale[3];
	GLfloat meshTranslation[3];
	GLfloat meshRotation[4];
    //GLfloat modelView[16];
}
PUFmesh;

typedef struct
{
    GLuint shaderProgram;
    
    // Shader uniforms, these are supposed to be the same as in ShaderToy
    // see https://shadertoyunofficial.wordpress.com/2016/07/20/special-shadertoy-features/ and https://www.shadertoy.com/howto

    //PUFvec3 uniformResolution;
    float uniformTime;
    float uniformTimeDelta;
    float uniformFrame;
    float uniformChannelTime[4];
    PUFvec4 uniformMouse;
    PUFvec4 uniformDate;
    float uniformSampleRate;
    PUFvec3 uniformChannelResolution[4];
    //uniform samplerXX iChanneli;
}
PUFshader;

typedef struct
{
    GLuint textureId;
    GLenum textureUnit;
    GLuint width;
    GLuint height;
    //GLint  pixelBytes;
    GLuint pixelBuffer;
    GLfloat* pixels;
    GLenum textureFormat;

}
PUFtexture;

typedef struct
{
    GLuint framebufferId;
    GLuint depthbufferId;
    GLuint depthbufferWidth;
    GLuint depthbufferHeight;
    int width;
    int height;
    int textureIsAttached;
}
PUFframebuffer;

typedef struct
{
    GLfloat projectionMatrix[16];
    GLfloat cameraMatrix[16];
	GLfloat translationMatrix[16];
    GLfloat cameraScale[3];
	GLfloat cameraTranslation[3];
	GLfloat cameraRotation[4];
    int width;
    int height;
	float nearClip;
	float farClip;

    PUFwindow* window;
}
PUFcamera;


PUFvec4 pufVectorFromAngle(double pitch, double yaw, PUF_ANGLE_UNITS units);

PUFwindow pufInit(int windowWidth, int windowHeight, int framerate, const char * windowTitle);
void pufIdle(void);
void pufRun();
void pufUpdate(PUFwindow* window);

void pufKeyboardCallback(void (*func)(unsigned char, int, int));
void pufPointerMotionCallback(PUFwindow* window, void (*func)(float,float,float,float));

void pufCameraInit(PUFcamera* camera, float fov, float nearClip, float farClip);
void pufCameraTranslate(PUFcamera* camera, float X, float Y, float Z);
void pufCameraRotate(PUFcamera* camera, float angle, float vectorX, float vectorY, float vectorZ);
void pufCameraRotateEuler(PUFcamera* camera, float angleX, float angleY, float angleZ);
void pufCameraRotateEulerDegrees(PUFcamera* camera, float angleX, float angleY, float angleZ);

void pufMeshInit(PUFmesh* mesh);
void pufMeshShapeQuad(PUFmesh* mesh);
void pufMeshLoadOBJ(PUFmesh* mesh, char const* file);
void pufMeshBind(PUFmesh* mesh);
void pufMeshRender(PUFmesh* mesh, PUFcamera* camera, PUFshader* shader, PUFframebuffer* framebuffer);

void pufMeshTranslate(PUFmesh* mesh, float X, float Y, float Z);
void pufMeshRotate(PUFmesh* mesh, float angle, float vectorX, float vectorY, float vectorZ, PUF_ANGLE_UNITS units);
void pufMeshRotateEuler(PUFmesh* mesh, float angleX, float angleY, float angleZ, PUF_ANGLE_UNITS units);
void pufMeshRotateEulerDegrees(PUFmesh* mesh, float angleX, float angleY, float angleZ);
void pufMeshScale(PUFmesh* mesh, float X,float Y,float Z);

void pufMeshDestroy(PUFmesh* mesh);

void pufColorFromRGBA(PUFcolor* color, GLfloat R, GLfloat G, GLfloat B, GLfloat A);

void pufTextureLoadBMP(PUFtexture* texture, char const* file);
void pufTextureCreate(PUFtexture* texture, GLuint width, GLuint height);

void pufTexturePixelSet(PUFtexture* texture, GLuint x, GLuint y, PUFcolor* color);
PUFcolor pufTexturePixelGet(PUFtexture* texture, GLuint x, GLuint y);

void pufTextureClear(PUFtexture* texture);
void pufTextureUpdate(PUFtexture* texture);
void pufTextureBind(PUFtexture* texture);

void pufTextureDestroy(PUFtexture* texture);

void pufFramebufferInit(PUFframebuffer* framebuffer);
void pufFramebufferTexture(PUFframebuffer* framebuffer, PUFtexture* texture);
void pufFramebufferBindAndClear(PUFframebuffer* framebuffer);
void pufFramebufferUnbind();

// shaders.c
void pufShaderLoad(PUFshader* shader,char const* vertexShaderSourceFile,char const* fragmentShaderSourceFile); // loads shader from source files
void pufShaderCreate(PUFshader* shader,char const* vertexShaderSource,char const* fragmentShaderSource); // creates shader from code typed into function parameter

void pufShaderUniform1fSet(PUFshader*, const char *, float);
void pufShaderUniform3fSet(PUFshader*, const char *, float, float, float);

#endif
