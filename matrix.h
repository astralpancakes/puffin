#ifndef MATRIX_H
#define MATRIX_H

GLfloat* pfnMatrixTranslate(float, float, float);

GLfloat* pfnMatrixRotate(float ang, float X, float Y, float Z, bool degrees);

GLfloat* pfnMatrixScale(float X, float Y, float Z);


GLfloat* pfnMatrixProject(float fov, float aspect, float zNear, float zFar, bool degrees);


GLfloat* pfnMatrixMult(GLfloat* A,GLfloat* B,GLfloat* M);

GLfloat* pfnMatrixShrink(GLfloat* A,GLfloat* M);

#endif
