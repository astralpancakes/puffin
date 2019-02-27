#!/bin/bash
rm puffin-example
gcc -o ./puffin-example puffin-example.c ../src/puffin.c ../src/loadobj.c -I ../../glfw/include -L../../glfw/src -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo
./puffin-example
