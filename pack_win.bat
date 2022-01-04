MKDIR build
CD build

MKDIR distro
CD distro

COPY ..\game\game.exe game.exe
COPY ..\unified_render\libunified_render.dll libunified_render.dll

MKDIR mods
XCOPY ..\..\mods mods /E

ECHO "Now add the remaining DLLs!"
CD ..\..\
