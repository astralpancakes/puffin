#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "helpers.h"

char* pufReadFile(char const* file)
{
  FILE* f;
  long len;
  char* s = 0;
  // open file an get its length
  if (!(f = fopen(file, "r"))) goto readFileError1;
  fseek(f, 0, SEEK_END);
  len = ftell(f);
  // read the file in an allocated buffer
  if (!(s = (char*)malloc(len+1))) goto readFileError2;
  rewind(f);
  len = fread(s, 1, len, f);
  s[len] = '\0';

  readFileError2: fclose(f);
  readFileError1: return s;
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