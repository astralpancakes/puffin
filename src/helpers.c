char* pufReadFile(char const* file)
{
	FILE* f;
	long len;
	char* s = 0;
	// open file and get its length
	if ((f = fopen(file, "r")))
	{
  		fseek(f, 0, SEEK_END);
  		len = ftell(f);
		s = (char*)malloc(len+1);
  		// read the file in an allocated buffer
  		if (s)
		{
  			rewind(f);
			len = fread(s, 1, len, f);
  			s[len] = '\0';
		}
		else
		{
			printf("Puffin failed to allocate memory for file %s...\n",file);
		}
		fclose(f);
	}
	else
	{
		printf("Puffin failed to open file %s...\n",file);
	}
  	return s;
}

int32_t pufClampi(int value, int min, int max)
{
    if (value > max)
        value = max;
    if (value < min)
        value = min;
    return value;
}

float pufClampf(float value, float min, float max)
{
    if (value > max)
        value = max;
    if (value < min)
        value = min;
    return value;
}

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



PUFcolor pufColorFromRGBA(GLfloat R, GLfloat G, GLfloat B, GLfloat A)
{
    PUFcolor color;

    if (R < 0.0f)
        R = 0.0f;
    if (G < 0.0f)
        G = 0.0f;
    if (B < 0.0f)
        B = 0.0f;
    if (A < 0.0f)
        A = 0.0f;
    color.R = R;
    color.G = G;
    color.B = B;
    color.A = A;

    return color;
}