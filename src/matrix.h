#ifndef MATRIX_H
#define MATRIX_H

GLfloat* pufMatrixTranslate(float, float, float, GLfloat*);

GLfloat* pufMatrixRotate(float ang, float X, float Y, float Z, GLfloat*);
GLfloat* pufMatrixFromQuaternion(float X, float Y, float Z, float W, GLfloat* M);
GLfloat* pufMatrixRotateEuler(float angleX, float angleY, float angleZ, GLfloat* M);

GLfloat* pufMatrixScale(float X, float Y, float Z, GLfloat*);


GLfloat* pufMatrixProjectPersp(float fov, float width, float height, float zNear, float zFar, GLfloat* M);
GLfloat* pufMatrixProjectOrtho(float width, float height, float zNear, float zFar, GLfloat* M);

GLfloat* pufMatrixMult(GLfloat* A,GLfloat* B,GLfloat* M);

GLfloat* pufMatrixShrink(GLfloat* A,GLfloat* M);

#endif
