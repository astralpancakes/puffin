#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "helpers.h"

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

int pufClampi(int value, int min, int max)
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