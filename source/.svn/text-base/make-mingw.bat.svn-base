@ECHO OFF

REM -- compile script for MinGW --
REM This links to the standard Win32 SDL library, thus you'll get the bug
REM where the sound sometimes goes silent if the window is not in focus.

REM Change this path to where you have MinGW\bin.
PATH=C:\MinGW\bin

del bin/protracker_opengl.exe 2>NUL
del bin/protracker.exe 2>NUL

windres src/protracker.rc src/resource.o

REM OpenGL version
echo Compiling OpenGL version...
mingw32-gcc src/*.c src/resource.o -DUSE_OPENGL -lmingw32 -lSDLmain -mwindows -lSDL -lopengl32 -lm -Wall -Wc++-compat -Wshadow -Winit-self -Wcast-align -Wextra -Wunused -Wunreachable-code -Wredundant-decls -Wswitch-default -march=pentium-m -mmmx -msse -msse2 -O3 -s -o bin/protracker_opengl.exe

REM Software render version
echo Compiling normal version...
mingw32-gcc src/*.c src/resource.o -lmingw32 -lSDLmain -mwindows -lSDL -lopengl32 -lm -Wall -Wc++-compat -Wshadow -Winit-self -Wcast-align -Wextra -Wunused -Wunreachable-code -Wredundant-decls -Wswitch-default -march=pentium-m -mmmx -msse -msse2 -O3 -s -o bin/protracker.exe

del src/*.o 2>NUL

echo.

echo Done! Executables are in the folder named 'bin'.
echo Remember to copy over SDL.dll (v1.2.15) to binary folder.

echo.

pause
