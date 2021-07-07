# Symphony of the Empires

Want to contribute or are interested in the development of the game? See our discord https://discord.gg/44QuvuDmCS

This is an Opensource RTS game engine; created to be very mod-friendly.

Required to build:

* libSDL2
* libSDL2-ttf
* liblua 5.4
* liblua 5.4-dev on ubuntu and similar distroes
* libGL 1.4 (+ Utility library)

## Build (Linux)
Install all dependencies with this command:
```
sudo apt install -y libpng-dev libsdl2-dev libsdl2-ttf-dev liblua5.4-dev
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
make -j4
```