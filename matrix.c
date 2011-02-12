#include <stdbool.h>
#include <math.h>
#include <GL/glew.h>

GLfloat* pfnMatrixTranslate(float X, float Y, float Z)
{
    //The returned pointer is to static data which is overwritten with each call

    static GLfloat M[16];
//
    M[0] = 1;M[4] = 0;M[8] = 0;M[12] = X;
    M[1] = 0;M[5] = 1;M[9] = 0;M[13] = Y;
    M[2] = 0;M[6] = 0;M[10] = 1;M[14] = Z;
    M[3] = 0;M[7] = 0;M[11] = 0;M[15] = 1;

    return M;
}

GLfloat* pfnMatrixRotate(float ang, float X, float Y, float Z, bool degrees)
{

    if (degrees) //ang given as degrees
    {
            ang = ang * M_PI / 180.0f;
    }

    if (X+Y+Z != 0)
    {
        //normalize vector
        float magnitude = sqrt((X*X) + (Y*Y) + (Z*Z));
        X = X/magnitude;
        Y = Y/magnitude;
        Z = Z/magnitude;
    }
    else
    {
        ang = 0;
    }

    static GLfloat M[16];

    float c = cos(ang);
    float s = sin(ang);

    M[0] = pow(X,2.0f)*(1-c)+c;
    M[1] = Y*X*(1-c)+Z*s;
    M[2] = X*Z*(1-c)-Y*s;
    M[3] = 0;

    M[4] = X*Y*(1-c)-Z*s;
    M[5] = pow(Y,2.0f)*(1-c)+c;
    M[6] = Y*Z*(1-c)+X*s;
    M[7] = 0;

    M[8] = X*Z*(1-c)+Y*s;
    M[9] = Y*Z*(1-c)-X*s;
    M[10] = pow(Z,2.0f)*(1-c)+c;
    M[11] = 0;

    M[12] = 0;
    M[13] = 0;
    M[14] = 0;
    M[15] = 1;

    return M;
}

GLfloat* pfnMatrixScale(float X, float Y, float Z)
{
    static GLfloat M[16] = {0};

    M[0] = X;
    M[5] = Y;
    M[10] = Z;
    M[15] = 1;

    return M;
}

GLfloat* pfnMatrixProject(float fov, float aspect, float zNear, float zFar, bool degrees)
{
    if (degrees) //fov given as degrees
    {
            fov = fov * M_PI / 180.0f;
    }

    GLfloat f = 1.0f/tan(fov/2.0f);
    GLfloat negDepth = zNear-zFar;

    static GLfloat M[16] = {0};

    M[0] = f/aspect;
    M[5] = f;
    M[10] = (zFar + zNear)/negDepth;
    M[11] = -1;
    M[14] = (2.0f*zFar*zNear)/negDepth;

    return M;
}


GLfloat* pfnMatrixMult(GLfloat* A,GLfloat* B,GLfloat* M)
{
    for (int i=0; i<16; i+=4)
        for (int j=0; j<4; j++)
            M[i+j] = A[j]*B[i] + A[j+4]*B[i+1] + A[j+8]*B[i+2] + A[j+12]*B[i+3];
    return M;
}

GLfloat* pfnMatrixShrink(GLfloat* A,GLfloat* M)
{
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            M[i*3+j] = A[i*4+j];
    return M;
}

