@ECHO OFF

CD ..
MKDIR build
CD build

ECHO ">>> Making the release build"
MKDIR debug
CD debug
cmake -DCMAKE_BUILD_TYPE=Release -DUR_BACKEND_OPENGL=1 -G"Unix Makefiles" ../..
make -j3
CD ..

ECHO ">>> Creating release distrobution package"
MKDIR distro
CD distro
COPY ..\release\game\game.exe soe.exe
STRIP --strip-debug soe.exe -o soe.exe
COPY ..\release\unified_render\libunified_render.dll libunified_render.dll
MKDIR mods
XCOPY ..\..\mods mods /E /Y
ECHO "Now add the remaining DLLs!"
CD ..

CD ..\scripts
