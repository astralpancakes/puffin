float* pufMatrixTranslate(float X, float Y, float Z, float* M)
{
    M[0] = 1;M[4] = 0;M[8] = 0;M[12] = X;
    M[1] = 0;M[5] = 1;M[9] = 0;M[13] = Y;
    M[2] = 0;M[6] = 0;M[10] = 1;M[14] = Z;
    M[3] = 0;M[7] = 0;M[11] = 0;M[15] = 1;

    return M;
}

float* pufMatrixRotate(float ang, float X, float Y, float Z, float* M)
{

    if (X+Y+Z != 0)
    {
        // normalize vector
        float magnitude = sqrt((X*X) + (Y*Y) + (Z*Z));
        X = X/magnitude;
        Y = Y/magnitude;
        Z = Z/magnitude;
    }
    else
    {
        ang = 0;
    }

    float c = cos(ang);
    float s = sin(ang);

    M[0] = pow(X,2.0f)*(1.0-c)+c;
    M[1] = Y*X*(1.0-c)+Z*s;
    M[2] = X*Z*(1.0-c)-Y*s;
    M[3] = 0.0;

    M[4] = X*Y*(1.0-c)-Z*s;
    M[5] = pow(Y,2.0f)*(1.0-c)+c;
    M[6] = Y*Z*(1.0-c)+X*s;
    M[7] = 0.0;

    M[8] = X*Z*(1.0-c)+Y*s;
    M[9] = Y*Z*(1.0-c)-X*s;
    M[10] = pow(Z,2.0f)*(1.0-c)+c;
    M[11] = 0.0;

    M[12] = 0.0;
    M[13] = 0.0;
    M[14] = 0.0;
    M[15] = 1.0;
    
	return M;
}

float* pufMatrixFromQuaternion(float X, float Y, float Z, float W, float* M)
{
    float xx      = X * X;
    float xy      = X * Y;
    float xz      = X * Z;
    float xw      = X * W;

    float yy      = Y * Y;
    float yz      = Y * Z;
    float yw      = Y * W;

    float zz      = Z * Z;
    float zw      = Z * W;

    M[0]  = 1 - 2 * ( yy + zz );
    M[1]  =     2 * ( xy - zw );
    M[2]  =     2 * ( xz + yw );

    M[4]  =     2 * ( xy + zw );
    M[5]  = 1 - 2 * ( xx + zz );
    M[6]  =     2 * ( yz - xw );

    M[8]  =     2 * ( xz - yw );
    M[9]  =     2 * ( yz + xw );
    M[10] = 1 - 2 * ( xx + yy );

	M[3]  = M[7] = M[11] = M[12] = M[13] = M[14] = 0.0;
    M[15] = 1.0;
	
	return M;
}

float* pufMatrixRotateEuler(float angleX, float angleY, float angleZ, float* M)
{
	float cX = cos(angleX);
	float sX = sin(angleX);
	float cY = cos(angleY);	
	float sY = sin(angleY);
	float cZ = cos(angleZ);
	float sZ = sin(angleZ);

	float cXsY = cX*sY;
	float sXsY = sX*sY;

    M[0] = cY * cZ;
    M[1] = sXsY*cZ + cX*sZ;
    M[2] = -cXsY*cZ + sX*sZ;
    M[3] = 0;

    M[4] = -cY*sZ;
    M[5] = -sXsY*sZ + cX*cZ;
    M[6] = cXsY*sZ + sX*cZ;
    M[7] = 0;

    M[8] = sY;
    M[9] = -sX*cY;
    M[10] = cX*cY;
    M[11] = 0;

    M[12] = 0;
    M[13] = 0;
    M[14] = 0;
    M[15] = 1;	
	
	return M;
}

float* pufMatrixScale(float X, float Y, float Z, float* M)
{
	memset(M, 0, sizeof(float)*16);
	
    M[0] = X;
    M[5] = Y;
    M[10] = Z;
    M[15] = 1;

    return M;
}

float* pufMatrixProjectPersp(float fov, float width, float height, float zNear, float zFar, float* M)
{


    // assume FOV given as degrees
    fov = fov * M_PI / 180.0f;
	
    float f = 1.0f/tan(fov/2.0f);
    float negDepth = zNear-zFar;

	memset(M, 0, sizeof(float)*16);

    M[0] = f/(width/height);
    M[5] = f;
    M[10] = (zFar + zNear)/negDepth;
    M[11] = -1;
    M[14] = (2.0f*zFar*zNear)/negDepth;
		
    return M;
}

float* pufMatrixProjectOrtho(float width, float height, float zNear, float zFar, float* M)
{
    memset(M, 0, sizeof(float)*16);
    
    M[0] = 1.0f/(width);
    M[5] = 1.0f/(height);
    M[10] = -2.0f/(zFar-zNear);
    M[14] = -((zFar+zNear)/(zFar-zNear));
    M[15] = 1.0f;
    
    return M;

}

float* pufMatrixMult(float* A, float* B, float* M)
{
    int i, j;
    for (i=0; i<16; i+=4)
        for (j=0; j<4; j++)
            M[i+j] = A[j]*B[i] + A[j+4]*B[i+1] + A[j+8]*B[i+2] + A[j+12]*B[i+3];
    return M;
}

float* pufMatrixShrink(float* A, float* M)
{
    int i, j;
    for (i=0; i<3; i++)
        for (j=0; j<3; j++)
            M[i*3+j] = A[i*4+j];
    return M;
}

