del .\bin\puffin-example.exe
gcc -Wall -o ./bin/puffin-example.exe puffin-example.c ../src/puffin.c ../src/matrix.c ../src/loadobj.c ../src/helpers.c -lglfw3dll -lopengl32 -lglew32
start /D .\bin .\bin\puffin-example.exe