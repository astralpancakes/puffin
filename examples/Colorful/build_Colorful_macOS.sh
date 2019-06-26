#!/bin/bash
rm Colorful
gcc -Wall -o ./Colorful Colorful.c ../../src/puffin.c -I ../../../glfw/include -L../../../glfw/src -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo
./Colorful
