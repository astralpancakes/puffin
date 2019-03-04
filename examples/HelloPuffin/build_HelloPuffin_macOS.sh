#!/bin/bash
rm HelloPuffin
gcc -o ./HelloPuffin HelloPuffin.c ../../src/puffin.c -I ../../../glfw/include -L../../../glfw/src -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo
./HelloPuffin
