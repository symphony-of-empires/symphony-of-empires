# Symphony of the Empires
![Screenshot_2021-08-18_19-09-37](https://user-images.githubusercontent.com/39974089/131609523-b7c825bc-5db0-4f52-b715-ac9e469f22b3.png)

Want to contribute or are interested in the development of the game? See our discord https://discord.gg/44QuvuDmCS

This is an Opensource RTS game engine; created to be very mod-friendly.

Required to build:

* SDL2 and SDL2-ttf
* lua 5.3 (or 5.4)
* GL and GLU

## Build (Linux)
Install all dependencies with this command:
```
sudo apt install -y libpng-dev libsdl2-dev libsdl2-ttf-dev liblua5.3-dev libtbb-dev
```

The build with these commands once all the dependencies are met:
```
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

Specifying `-j` to make will freeze systems, use `-j$(nproc)` instead, if a multithreaded compilation is desired.

Use this code to install Lua5.4
```
wget http://mirrors.kernel.org/ubuntu/pool/universe/l/lua5.4/liblua5.4-0_5.4.0-2_amd64.deb
sudo dpkg -i liblua5.4-0_5.4.0-2_amd64.deb
wget http://mirrors.kernel.org/ubuntu/pool/universe/l/lua5.4/liblua5.4-dev_5.4.0-2_amd64.deb
sudo dpkg -i liblua5.4-dev_5.4.0-2_amd64.deb
```
Alternatively, you can call `cmake` with the flag `-Dlua53:BOOL=ON` to compile with Lua 5.3 instead.

The project can also be built using clang.

## Build (Linux, MinGW)
Using mingw-w64 on linux it is possible to cross compile for Windows builds, simply run the following commands:
```
mkdir build && cd build
cmake -DWIN32=1 -DCMAKE_BUILD_TYPE=Debug ..
make -j
```

## Build (Windows, Visual Studio)
Builds can be built using Visual C compiler. The trick is to create a new solution and place everything there, add src and src\\client as include folders and use NuGet to obtain the required dependencies, then define the macro `windows`.

## Running

In order to run the game you need to first start the server, this server manages the world state and does the economic simulation:
```
./SymphonyOfEmpiresServer
```

After that you may want to initialize the client so you can interact with the server - this is done as follows:
```
./SymphonyOfEmpiresClient [optional IP address]
```
Not specifying a IP address will automatically connect the client to the localhost server - all servers use the port 1825.
