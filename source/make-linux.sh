#!/bin/bash

rm bin/protracker_opengl &> /dev/null
rm bin/protracker &> /dev/null

echo Compiling OpenGL version...
gcc src/*.c -DUSE_OPENGL -lSDL -lGL -lm -ldl -Wall -Wc++-compat -Wshadow -Winit-self -Wcast-align -Wextra -Wunused -Wunreachable-code -Wredundant-decls -Wswitch-default -O3 -o bin/protracker_opengl

echo Compiling normal version...
gcc src/*.c -lSDL -lm -ldl -Wall -Wc++-compat -Wshadow -Winit-self -Wcast-align -Wextra -Wunused -Wunreachable-code -Wredundant-decls -Wswitch-default -O3 -o bin/protracker

rm src/*.o &> /dev/null

echo Done! Executables are in the folder named \'bin\'.
