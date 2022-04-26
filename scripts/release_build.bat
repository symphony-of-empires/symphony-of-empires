@ECHO OFF

CD ..
MKDIR build
CD build

ECHO ">>> Making the release build"
MKDIR release
CD release
cmake -DCMAKE_BUILD_TYPE=Release -DE3D_BACKEND_OPENGL=1 -G"Unix Makefiles" ../..
make -j2
CD ..

ECHO ">>> Creating release distrobution package"
MKDIR distro
CD distro
COPY ..\release\game\game.exe soe.exe
STRIP --strip-debug soe.exe -o soe.exe
COPY ..\release\eng3d\libeng3d.dll libeng3d.dll
MKDIR mods
XCOPY ..\..\mods mods /E /Y
ECHO "Now add the remaining DLLs!"
CD ..

CD ..\scripts
