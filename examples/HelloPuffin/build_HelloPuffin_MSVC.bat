del HelloPuffin.exe
cl /Zi /TP /D_USE_MATH_DEFINES /I../../../include /FeHelloPuffin.exe HelloPuffin.c ../../src/puffin.c /link opengl32.lib ../../../lib/glew32.lib ../../../lib/glfw3dll.lib 
start HelloPuffin.exe