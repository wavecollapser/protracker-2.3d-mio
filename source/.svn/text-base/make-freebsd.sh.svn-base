#!/bin/sh

rm bin/protracker_opengl &> /dev/null
rm bin/protracker &> /dev/null

echo Compiling OpenGL version...
cc src/*.c -DUSE_OPENGL -I/usr/local/include -L/usr/local/lib -lSDL -lGL -lm -Wall -Wc++-compat -Wshadow -Winit-self -Wcast-align -Wextra -Wunused -Wunreachable-code -Wredundant-decls -Wswitch-default -O3 -o bin/protracker_opengl

echo Compiling normal version...
cc src/*.c -I/usr/local/include -L/usr/local/lib -lSDL -lm -Wall -Wc++-compat -Wshadow -Winit-self -Wcast-align -Wextra -Wunused -Wunreachable-code -Wredundant-decls -Wswitch-default -O3 -o bin/protracker

rm src/*.o &> /dev/null

echo Done! Executables are in the folder named \'bin\'.
