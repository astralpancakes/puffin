REM Windows Batch File to build The OpenGL Extension Wrangler (GLEW) on Windows using MinGW.
REM Adapted from https://bruceoutdoors.wordpress.com/2014/07/16/glew-for-mingw-in-windows/
REM 
REM Download the source from http://glew.sourceforge.net/ and unpack, add this file to the extracted folder and run. 
REM MinGW and the mingw32-base package need to be installed.
REM
REM Tested and found working with GLEW 2.1.0 and mingw32-base 2013072200


REM mkdir lib
gcc -DGLEW_NO_GLU -O2 -Wall -W -Iinclude  -DGLEW_BUILD -o src/glew.o -c src/glew.c
gcc -nostdlib -shared -Wl,-soname,libglew32.dll -Wl,--out-implib,lib/libglew32.dll.a    -o lib/glew32.dll src/glew.o -L/mingw/lib -lglu32 -lopengl32 -lgdi32 -luser32 -lkernel32
 
ar cr lib/libglew32.a src/glew.o
 
gcc -DGLEW_NO_GLU -DGLEW_MX -O2 -Wall -W -Iinclude  -DGLEW_BUILD -o src/glew.mx.o -c src/glew.c
gcc -nostdlib -shared -Wl,-soname,libglew32mx.dll -Wl,--out-implib,lib/libglew32mx.dll.a -o lib/glew32mx.dll src/glew.mx.o -L/mingw/lib -lglu32 -lopengl32 -lgdi32 -luser32 -lkernel32
 
ar cr lib/libglew32mx.a src/glew.mx.o