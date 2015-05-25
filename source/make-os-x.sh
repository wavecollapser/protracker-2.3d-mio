#!/bin/bash

arch=$(arch)
if [ $arch == "ppc" ]; then
    echo Compiling PowerPC binary...
    
    rm bin/protracker-ppc.app/Contents/MacOS/protracker &> /dev/null
    
    gcc -mmacosx-version-min=10.4 -I/Library/Frameworks/SDL.framework/Headers -c SDLMain_OS_X/SDLMain.m -o SDLMain_OS_X/SDLMain.o
    gcc -mmacosx-version-min=10.4 -DUSE_OPENGL -I/Library/Frameworks/SDL.framework/Headers -F/Library/Frameworks src/*.c -O3 -lm -Wall -Wshadow -Winit-self -Wextra -Wunused -Wredundant-decls -Wswitch-default SDLMain_OS_X/SDLMain.o -framework SDL -framework OpenGL -framework Cocoa -lm -o bin/protracker-ppc.app/Contents/MacOS/protracker
    install_name_tool -change @rpath/SDL.framework/Versions/A/SDL @executable_path/../Frameworks/SDL.framework/Versions/A/SDL bin/protracker-ppc.app/Contents/MacOS/protracker
    
    rm src/*.o &> /dev/null
    rm SDLMain_OS_X/SDLMain.o &> /dev/null
else
    echo Compiling Intel binary...
    
    rm bin/protracker-intel.app/Contents/MacOS/protracker &> /dev/null
    
    gcc -mmacosx-version-min=10.5 -m32 -mfpmath=sse -msse2 -I/Library/Frameworks/SDL.framework/Headers -c SDLMain_OS_X/SDLMain.m -o SDLMain_OS_X/SDLMain.o
    gcc -mmacosx-version-min=10.5 -m32 -mfpmath=sse -msse2 -DUSE_OPENGL -I/Library/Frameworks/SDL.framework/Headers -F/Library/Frameworks src/*.c -O3 -lm -Wall -Wshadow -Winit-self -Wextra -Wunused -Wredundant-decls -Wswitch-default SDLMain_OS_X/SDLMain.o -framework SDL -framework OpenGL -framework Cocoa -lm -o bin/protracker-intel.app/Contents/MacOS/protracker
    install_name_tool -change @rpath/SDL.framework/Versions/A/SDL @executable_path/../Frameworks/SDL.framework/Versions/A/SDL bin/protracker-intel.app/Contents/MacOS/protracker
    
    rm src/*.o &> /dev/null
    rm SDLMain_OS_X/SDLMain.o &> /dev/null
fi

echo Done! Executable is in the folder named \'bin\'.
