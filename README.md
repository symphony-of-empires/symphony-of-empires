# Symphony of the Empires

Want to contribute or are interested in the development of the game? See our discord https://discord.gg/44QuvuDmCS

This is an Opensource RTS game engine; created to be very mod-friendly.

Required to build:

* libSDL2
* libSDL2-ttf
* liblua 5.4
* libGL 1.4 (+ GL Utility library)
* libTBB
* libPNG

## Build (Linux)
Install all dependencies with this command:
```
sudo apt install -y libpng-dev libsdl2-dev libsdl2-ttf-dev liblua5.4-dev libtbb-dev
```

If Lua5.4 is not available you can alternatively obtain both normal and development libraries directly:
```
wget http://mirrors.kernel.org/ubuntu/pool/universe/l/lua5.4/liblua5.4-0_5.4.0-2_amd64.deb
sudo dpkg -i liblua5.4-0_5.4.0-2_amd64.deb
wget http://mirrors.kernel.org/ubuntu/pool/universe/l/lua5.4/liblua5.4-dev_5.4.0-2_amd64.deb
sudo dpkg -i liblua5.4-dev_5.4.0-2_amd64.deb
```

Then these commands will build the project once all dependencies are met
```
mkdir build && cd build
cmake ..
make -j
```

## Build (Linux, MinGW)
Using mingw-w64 on linux it is possible to cross compile for Windows builds, simply run the following commands:
```
mkdir build && cd build
cmake -DWIN32=1 ..
make -j
```

## Build (Windows, Visual Studio)
Builds can be built using Visual C compiler. The trick is to create a new solution and place everything there, add src and src\\client as include folders and use NuGet to obtain the required dependencies, then define the macro `windows`.

## Running

In order to run the game you need to first start the server, this server manages the world state and does the economic simulation:
```
./SymphonyOfEmpires server
```

After that you may want to initialize the client so you can interact with the server - this is done as follows:
```
./SymphonyOfEmpires [optional IP address]
```
Not specifying a IP address will automatically connect the client to the localhost server - all servers use the port 1825.
