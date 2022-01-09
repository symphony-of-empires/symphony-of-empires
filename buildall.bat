@ECHO OFF

MKDIR build
CD build

ECHO ">>> Making the release build"
MKDIR release
CD release
cmake -DCMAKE_BUILD_TYPE=Release -DUR_BACKEND_OPENGL=1 -G"Unix Makefiles" ../..
make -j3
CD ..

ECHO ">>> Making the debug build"
MKDIR debug
CD debug
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DUR_BACKEND_OPENGL=1 -G"Unix Makefiles" ../..
make -j3
CD ..

ECHO ">>> Creating distrobution package"
MKDIR distro
CD distro
COPY ..\release\game\game.exe soe.exe
STRIP --strip-debug soe.exe -o soe.exe
COPY ..\release\unified_render\libunified_render.dll libunified_render.dll
MKDIR mods
XCOPY ..\..\mods mods /E /Y
ECHO "Now add the remaining DLLs!"
CD ..

ECHO ">>> Creating distrobution package"
MKDIR distro_debug
CD distro_debug
COPY ..\debug\game\game.exe soe.exe
COPY ..\debug\unified_render\libunified_render.dll libunified_render.dll
MKDIR mods
XCOPY ..\..\mods mods /E /Y
ECHO "Now add the remaining DLLs!"
CD ..

CD ..
