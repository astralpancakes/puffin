del puffin-example.exe
cl /Zi /TP /D_USE_MATH_DEFINES /I../../include /Fepuffin-example.exe puffin-example.c ../src/puffin.c ../src/matrix.c ../src/loadobj.c ../src/helpers.c /link opengl32.lib ../../lib/glew32.lib ../../lib/glfw3dll.lib 
start puffin-example.exe