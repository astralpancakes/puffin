

/* zero the transformations of the mesh */
void pufMeshInit(PUFmesh* mesh) 
{
	for (int i=0; i<3; i++)
		mesh->meshScale[i] = 1.0f;
	for (int i=0; i<3; i++)
		mesh->meshTranslation[i] = 0.0f;
	for (int i=0; i<4; i++)
		mesh->meshRotation[i] = 0.0f;
	mesh->meshRotation[3] = 1.0f;
}

/* generates a nicely generic quad */
PUFmesh pufMeshShapeQuad()
{
    PUFmesh mesh;

    pufMeshInit(&mesh);

	mesh.vertexCount = 6;
	mesh.verts = (PUFvertex*)malloc(sizeof(PUFvertex)*mesh.vertexCount);
	mesh.verts[0].position[0] = -1.0f;
	mesh.verts[0].position[1] = 1.0f;
	mesh.verts[0].position[2] = 0.0f;
	mesh.verts[0].texture[0] = 0.0f;
	mesh.verts[0].texture[1] = 1.0f;
	mesh.verts[0].texture[2] = 0.0f;
	
	mesh.verts[1].position[0] = -1.0f;
	mesh.verts[1].position[1] = -1.0f;
	mesh.verts[1].position[2] = 0.0f;
	mesh.verts[1].texture[0] = 0.0f;
	mesh.verts[1].texture[1] = 0.0f;
	mesh.verts[1].texture[2] = 0.0f;
	
	mesh.verts[2].position[0] = 1.0f;
	mesh.verts[2].position[1] = -1.0f;
	mesh.verts[2].position[2] = 0.0f;
	mesh.verts[2].texture[0] = 1.0f;
	mesh.verts[2].texture[1] = 0.0f;
	mesh.verts[2].texture[2] = 0.0f;
	
	mesh.verts[3].position[0] = -1.0f;
	mesh.verts[3].position[1] = 1.0f;
	mesh.verts[3].position[2] = 0.0f;
	mesh.verts[3].texture[0] = 0.0f;
	mesh.verts[3].texture[1] = 1.0f;
	mesh.verts[3].texture[2] = 0.0f;
	
	mesh.verts[4].position[0] = 1.0f;
	mesh.verts[4].position[1] = -1.0f;
	mesh.verts[4].position[2] = 0.0f;
	mesh.verts[4].texture[0] = 1.0f;
	mesh.verts[4].texture[1] = 0.0f;
	mesh.verts[4].texture[2] = 0.0f;
	
	mesh.verts[5].position[0] = 1.0f;
	mesh.verts[5].position[1] = 1.0f;
	mesh.verts[5].position[2] = 0.0f;
	mesh.verts[5].texture[0] = 1.0f;
	mesh.verts[5].texture[1] = 1.0f;
	mesh.verts[5].texture[2] = 0.0f;
	
	for (int i=0; i<mesh.vertexCount; ++i)
	{
		mesh.verts[i].color[0] = ((float)(rand() % 100))/100;
        mesh.verts[i].color[1] = (float)(rand() % 100)/100;
        mesh.verts[i].color[2] = (float)(rand() % 100)/100;
        mesh.verts[i].normal[0] = 0.0f;
        mesh.verts[i].normal[1] = 0.0f;
        mesh.verts[i].normal[2] = 1.0f;
	}

    mesh.isBound = GL_FALSE;

    return mesh;

}

/* loads an OBJ file into Puffin mesh */
PUFmesh pufMeshLoadOBJ(char const* file) 
{
    PUFmesh mesh;

    pufMeshInit(&mesh);

    GLfloat* obj = pufLoadOBJ(file, &mesh.vertexCount);
    mesh.verts = (PUFvertex*)malloc(sizeof(PUFvertex)*mesh.vertexCount);

    for (int i=0; i<mesh.vertexCount; ++i)
    {
        mesh.verts[i].position[0] = obj[i*9+0];
        mesh.verts[i].position[1] = obj[i*9+1];
        mesh.verts[i].position[2] = obj[i*9+2];
        mesh.verts[i].color[0] = ((float)(rand() % 100))/100;
        mesh.verts[i].color[1] = (float)(rand() % 100)/100;
        mesh.verts[i].color[2] = (float)(rand() % 100)/100;
        mesh.verts[i].texture[0] = obj[i*9+3];
        mesh.verts[i].texture[1] = obj[i*9+4];
        mesh.verts[i].texture[2] = obj[i*9+5];
        mesh.verts[i].normal[0] = obj[i*9+6];
        mesh.verts[i].normal[1] = obj[i*9+7];
        mesh.verts[i].normal[2] = obj[i*9+8];
    }
    free((void*)obj);
    return mesh;
}

/* uploads Puffin mesh vertex buffer object */
void pufMeshBind(PUFmesh* mesh) 
{
	glGenBuffers(1, &mesh->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PUFvertex)*mesh->vertexCount, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(PUFvertex)*mesh->vertexCount, mesh->verts);	
    mesh->isBound = GL_TRUE;
}

/* draws Puffin mesh */
void pufMeshRender(PUFmesh* mesh, PUFcamera* camera, PUFshader* shader, PUFframebuffer* framebuffer) 
{
    if (mesh->isBound)
    {
        GLint fbHeight;
        GLint fbWidth;

        if (framebuffer)
        {
            pufFramebufferBindAndClear(framebuffer);
            // get framebuffer dimensions
            fbWidth = framebuffer->width;
            fbHeight = framebuffer->height;
        }
        else
        {
            // we're drawing to the default framebuffer, find out its width and height
            GLint dims[4] = {0};
            glGetIntegerv(GL_VIEWPORT, dims);
            fbWidth = dims[2];
            fbHeight = dims[3];
        }

        glUseProgram(shader->shaderProgram);
        

        GLint uniformModelviewProjectionMatrix = glGetUniformLocation(shader->shaderProgram, "modelviewProjectionMatrix");
        GLint uniformModelMatrix = glGetUniformLocation(shader->shaderProgram, "modelMatrix");
        GLint uniformModelViewMatrix = glGetUniformLocation(shader->shaderProgram, "modelViewMatrix");
        //GLint uniformRenderTargetWidth = glGetUniformLocation(shader->shaderProgram, "renderTargetWidth");
        //GLint uniformRenderTargetHeight = glGetUniformLocation(shader->shaderProgram, "renderTargetHeight");
        //GLint uniformRenderTargetSize = glGetUniformLocation(shader->shaderProgram, "renderTargetSize");
        GLint uniformLocResolution = glGetUniformLocation(shader->shaderProgram, "iResolution");
        GLint uniformTime = glGetUniformLocation(shader->shaderProgram, "iTime");

        GLfloat modelMatrix[16];
        GLfloat viewMatrix[16];
        GLfloat modelviewMatrix[16];

        GLfloat modelviewProjectionMatrix[16];

        GLfloat tempMatrix[16];
        GLfloat translationMatrix[16];
        GLfloat scalingMatrix[16];
        GLfloat rotationMatrix[16];

    // TODO: The following could be made clearer by calculating the matrices and multiplying them together as separate steps. 
    // Consider making the pufMatrix* functions not return anything since they need the return value pointer passed as an argument anyway.

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

        glUniformMatrix4fv(uniformModelviewProjectionMatrix, 1, GL_FALSE, modelviewProjectionMatrix);
        glUniformMatrix4fv(uniformModelViewMatrix, 1, GL_FALSE, modelviewMatrix);
        glUniformMatrix4fv(uniformModelMatrix, 1, GL_FALSE, modelMatrix);
        
        glUniform3f(uniformLocResolution, (GLfloat)fbWidth, (GLfloat)fbHeight, 1.0f);
        //glUniform1i(uniformRenderTargetWidth, fbWidth);
        //glUniform1i(uniformRenderTargetHeight, fbHeight);
        //glUniform2f(uniformRenderTargetSize, (float)camera->width, (float)camera->height);
        glUniform1f(uniformTime, shader->uniformTime);
        //glUniform1f(uniformTime, 1.0f);
        
        // dummy vertex array object for compatibility (https://stackoverflow.com/questions/24643027/opengl-invalid-operation-following-glenablevertexattribarray)
        GLuint vaoId = 0;
        glGenVertexArrays(1, &vaoId);
        glBindVertexArray(vaoId);
        
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

        glUseProgram(0);

        if (framebuffer)
        {
            pufFramebufferUnbind();
        }
        
    }
    else printf("Tried to render unbound mesh\n");
}

/* translates Puffin mesh */
void pufMeshTranslate(PUFmesh* mesh, float X, float Y, float Z) 
{
	mesh->meshTranslation[0] += X;
	mesh->meshTranslation[1] += Y;
	mesh->meshTranslation[2] += Z;
}

/* rotates Puffin mesh by a specified XYZ (Euler) angle */
void pufMeshRotateEuler(PUFmesh* mesh, float angleX, float angleY, float angleZ, PUF_ANGLE_UNITS units) 
{

    // convert angles to radians
    if (units == DEGREES)
    {
        angleX = angleX * M_PI / 180.0f;
        angleY = angleY * M_PI / 180.0f;
        angleZ = angleZ * M_PI / 180.0f;
    }

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
    float x2 = mesh->meshRotation[0];
    float y2 = mesh->meshRotation[1];
    float z2 = mesh->meshRotation[2];
    float w2 = mesh->meshRotation[3];

    // multiply rotations
    float x = (w1*x2) + (x1*w2) + (y1*z2) - (z1*y2);
    float y = (w1*y2) + (y1*w2) + (z1*x2) - (x1*z2);
    float z = (w1*z2) + (z1*w2) + (x1*y2) - (y1*x2);
    float w = (w1*w2) - (x1*x2) - (y1*y2) - (z1*z2);
    
    // normalize and set result
    float magnitude = sqrt((x*x) + (y*y) + (z*z) + (w*w));
    mesh->meshRotation[0] = x/magnitude;
    mesh->meshRotation[1] = y/magnitude;
    mesh->meshRotation[2] = z/magnitude;
    mesh->meshRotation[3] = w/magnitude;    

}

void pufMeshRotateEulerDegrees(PUFmesh* mesh, float angleX, float angleY, float angleZ) //rotates Puffin mesh by a specified (in degrees) XYZ (Euler) angle
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

/* rotates Puffin mesh about vector by angle */
void pufMeshRotate(PUFmesh* mesh, float angle, float vectorX, float vectorY, float vectorZ, PUF_ANGLE_UNITS units) 
{

	if (angle != 0.0)
	{
        // convert angles to radians
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
			float x2 = mesh->meshRotation[0];
			float y2 = mesh->meshRotation[1];
			float z2 = mesh->meshRotation[2];
			float w2 = mesh->meshRotation[3];

			// multiply rotations
			float x = (w1*x2) + (x1*w2) + (y1*z2) - (z1*y2);
			float y = (w1*y2) + (y1*w2) + (z1*x2) - (x1*z2);
			float z = (w1*z2) + (z1*w2) + (x1*y2) - (y1*x2);
			float w = (w1*w2) - (x1*x2) - (y1*y2) - (z1*z2);
			
			// normalize and set result
			magnitude = sqrt((x*x) + (y*y) + (z*z) + (w*w));
			mesh->meshRotation[0] = x/magnitude;
			mesh->meshRotation[1] = y/magnitude;
			mesh->meshRotation[2] = z/magnitude;
			mesh->meshRotation[3] = w/magnitude;	
		}
	}

}

/* scales Puffin mesh */
void pufMeshScale(PUFmesh* mesh, float X, float Y, float Z) 
{
	mesh->meshScale[0] *= X;
	mesh->meshScale[1] *= Y;
	mesh->meshScale[2] *= Z;
}

 /* deletes Puffin mesh */
void pufMeshDestroy(PUFmesh* mesh)
{
    free((void*)mesh->verts);
}
