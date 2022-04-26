@ECHO OFF

CD ..
MKDIR build
CD build

ECHO ">>> Making the debug build"
MKDIR debug
CD debug
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DE3D_BACKEND_OPENGL=1 -G"Unix Makefiles" ../..
make -j2
CD ..

ECHO ">>> Creating debug distrobution package"
MKDIR distro_debug
CD distro_debug
COPY ..\debug\game\game.exe soe.exe
COPY ..\debug\eng3d\libeng3d.dll libeng3d.dll
MKDIR mods
XCOPY ..\..\mods mods /E /Y
ECHO ">>> Now add the remaining DLLs!"
CD ..

CD ..\scripts
