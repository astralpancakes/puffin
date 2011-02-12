#include <stdio.h>

char* readFile(char const* name)
{
  FILE* f;
  int len;
  char* s = 0;

  // open file an get its length
  if (!(f = fopen(name, "r"))) goto readFileError1;
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

