PUFcamera pufCameraInit(float fov, float nearClip, float farClip)
{
    PUFcamera camera;

    //camera.window = window;
    //camera.width = window.windowWidth;
    //camera.height = window.windowHeight;
    //glViewport(0,0, camera.width,camera.height);

    /* set OpenGL stuff to something sensible */
	
	//glShadeModel(GL_SMOOTH);
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
    GLint viewportDims[4] = {0};
    glGetIntegerv(GL_VIEWPORT, viewportDims);
    int32_t viewportWidth = viewportDims[2];
    int32_t viewportHeight = viewportDims[3];
    
    if (fov == 0.0) // orthographic projection
        pufMatrixProjectOrtho(viewportWidth, viewportHeight, nearClip, farClip, camera.projectionMatrix);
    else // perspective projection
        pufMatrixProjectPersp(fov, viewportWidth, viewportHeight, nearClip, farClip, camera.projectionMatrix);
     
    for (int32_t i = 0; i < 15; i++)
		camera.cameraMatrix[i] = 0.0f;
    
    camera.cameraMatrix[0] = 1.0f;
    camera.cameraMatrix[5] = 1.0f;
    camera.cameraMatrix[10] = 1.0f;
    camera.cameraMatrix[15] = 1.0f;
	
	for (int32_t i = 0; i < 15; i++)
		camera.translationMatrix[i] = 0.0f;
    
    camera.translationMatrix[0] = 1.0f;
    camera.translationMatrix[5] = 1.0f;
    camera.translationMatrix[10] = 1.0f;
    camera.translationMatrix[15] = 1.0f;

	for (int32_t i = 0; i < 3; i++)
		camera.cameraTranslation[i] = 0.0f;
	for (int32_t i = 0; i < 4; i++)
		camera.cameraRotation[i] = 0.0f;
	camera.cameraRotation[3] = 1.0f;

    return camera;
}

void pufCameraTranslate(PUFcamera* camera, float X, float Y, float Z)
{
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

			// normalize vector
			magnitude = sqrt((vectorX*vectorX) + (vectorY*vectorY) + (vectorZ*vectorZ));
			normalizedX = vectorX/magnitude;
			normalizedY = vectorY/magnitude;
			normalizedZ = vectorZ/magnitude;

			// convert euler angle to quaternion rotation
			float sinAngle = sin(angle/2.0);
			float cosAngle = cos(angle/2.0);
			
			float x1 = normalizedX * sinAngle;
			float y1 = normalizedY * sinAngle;
			float z1 = normalizedZ * sinAngle;
			float w1 = cosAngle;

			// get current rotation
			float x2 = camera->cameraRotation[0];
			float y2 = camera->cameraRotation[1];
			float z2 = camera->cameraRotation[2];
			float w2 = camera->cameraRotation[3];

			// multiply rotations
			float x = (w1*x2) + (x1*w2) + (y1*z2) - (z1*y2);
			float y = (w1*y2) + (y1*w2) + (z1*x2) - (x1*z2);
			float z = (w1*z2) + (z1*w2) + (x1*y2) - (y1*x2);
			float w = (w1*w2) - (x1*x2) - (y1*y2) - (z1*z2);
			
			// normalize and set result
			magnitude = sqrt((x*x) + (y*y) + (z*z) + (w*w));
			camera->cameraRotation[0] = x/magnitude;
			camera->cameraRotation[1] = y/magnitude;
			camera->cameraRotation[2] = z/magnitude;
			camera->cameraRotation[3] = w/magnitude;	
		}
	}
}

void pufCameraRotateEuler(PUFcamera* camera, float angleX, float angleY, float angleZ)
{

    // convert normalized angle to quaternion rotation
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

	// get current rotation
    float x2 = camera->cameraRotation[0];
    float y2 = camera->cameraRotation[1];
    float z2 = camera->cameraRotation[2];
    float w2 = camera->cameraRotation[3];

    // multiply rotations
    float x = (w1*x2) + (x1*w2) + (y1*z2) - (z1*y2);
    float y = (w1*y2) + (y1*w2) + (z1*x2) - (x1*z2);
    float z = (w1*z2) + (z1*w2) + (x1*y2) - (y1*x2);
    float w = (w1*w2) - (x1*x2) - (y1*y2) - (z1*z2);
    
    // normalize and set result
    float magnitude = sqrt((x*x) + (y*y) + (z*z) + (w*w));
    camera->cameraRotation[0] = x/magnitude;
    camera->cameraRotation[1] = y/magnitude;
    camera->cameraRotation[2] = z/magnitude;
    camera->cameraRotation[3] = w/magnitude;    

}

void pufCameraRotateEulerDegrees(PUFcamera* camera, float angleX, float angleY, float angleZ)
{
    

    // convert angles to radians
    angleX = angleX * M_PI / 180.0f;
    angleY = angleY * M_PI / 180.0f;
    angleZ = angleZ * M_PI / 180.0f;

    // convert angles to quaternion rotation
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

	// get current rotation
    float x2 = camera->cameraRotation[0];
    float y2 = camera->cameraRotation[1];
    float z2 = camera->cameraRotation[2];
    float w2 = camera->cameraRotation[3];

    // multiply new and current rotations
    float x = (w1*x2) + (x1*w2) + (y1*z2) - (z1*y2);
    float y = (w1*y2) + (y1*w2) + (z1*x2) - (x1*z2);
    float z = (w1*z2) + (z1*w2) + (x1*y2) - (y1*x2);
    float w = (w1*w2) - (x1*x2) - (y1*y2) - (z1*z2);
    
    // normalize and set result
    float magnitude = sqrt((x*x) + (y*y) + (z*z) + (w*w));
    camera->cameraRotation[0] = x/magnitude;
    camera->cameraRotation[1] = y/magnitude;
    camera->cameraRotation[2] = z/magnitude;
    camera->cameraRotation[3] = w/magnitude;    

}

