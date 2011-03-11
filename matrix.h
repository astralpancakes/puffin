#ifndef MATRIX_H
#define MATRIX_H

GLfloat* pufMatrixTranslate(float, float, float, GLfloat*);

GLfloat* pufMatrixRotate(float ang, float X, float Y, float Z, bool degrees, GLfloat*);

GLfloat* pufMatrixScale(float X, float Y, float Z, GLfloat*);


GLfloat* pufMatrixProject(float fov, float width, float height, float zNear, float zFar, bool degrees, GLfloat* M);


GLfloat* pufMatrixMult(GLfloat* A,GLfloat* B,GLfloat* M);

GLfloat* pufMatrixShrink(GLfloat* A,GLfloat* M);

#endif
