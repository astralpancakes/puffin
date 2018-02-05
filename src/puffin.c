
#include <GL/glew.h>
#include <GL/glut.h>
#include "puffin.h"
#include "loadobj.h"
#include "matrix.h"
#include "helpers.h"
#include <stdio.h>
#include <math.h>

// special function prototypes
void draw();
void setup();
void keyboard(unsigned char key, int specialKey, int x, int y);

// prototypes for GLUT callbacks registered in pufInit
void pufKeyboard(unsigned char key, int x, int y);
void pufSpecialKeys(int specialkey, int x, int y);
void pufWindowResize(int, int);

PUFvector pufVectorFromAngle(double pitch, double yaw, PUF_ANGLE_UNITS units)
{

    if (units == DEGREES)
    {
        pitch = pitch * M_PI / 180.0f;
        yaw = yaw * M_PI / 180.0f;
    }

    PUFvector vector;

    vector.x = cos(pitch)*sin(yaw);
    vector.y = sin(pitch);
    vector.z = cos(yaw)*cos(pitch);
    vector.w = 0.0;

    return vector;
}

PUFwindow pufInit(int windowWidth, int windowHeight, int framerate, const char* windowTitle)
{
    PUFwindow window;
	if (windowHeight == 0)	// prevent divide by zero etc
        windowHeight = 1;
    window.windowHeight = windowHeight;
    window.windowWidth = windowWidth;
    window.frameRate = framerate;
	int arghc = 0;
    char* arghv;
    glutInit(&arghc, &arghv);
    
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glEnable(GL_MULTISAMPLE);
    glutInitWindowSize(windowWidth, windowHeight);
    window.windowID = glutCreateWindow(windowTitle);
    
    if (window.windowID == 1) // special functions will apply to the first created window
    {
        glutDisplayFunc(draw);
        glutIdleFunc(NULL);
        glutReshapeFunc(pufWindowResize);
        glutKeyboardFunc(pufKeyboard);
        glutSpecialFunc(pufSpecialKeys);
        //glutFullScreen();
    }

    glewInit();
	
	window.glMajorVersion = atoi((const char *)&glGetString(GL_VERSION)[0]);
	window.glMajorVersion = atoi((const char *)&glGetString(GL_VERSION)[2]); 
/* 
    window->frameCounter = 0;
    window->frameDelay = 0;    
    window->frameTick = 0;
*/
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

    return window;
}

void pufKeyboard(unsigned char key, int x, int y)
{
    keyboard(key,0,x,y);
}

void pufSpecialKeys(int specialKey, int x, int y)
{
    keyboard(0,specialKey,x,y);
}

void pufRun()
{
    setup();
    glutMainLoop();
}

void pufWindowResize(int windowWidth, int windowHeight)
{
}

void pufRedisplayTimer(int id)
{
    glutPostRedisplay();
}

void pufUpdate(PUFwindow* window)
{
    glutSetWindow(window->windowID);
    glutSwapBuffers();
    
    //
    double delay = 1000.0/(double)window->frameRate; // set delay to 1000ms divided by the window framerate
    //glutTimerFunc(delay, pufRedisplayTimer, window->windowID); // set a timer to call glutPostRedisplay on window synced to framerate
    //
    glutPostRedisplay();
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    
    
    if (window->frameRate > 0)
    {
        if (window->frameTick == 0)
        {
            window->frameTick = glutGet(GLUT_ELAPSED_TIME);
        }
        
        int maxDelay = (int)(1000.0f/(float)window->frameRate+0.5f);
        unsigned int delay = (unsigned int)(window->frameTick + (1000/window->frameRate) - glutGet(GLUT_ELAPSED_TIME));
        while(delay > 0 && delay < maxDelay)
            delay = (unsigned int)(window->frameTick + (1000/window->frameRate) - glutGet(GLUT_ELAPSED_TIME));
        window->frameTick = glutGet(GLUT_ELAPSED_TIME);
    }
    ++window->frameCounter;
    
     
}

void pufCameraInit(PUFwindow* window, PUFcamera* camera, float fov, float nearClip, float farClip)
{
    camera->window = window;
    camera->width = window->windowWidth;
    camera->height = window->windowHeight;
    glViewport(0,0, camera->width,camera->height);
	glShadeModel(GL_SMOOTH);
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    if (fov == 0.0)
        pufMatrixProjectOrtho(camera->width, camera->height, nearClip, farClip, camera->projectionMatrix);
    else
        pufMatrixProjectPersp(fov, camera->width, camera->height, nearClip, farClip, camera->projectionMatrix);
    
    for (int i=0;i < 15; i++)
		camera->cameraMatrix[i] = 0.0f;
    
    camera->cameraMatrix[0] = 1.0f;
    camera->cameraMatrix[5] = 1.0f;
    camera->cameraMatrix[10] = 1.0f;
    camera->cameraMatrix[15] = 1.0f;
	
	for (int i=0;i < 15; i++)
		camera->translationMatrix[i] = 0.0f;
    
    camera->translationMatrix[0] = 1.0f;
    camera->translationMatrix[5] = 1.0f;
    camera->translationMatrix[10] = 1.0f;
    camera->translationMatrix[15] = 1.0f;

	for (int i=0; i<3; i++)
		camera->cameraTranslation[i] = 0.0f;
	for (int i=0; i<4; i++)
		camera->cameraRotation[i] = 0.0f;
	camera->cameraRotation[3] = 1.0f;
}

void pufCameraTranslate(PUFcamera* camera, float X, float Y, float Z)
{
    /*
	GLfloat tempCameraMatrix[16];
	GLfloat tempMatrix[16];
	pufMatrixMult(pufMatrixTranslate(-vectorX,-vectorY,-vectorZ,tempMatrix),memcpy(tempCameraMatrix,camera->cameraMatrix,sizeof(GLfloat)*16),camera->cameraMatrix);
    */
    camera->cameraTranslation[0] += X;
    camera->cameraTranslation[1] += Y;
    camera->cameraTranslation[2] += Z;
}

void pufCameraRotate(PUFcamera* camera, float angle, float vectorX, float vectorY, float vectorZ)
{
    if (angle != 0.0)
	{
		if (vectorX != 0.0 || vectorY != 0.0 || vectorZ != 0.0)
		{
			float magnitude;
			float normalizedX;
			float normalizedY;
			float normalizedZ;
			

			//normalize vector
			magnitude = sqrt((vectorX*vectorX) + (vectorY*vectorY) + (vectorZ*vectorZ));
			normalizedX = vectorX/magnitude;
			normalizedY = vectorY/magnitude;
			normalizedZ = vectorZ/magnitude;

			//convert euler angle to quaternion rotation
			float sinAngle = sin(angle/2.0);
			float cosAngle = cos(angle/2.0);
			
			float x1 = normalizedX * sinAngle;
			float y1 = normalizedY * sinAngle;
			float z1 = normalizedZ * sinAngle;
			float w1 = cosAngle;

			//get current rotation
			float x2 = camera->cameraRotation[0];
			float y2 = camera->cameraRotation[1];
			float z2 = camera->cameraRotation[2];
			float w2 = camera->cameraRotation[3];

			//multiply rotations
			float x = (w1*x2) + (x1*w2) + (y1*z2) - (z1*y2);
			float y = (w1*y2) + (y1*w2) + (z1*x2) - (x1*z2);
			float z = (w1*z2) + (z1*w2) + (x1*y2) - (y1*x2);
			float w = (w1*w2) - (x1*x2) - (y1*y2) - (z1*z2);
			
			//normalize and set result
			magnitude = sqrt((x*x) + (y*y) + (z*z) + (w*w));
			camera->cameraRotation[0] = x/magnitude;
			camera->cameraRotation[1] = y/magnitude;
			camera->cameraRotation[2] = z/magnitude;
			camera->cameraRotation[3] = w/magnitude;	
		}
	}
    /*
    GLfloat tempCameraMatrix[16];
	GLfloat tempMatrix[16];

    pufMatrixMult(memcpy(tempCameraMatrix,camera->cameraMatrix,sizeof(GLfloat)*16),pufMatrixRotate(angle,-vectorX,-vectorY,-vectorZ,tempMatrix),camera->cameraMatrix);
    */
}

void pufCameraRotateEuler(PUFcamera* camera, float angleX, float angleY, float angleZ) //rotates Puffin mesh by a specified (in radians) XYZ (Euler) angle
{

    //convert normalized angle to quaternion rotation
    double t0 = cos(angleZ * 0.5);
	double t1 = sin(angleZ * 0.5);
	double t2 = cos(angleX * 0.5);
	double t3 = sin(angleX * 0.5);
	double t4 = cos(angleY * 0.5);
	double t5 = sin(angleY * 0.5);

    double x1 = t0 * t3 * t4 - t1 * t2 * t5;
	double y1 = t0 * t2 * t5 + t1 * t3 * t4;
	double z1 = t1 * t2 * t4 - t0 * t3 * t5;
	double w1 = t0 * t2 * t4 + t1 * t3 * t5;

	//get current rotation
    float x2 = camera->cameraRotation[0];
    float y2 = camera->cameraRotation[1];
    float z2 = camera->cameraRotation[2];
    float w2 = camera->cameraRotation[3];

    //multiply rotations
    float x = (w1*x2) + (x1*w2) + (y1*z2) - (z1*y2);
    float y = (w1*y2) + (y1*w2) + (z1*x2) - (x1*z2);
    float z = (w1*z2) + (z1*w2) + (x1*y2) - (y1*x2);
    float w = (w1*w2) - (x1*x2) - (y1*y2) - (z1*z2);
    
    //normalize and set result
    float magnitude = sqrt((x*x) + (y*y) + (z*z) + (w*w));
    camera->cameraRotation[0] = x/magnitude;
    camera->cameraRotation[1] = y/magnitude;
    camera->cameraRotation[2] = z/magnitude;
    camera->cameraRotation[3] = w/magnitude;    

}

void pufCameraRotateEulerDegrees(PUFcamera* camera, float angleX, float angleY, float angleZ) //rotates Puffin mesh by a specified (in degrees) XYZ (Euler) angle
{
    

    //convert angles to radians
    angleX = angleX * M_PI / 180.0f;
    angleY = angleY * M_PI / 180.0f;
    angleZ = angleZ * M_PI / 180.0f;

    //convert angles to quaternion rotation
    double t0 = cos(angleZ * 0.5);
	double t1 = sin(angleZ * 0.5);
	double t2 = cos(angleX * 0.5);
	double t3 = sin(angleX * 0.5);
	double t4 = cos(angleY * 0.5);
	double t5 = sin(angleY * 0.5);

    double x1 = t0 * t3 * t4 - t1 * t2 * t5;
	double y1 = t0 * t2 * t5 + t1 * t3 * t4;
	double z1 = t1 * t2 * t4 - t0 * t3 * t5;
	double w1 = t0 * t2 * t4 + t1 * t3 * t5;

	//get current rotation
    float x2 = camera->cameraRotation[0];
    float y2 = camera->cameraRotation[1];
    float z2 = camera->cameraRotation[2];
    float w2 = camera->cameraRotation[3];

    //multiply new and current rotations
    float x = (w1*x2) + (x1*w2) + (y1*z2) - (z1*y2);
    float y = (w1*y2) + (y1*w2) + (z1*x2) - (x1*z2);
    float z = (w1*z2) + (z1*w2) + (x1*y2) - (y1*x2);
    float w = (w1*w2) - (x1*x2) - (y1*y2) - (z1*z2);
    
    //normalize and set result
    float magnitude = sqrt((x*x) + (y*y) + (z*z) + (w*w));
    camera->cameraRotation[0] = x/magnitude;
    camera->cameraRotation[1] = y/magnitude;
    camera->cameraRotation[2] = z/magnitude;
    camera->cameraRotation[3] = w/magnitude;    

}


void pufMeshInit(PUFmesh* mesh) //zero the transformations of the mesh
{
	for (int i=0; i<3; i++)
		mesh->meshScale[i] = 1.0f;
	for (int i=0; i<3; i++)
		mesh->meshTranslation[i] = 0.0f;
	for (int i=0; i<4; i++)
		mesh->meshRotation[i] = 0.0f;
	mesh->meshRotation[3] = 1.0f;
/*
	for (int i=0;i < 15; i++)
		mesh->modelView[i] = 0.0f;
    mesh->modelView[0] = 1.0f;
    mesh->modelView[5] = 1.0f;
    mesh->modelView[10] = 1.0f;
    mesh->modelView[15] = 1.0f;
*/    
}

void pufMeshShapeQuad(PUFmesh* mesh) //generates a nicely generic quad
{
	mesh->vertexCount = 6;
	mesh->verts = malloc(sizeof(PUFvertex)*mesh->vertexCount);
	mesh->verts[0].position[0] = -1.0f;
	mesh->verts[0].position[1] = 1.0f;
	mesh->verts[0].position[2] = 0.0f;
	mesh->verts[0].texture[0] = 0.0f;
	mesh->verts[0].texture[1] = 1.0f;
	mesh->verts[0].texture[2] = 0.0f;
	
	mesh->verts[1].position[0] = -1.0f;
	mesh->verts[1].position[1] = -1.0f;
	mesh->verts[1].position[2] = 0.0f;
	mesh->verts[1].texture[0] = 0.0f;
	mesh->verts[1].texture[1] = 0.0f;
	mesh->verts[1].texture[2] = 0.0f;
	
	mesh->verts[2].position[0] = 1.0f;
	mesh->verts[2].position[1] = -1.0f;
	mesh->verts[2].position[2] = 0.0f;
	mesh->verts[2].texture[0] = 1.0f;
	mesh->verts[2].texture[1] = 0.0f;
	mesh->verts[2].texture[2] = 0.0f;
	
	mesh->verts[3].position[0] = -1.0f;
	mesh->verts[3].position[1] = 1.0f;
	mesh->verts[3].position[2] = 0.0f;
	mesh->verts[3].texture[0] = 0.0f;
	mesh->verts[3].texture[1] = 1.0f;
	mesh->verts[3].texture[2] = 0.0f;
	
	mesh->verts[4].position[0] = 1.0f;
	mesh->verts[4].position[1] = -1.0f;
	mesh->verts[4].position[2] = 0.0f;
	mesh->verts[4].texture[0] = 1.0f;
	mesh->verts[4].texture[1] = 0.0f;
	mesh->verts[4].texture[2] = 0.0f;
	
	mesh->verts[5].position[0] = 1.0f;
	mesh->verts[5].position[1] = 1.0f;
	mesh->verts[5].position[2] = 0.0f;
	mesh->verts[5].texture[0] = 1.0f;
	mesh->verts[5].texture[1] = 1.0f;
	mesh->verts[5].texture[2] = 0.0f;
	
	for (int i=0;i<mesh->vertexCount;++i)
	{
		mesh->verts[i].color[0] = ((float)(rand() % 100))/100;
        mesh->verts[i].color[1] = (float)(rand() % 100)/100;
        mesh->verts[i].color[2] = (float)(rand() % 100)/100;
        mesh->verts[i].normal[0] = 0.0f;
        mesh->verts[i].normal[1] = 0.0f;
        mesh->verts[i].normal[2] = 1.0f;
	}
}

void pufMeshLoadOBJ(PUFmesh* mesh, char const* file) //loads an OBJ file into Puffin mesh
{
    GLfloat* obj = pufLoadOBJ(file, &mesh->vertexCount);
    mesh->verts = malloc(sizeof(PUFvertex)*mesh->vertexCount);

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

void pufMeshBind(PUFmesh* mesh) //uploads Puffin mesh vertex buffer object
{
	glGenBuffers(1, &mesh->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PUFvertex)*mesh->vertexCount, NULL,GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(PUFvertex)*mesh->vertexCount, mesh->verts);	
}

void pufMeshRender(PUFmesh* mesh, PUFcamera* camera, PUFshader* shader) //draws Puffin mesh
{
    glUseProgram(shader->shaderProgram);
		
    GLint uniformModelviewProjectionMatrix = glGetUniformLocation(shader->shaderProgram, "modelviewProjectionMatrix");
    GLint uniformModelMatrix = glGetUniformLocation(shader->shaderProgram, "modelMatrix");
    GLint uniformModelViewMatrix = glGetUniformLocation(shader->shaderProgram, "modelViewMatrix");
    GLint uniformRenderTargetWidth = glGetUniformLocation(shader->shaderProgram, "renderTargetWidth");
    GLint uniformRenderTargetHeight = glGetUniformLocation(shader->shaderProgram, "renderTargetHeight");
    GLint uniformRenderTargetSize = glGetUniformLocation(shader->shaderProgram, "renderTargetSize");
    GLint uniformTime = glGetUniformLocation(shader->shaderProgram, "time");

    GLfloat modelMatrix[16];
    GLfloat viewMatrix[16];
    GLfloat modelviewMatrix[16];

    GLfloat modelviewProjectionMatrix[16];
    //GLfloat modelviewCameraMatrix[16];
	
	//GLfloat tempModelView[16];

	GLfloat tempMatrix[16];
    GLfloat translationMatrix[16];
    GLfloat scalingMatrix[16];
    GLfloat rotationMatrix[16];
/*
	for (int i=0;i < 15; i++)
		mesh->modelView[i] = 0.0f;
	
    mesh->modelView[0] = 1.0f;
    mesh->modelView[5] = 1.0f;
    mesh->modelView[10] = 1.0f;
    mesh->modelView[15] = 1.0f;
*/

// TODO: The following could be made clearer by calculating the matrices and multiplying them together as separate steps. Consider making the pufMatrix* functions not return anything since they need the return value pointer passed as an argument anyway.

// Calculate model matrix

    pufMatrixMult(
        pufMatrixTranslate(mesh->meshTranslation[0],mesh->meshTranslation[1],mesh->meshTranslation[2],translationMatrix),
        pufMatrixMult(
            pufMatrixScale(mesh->meshScale[0],mesh->meshScale[1],mesh->meshScale[2],scalingMatrix),
            pufMatrixFromQuaternion(mesh->meshRotation[0],mesh->meshRotation[1],mesh->meshRotation[2],mesh->meshRotation[3],rotationMatrix),
            tempMatrix
        ),
        modelMatrix
    );

    pufMatrixMult(
        pufMatrixTranslate(-camera->cameraTranslation[0], -camera->cameraTranslation[1], -camera->cameraTranslation[2], translationMatrix),
        pufMatrixFromQuaternion(-camera->cameraRotation[0], -camera->cameraRotation[1],-camera->cameraRotation[2], -camera->cameraRotation[3], rotationMatrix),
        viewMatrix
    );

    pufMatrixMult(viewMatrix, modelMatrix, modelviewMatrix);
    pufMatrixMult(camera->projectionMatrix, modelviewMatrix, modelviewProjectionMatrix);
/*
	pufMatrixMult(
        pufMatrixScale(mesh->meshScale[0],mesh->meshScale[1],mesh->meshScale[2],tempMatrix),
        memcpy(tempModelView,mesh->modelView,sizeof(GLfloat)*16),
        mesh->modelView);
	pufMatrixMult(pufMatrixFromQuaternion(mesh->meshRotation[0],mesh->meshRotation[1],mesh->meshRotation[2],mesh->meshRotation[3],tempMatrix),memcpy(tempModelView,mesh->modelView,sizeof(GLfloat)*16),mesh->modelView);
	pufMatrixMult(pufMatrixTranslate(mesh->meshTranslation[0],mesh->meshTranslation[1],mesh->meshTranslation[2],tempMatrix),memcpy(tempModelView,mesh->modelView,sizeof(GLfloat)*16),mesh->modelView);
	
	pufMatrixMult(camera->cameraMatrix, mesh->modelView, modelviewCameraMatrix);

    pufMatrixMult(camera->projectionMatrix, modelviewCameraMatrix, modelviewProjectionMatrix);
*/
    glUniformMatrix4fv(uniformModelviewProjectionMatrix, 1, GL_FALSE, modelviewProjectionMatrix);
    glUniformMatrix4fv(uniformModelViewMatrix, 1, GL_FALSE, modelviewMatrix);
    glUniformMatrix4fv(uniformModelMatrix, 1, GL_FALSE, modelMatrix);
    
	glUniform1f(uniformRenderTargetWidth, (float)camera->width);
    glUniform1f(uniformRenderTargetHeight, (float)camera->height);
    glUniform2f(uniformRenderTargetSize, (float)camera->width, (float)camera->height);
    glUniform1f(uniformTime, (float)glutGet(GLUT_ELAPSED_TIME));

    /*
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
	*/
	
	glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
	
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	
	
    glVertexAttribPointer(0, 3, GL_FLOAT, 1, sizeof(PUFvertex), (void*)0); // vertex position
    glVertexAttribPointer(1, 3, GL_FLOAT, 0, sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*3)); // vertex color
    glVertexAttribPointer(2, 2, GL_FLOAT, 0 , sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*6)); // vertex texture coordinates (UV)
    glVertexAttribPointer(3, 3, GL_FLOAT, 0, sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*9)); // vertex normal
    glVertexAttribPointer(4, 3, GL_FLOAT, 0, sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*12)); // vertex data 3f (normally unused)
    glVertexAttribPointer(5, 1, GL_FLOAT, 0, sizeof(PUFvertex), ((char*)NULL) + (sizeof(GLfloat)*15)); // vertex data 1f (normally unused)
	
    glDrawArrays(GL_TRIANGLES,0,mesh->vertexCount);
		
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	/*
    glDisableVertexAttribArray(vertexMeta1f);
    glDisableVertexAttribArray(vertexMeta3f);
    glDisableVertexAttribArray(vertexNormal);
    glDisableVertexAttribArray(vertexTexture);
    glDisableVertexAttribArray(vertexColor);
    glDisableVertexAttribArray(vertexPosition);
    */
	glUseProgram(0);
	

}

void pufMeshTranslate(PUFmesh* mesh, float X, float Y, float Z) //translates Puffin mesh
{
	mesh->meshTranslation[0] += X;
	mesh->meshTranslation[1] += Y;
	mesh->meshTranslation[2] += Z;
	//copy the existing mesh modelview matrix into tempModelView, ask for a new matrix for tempMatrix, then multiply them and put into mesh modelview
	//GLfloat tempModelView[16];
	//GLfloat tempMatrix[16];
	//pufMatrixMult(pufMatrixTranslate(X,Y,Z,tempMatrix),memcpy(tempModelView,mesh->modelView,sizeof(GLfloat)*16),mesh->modelView);
}

void pufMeshRotateEuler(PUFmesh* mesh, float angleX, float angleY, float angleZ, PUF_ANGLE_UNITS units) //rotates Puffin mesh by a specified (in radians) XYZ (Euler) angle
{

    //convert angles to radians
    if (units == DEGREES)
    {
        angleX = angleX * M_PI / 180.0f;
        angleY = angleY * M_PI / 180.0f;
        angleZ = angleZ * M_PI / 180.0f;
    }


	//normalize angle
	//float magnitude = sqrt((angleX*angleX) + (angleY*angleY) + (angleZ*angleZ));
	//angleX = angleX/magnitude;
	//angleY = angleY/magnitude;
	//angleZ = angleZ/magnitude;

    //convert normalized angle to quaternion rotation
    double t0 = cos(angleZ * 0.5);
	double t1 = sin(angleZ * 0.5);
	double t2 = cos(angleX * 0.5);
	double t3 = sin(angleX * 0.5);
	double t4 = cos(angleY * 0.5);
	double t5 = sin(angleY * 0.5);

    double x1 = t0 * t3 * t4 - t1 * t2 * t5;
	double y1 = t0 * t2 * t5 + t1 * t3 * t4;
	double z1 = t1 * t2 * t4 - t0 * t3 * t5;
	double w1 = t0 * t2 * t4 + t1 * t3 * t5;

	//get current rotation
    float x2 = mesh->meshRotation[0];
    float y2 = mesh->meshRotation[1];
    float z2 = mesh->meshRotation[2];
    float w2 = mesh->meshRotation[3];

    //multiply rotations
    float x = (w1*x2) + (x1*w2) + (y1*z2) - (z1*y2);
    float y = (w1*y2) + (y1*w2) + (z1*x2) - (x1*z2);
    float z = (w1*z2) + (z1*w2) + (x1*y2) - (y1*x2);
    float w = (w1*w2) - (x1*x2) - (y1*y2) - (z1*z2);
    
    //normalize and set result
    float magnitude = sqrt((x*x) + (y*y) + (z*z) + (w*w));
    mesh->meshRotation[0] = x/magnitude;
    mesh->meshRotation[1] = y/magnitude;
    mesh->meshRotation[2] = z/magnitude;
    mesh->meshRotation[3] = w/magnitude;    

}

void pufMeshRotateEulerDegrees(PUFmesh* mesh, float angleX, float angleY, float angleZ) //rotates Puffin mesh by a specified (in degrees) XYZ (Euler) angle
{
    //convert angles to radians
    angleX = angleX * M_PI / 180.0f;
    angleY = angleY * M_PI / 180.0f;
    angleZ = angleZ * M_PI / 180.0f;

    //convert angles to quaternion rotation
    double t0 = cos(angleZ * 0.5);
	double t1 = sin(angleZ * 0.5);
	double t2 = cos(angleX * 0.5);
	double t3 = sin(angleX * 0.5);
	double t4 = cos(angleY * 0.5);
	double t5 = sin(angleY * 0.5);

    double x1 = t0 * t3 * t4 - t1 * t2 * t5;
	double y1 = t0 * t2 * t5 + t1 * t3 * t4;
	double z1 = t1 * t2 * t4 - t0 * t3 * t5;
	double w1 = t0 * t2 * t4 + t1 * t3 * t5;

	//get current rotation
    float x2 = mesh->meshRotation[0];
    float y2 = mesh->meshRotation[1];
    float z2 = mesh->meshRotation[2];
    float w2 = mesh->meshRotation[3];

    //multiply new and current rotations
    float x = (w1*x2) + (x1*w2) + (y1*z2) - (z1*y2);
    float y = (w1*y2) + (y1*w2) + (z1*x2) - (x1*z2);
    float z = (w1*z2) + (z1*w2) + (x1*y2) - (y1*x2);
    float w = (w1*w2) - (x1*x2) - (y1*y2) - (z1*z2);
    
    //normalize and set result
    float magnitude = sqrt((x*x) + (y*y) + (z*z) + (w*w));
    mesh->meshRotation[0] = x/magnitude;
    mesh->meshRotation[1] = y/magnitude;
    mesh->meshRotation[2] = z/magnitude;
    mesh->meshRotation[3] = w/magnitude;    

}

void pufMeshRotate(PUFmesh* mesh, float angle, float vectorX, float vectorY, float vectorZ, PUF_ANGLE_UNITS units) //rotates Puffin mesh about vector by angle
{

	if (angle != 0.0)
	{
            //convert angles to radians
        if (units == DEGREES)
        {
            angle = angle * M_PI / 180.0f;
        }

		if (vectorX != 0.0 || vectorY != 0.0 || vectorZ != 0.0)
		{
			float magnitude;
			float normalizedX;
			float normalizedY;
			float normalizedZ;
			

			//normalize vector
			magnitude = sqrt((vectorX*vectorX) + (vectorY*vectorY) + (vectorZ*vectorZ));
			normalizedX = vectorX/magnitude;
			normalizedY = vectorY/magnitude;
			normalizedZ = vectorZ/magnitude;

			//convert euler angle to quaternion rotation
			float sinAngle = sin(angle/2.0);
			float cosAngle = cos(angle/2.0);
			
			float x1 = normalizedX * sinAngle;
			float y1 = normalizedY * sinAngle;
			float z1 = normalizedZ * sinAngle;
			float w1 = cosAngle;

			//get current rotation
			float x2 = mesh->meshRotation[0];
			float y2 = mesh->meshRotation[1];
			float z2 = mesh->meshRotation[2];
			float w2 = mesh->meshRotation[3];

			//multiply rotations
			float x = (w1*x2) + (x1*w2) + (y1*z2) - (z1*y2);
			float y = (w1*y2) + (y1*w2) + (z1*x2) - (x1*z2);
			float z = (w1*z2) + (z1*w2) + (x1*y2) - (y1*x2);
			float w = (w1*w2) - (x1*x2) - (y1*y2) - (z1*z2);
			
			//normalize and set result
			magnitude = sqrt((x*x) + (y*y) + (z*z) + (w*w));
			mesh->meshRotation[0] = x/magnitude;
			mesh->meshRotation[1] = y/magnitude;
			mesh->meshRotation[2] = z/magnitude;
			mesh->meshRotation[3] = w/magnitude;	
		}
	}
	/*
	// set rotation directly
    
	//normalize vector
    float magnitude = sqrt((vectorX*vectorX) + (vectorY*vectorY) + (vectorZ*vectorZ));
    float X = vectorX/magnitude;
    float Y = vectorY/magnitude;
    float Z = vectorZ/magnitude;
	
	float sinAngle = sin(angle/2.0);
	float cosAngle = cos(angle/2.0);
	
	mesh->meshRotation[0] = X * sinAngle;
	mesh->meshRotation[1] = Y * sinAngle;
	mesh->meshRotation[2] = Z * sinAngle;
	mesh->meshRotation[3] = cosAngle;	
	*/
	
	//tweak matrix
    /*
	GLfloat tempModelView[16];
	GLfloat tempMatrix[16];
    pufMatrixMult(memcpy(tempModelView,mesh->modelView,sizeof(GLfloat)*16),pufMatrixRotate(angle,vectorX,vectorY,vectorZ,tempMatrix),mesh->modelView);
	*/
}

void pufMeshScale(PUFmesh* mesh, float X, float Y, float Z) //scales Puffin mesh
{
	mesh->meshScale[0] = X;
	mesh->meshScale[1] = Y;
	mesh->meshScale[2] = Z;
	/*
	GLfloat tempModelView[16];
	GLfloat tempMatrix[16];
    pufMatrixMult(pufMatrixScale(X,Y,Z,tempMatrix),memcpy(tempModelView,mesh->modelView,sizeof(GLfloat)*16),mesh->modelView);
	*/
}

void pufMeshDestroy(PUFmesh* mesh) //deletes Puffin mesh
{
    free((void*)mesh->verts);
}

void pufShaderLoad(PUFshader* shader, char const* vertexShaderSourceFile, char const* fragmentShaderSourceFile) //loads shader from GLSL source files, compiles and links shader program
{

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		const GLchar* vertexShaderSource = pufReadFile(vertexShaderSourceFile);
		glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
		glCompileShader(vertexShader);
		free((void*)vertexShaderSource);
		
		
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		
		const GLchar* fragmentShaderSource = pufReadFile(fragmentShaderSourceFile);
		glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
		glCompileShader(fragmentShader);
		free((void*)fragmentShaderSource);
		
		shader->shaderProgram = glCreateProgram();
		glAttachShader(shader->shaderProgram,vertexShader);
		glAttachShader(shader->shaderProgram,fragmentShader);
		glLinkProgram(shader->shaderProgram);
	

} 

void pufShaderCreate(PUFshader* shader, char const* vertexShaderSource, char const* fragmentShaderSource)
{

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	
	glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
	glCompileShader(vertexShader);
	
	glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
	glCompileShader(fragmentShader);
	
	shader->shaderProgram = glCreateProgram();
	glAttachShader(shader->shaderProgram,vertexShader);
	glAttachShader(shader->shaderProgram,fragmentShader);
	glLinkProgram(shader->shaderProgram);
	
}

void pufColorFromRGBA(PUFcolor* color, GLfloat R, GLfloat G, GLfloat B, GLfloat A)
{
    if (R < 0.0f)
        R = 0.0f;
    if (G < 0.0f)
        G = 0.0f;
    if (B < 0.0f)
        B = 0.0f;
    if (A < 0.0f)
        A = 0.0f;
    color->R = R;
    color->G = G;
    color->B = B;
    color->A = A;
}

void pufTextureLoadBMP(PUFtexture* texture, char const* file) //loads BMP file into Puffin texture, binds the pixel buffer object and loads into OpenGL texture map
{
    glGenTextures(1, &texture->textureId);
    
    
    FILE* img = NULL;
    if ((img = fopen(file,"rb")))
    {
       
        int pixelDataStartingOffset;
        GLshort pixelBits;
        fseek(img,10,SEEK_SET);
        fread(&pixelDataStartingOffset,4,1,img);
        fseek(img,4,SEEK_CUR);
        fread(&texture->width,4,1,img);
        fread(&texture->height,4,1,img);
        fseek(img,2,SEEK_CUR);
        fread(&pixelBits,2,1,img);
        GLint pixelBytes = pixelBits/8;
        
        if (pixelBytes != 4 & pixelBytes != 3)
        {
            printf("Puffin BMP loader found that %s does not use 32 or 24 bits per pixel. This wasn't expected.\n", file);
            printf("Bytes per pixel: %li\n",pixelBytes);
        }
		/*
        printf("Puffin BMP loader opened file %s\n",file);
        printf("Pixel data start offset is %i...\n", pixelDataStartingOffset);
        printf("Width is %i...\n", texture->width);
        printf("Height is %i...\n", texture->height);
        printf("Number of bits per pixel is %i...\n", pixelBits);
        printf("Number of bytes per pixel is %i...\n", pixelBytes);
        printf("Allocating memory for %i pixels of %i bytes each...\n",texture->height*texture->width, pixelBytes);
		*/
		
        GLubyte *tempBuffer = (GLubyte*)malloc(texture->height*texture->width*pixelBytes*sizeof(GL_UNSIGNED_BYTE));
        texture->pixels = (GLfloat*)malloc(texture->height*texture->width*4*sizeof(GL_FLOAT));
        
        fseek(img,pixelDataStartingOffset,SEEK_SET);	// start reading image data
        for(int i = 0;i<texture->height;i++) // for each row...
        {
            for(int j = 0;j<texture->width;j++) // for each pixel...
                fread(tempBuffer+(i*texture->width*pixelBytes+j*pixelBytes),pixelBytes,1,img); // read it...
            if ((texture->width*pixelBytes) % 4 != 0)  // BMP rows are stored on four byte alignments, so hop ahead after each row if needed
                fseek(img,4 - ((texture->width*pixelBytes) % 4), SEEK_CUR);
        }
        
        fclose(img);
        

        for (int i=0; i < texture->width*texture->height; i++)
        {
        /* BMP files are stored with the pixel data as ABGR (32 bits per pixel) or BGR (24 bits per pixel). 
        We want puffin textures to be RGBA, so we do some twiddling when we copy the pixel data.
        
        If you want to play around with this, note that Photoshop does not seem to save BMP files with alpha channels properly. Use The GIMP or something...*/

            if(pixelBytes == 4)
            {
                texture->pixels[i*4] = (GLfloat)tempBuffer[i*pixelBytes+3]/255.0f;
                texture->pixels[i*4+1] = (GLfloat)tempBuffer[i*pixelBytes+2]/255.0f;
                texture->pixels[i*4+2] = (GLfloat)tempBuffer[i*pixelBytes+1]/255.0f;
                texture->pixels[i*4+3] = (GLfloat)tempBuffer[i*pixelBytes]/255.0f;
            }
			if(pixelBytes == 3)
            {
                texture->pixels[i*4] = (GLfloat)tempBuffer[i*pixelBytes+2]/255.0f;
                texture->pixels[i*4+1] = (GLfloat)tempBuffer[i*pixelBytes+1]/255.0f;
                texture->pixels[i*4+2] = (GLfloat)tempBuffer[i*pixelBytes]/255.0f;
                texture->pixels[i*4+3] = 1.0f;
            }

        }
        free((void*)tempBuffer);
        
    }
    else // failed to open file, let's make the texture solid yellow instead
    {
        printf("Puffin BMP loader failed to open file %s\n",file);
        texture->width = 1;
        texture->height = 1;
        texture->pixels = (GLfloat*)malloc(texture->height*texture->width*4*sizeof(GL_FLOAT));
        texture->pixels[0] = 0.0f;
		for (int i = 1;i<4;i++)
            texture->pixels[i] = 1.0f;
    }
    
     texture->textureFormat = GL_RGBA;

        
    glGenBuffers(1, &texture->pixelBuffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texture->pixelBuffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, texture->width*texture->height*4*sizeof(GL_FLOAT), NULL,GL_STATIC_DRAW);
    glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, texture->width*texture->height*4*sizeof(GL_FLOAT), texture->pixels);
        
    

    glBindTexture(GL_TEXTURE_2D, texture->textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,texture->width,texture->height,0,texture->textureFormat,GL_FLOAT,NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void pufTextureCreate(PUFtexture* texture, GLuint width, GLuint height) //creates blank Puffin texture of specified size
{	
    texture->textureFormat = GL_RGBA;
    texture->width = width;
    texture->height = height;
    
    texture->pixels = (GLfloat*)malloc(texture->width*texture->height*4*sizeof(GL_FLOAT));
    
    glGenBuffers(1, &texture->pixelBuffer);

    glGenTextures(1, &texture->textureId);
    glBindTexture(GL_TEXTURE_2D, texture->textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void pufTexturePixelSet(PUFtexture* texture, GLuint x, GLuint y, PUFcolor* color)
{
    if (color->R < 0.0f)
        color->R = 0.0f;
    if (color->G < 0.0f)
        color->G = 0.0f;
    if (color->B < 0.0f)
        color->B = 0.0f;
    if (color->A < 0.0f)
        color->A = 0.0f;
    if (texture->textureFormat == GL_BGRA)
    {
        texture->pixels[y*texture->width*4+x*4] = color->B;
        texture->pixels[y*texture->width*4+x*4+1] = color->G;
        texture->pixels[y*texture->width*4+x*4+2] = color->R;
        texture->pixels[y*texture->width*4+x*4+3] = color->A;
    }
    else
    {
        texture->pixels[y*texture->width*4+x*4] = color->R;
        texture->pixels[y*texture->width*4+x*4+1] = color->G;
        texture->pixels[y*texture->width*4+x*4+2] = color->B;
        texture->pixels[y*texture->width*4+x*4+3] = color->A;
    }
}

PUFcolor pufTexturePixelGet(PUFtexture* texture, GLuint x, GLuint y) 
{
    PUFcolor color;
    if (texture->textureFormat == GL_BGRA)
    {
        color.B = texture->pixels[y*texture->width*4+x*4];
        color.G = texture->pixels[y*texture->width*4+x*4+1];
        color.R = texture->pixels[y*texture->width*4+x*4+2];
        color.A =texture->pixels[y*texture->width*4+x*4+3];
    }
    else
    {
        color.R = texture->pixels[y*texture->width*4+x*4];
        color.G = texture->pixels[y*texture->width*4+x*4+1];
        color.B = texture->pixels[y*texture->width*4+x*4+2];
        color.A = texture->pixels[y*texture->width*4+x*4+3];
    }
    if (color.R < 0.0f)
        color.R = 0.0f;
    if (color.G < 0.0f)
        color.G = 0.0f;
    if (color.B < 0.0f)
        color.B = 0.0f;
    if (color.A < 0.0f)
        color.A = 0.0f;
    return color;
}

void pufTextureClear(PUFtexture* texture) //clears a Puffin texture
{
    memset(texture->pixels, 0, texture->width*texture->height*4*sizeof(GL_FLOAT));
}

void pufTextureUpdate(PUFtexture* texture) //binds pixel buffer object of Puffin texture, and updates OpenGL texture map 
{
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texture->pixelBuffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, texture->width*texture->height*4*sizeof(GL_FLOAT), NULL,GL_STATIC_DRAW);
    glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, texture->width*texture->height*4*sizeof(GL_FLOAT), texture->pixels);

    glBindTexture(GL_TEXTURE_2D, texture->textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1); 
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,texture->width,texture->height,0,texture->textureFormat,GL_FLOAT,NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void pufTextureBind(PUFtexture* texture) //binds Puffin texture for use
{
    glBindTexture(GL_TEXTURE_2D, texture->textureId);
}

void pufTextureDestroy(PUFtexture* texture) //deletes Puffin texture
{
    free((void*)texture->pixels);
}

void pufFramebufferInit(PUFframebuffer* framebuffer)
{
    glGenFramebuffers(1, &framebuffer->framebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->framebufferId);
    glGenRenderbuffers(1, &framebuffer->depthbufferId);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void pufFramebufferTexture(PUFframebuffer* framebuffer, PUFtexture* texture)
{
	//texture stuff
//    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture->textureId);
    glBindTexture(GL_TEXTURE_2D, texture->textureId);
    //glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,texture->width,texture->height,0,texture->textureFormat,GL_FLOAT,0); 
    //glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,1,GL_RGBA32F,texture->width,texture->height,0);

    
	//framebuffer stuff
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->framebufferId);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture->textureId, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->textureId, 0);
	//depth buffer stuff
	
    glBindRenderbuffer(GL_RENDERBUFFER,framebuffer->depthbufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texture->width, texture->height);
//    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, texture->width, texture->height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer->depthbufferId);
    
	//GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	//glDrawBuffers(1, DrawBuffers);
	
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("Alert!",0);
	
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void pufFramebufferBindAndClear(PUFframebuffer* framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->framebufferId);
    glBindRenderbuffer(GL_RENDERBUFFER,framebuffer->depthbufferId);
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
}

void pufFramebufferUnbind()
{
    glBindRenderbuffer(GL_RENDERBUFFER,0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
