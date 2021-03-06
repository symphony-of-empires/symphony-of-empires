# Symphony of the Empires
![Screenshot_2022-04-24_00-50-26](https://user-images.githubusercontent.com/39974089/164966091-b1683ebb-d1c5-4b22-b768-cb9c04b96b56.png)

Want to contribute or are interested in the development of the game? See our discord https://discord.gg/44QuvuDmCS

This is an Opensource RTS game engine; created to be very mod-friendly.

Required to build:

* SDL2 and SDL2-ttf
* lua 5.3 (or 5.4)
* GL and GLU
* libavcodec (on most package managers it's ffmpeg)

## Build on Linux

**If you simply want to play the game on linux:**
```
./quick_build.sh
```

### Debian-based distros
Install all dependencies with this command:
```
sudo apt install -y libpng-dev libsdl2-dev libsdl2-ttf-dev liblua5.3-dev libtbb-dev libglew-dev libglm-dev libassimp-dev
```

The build with these commands once all the dependencies are met:
```sh
# Use this for a normal build
cmake -DE3D_BACKEND_OPENGL=1 -DNO_COPY_MODS=1 .
# Alternatively use this for a debug build
cmake -DE3D_DEBUG=1 -DE3D_BACKEND_OPENGL=1 -DCMAKE_BUILD_TYPE=RelWithDebInfo .
make
```

Specifying `-j` to make will freeze systems, use `-j$(nproc)` instead, if a multithreaded compilation is desired.

Use this code to install Lua5.4
```sh
wget http://mirrors.kernel.org/ubuntu/pool/universe/l/lua5.4/liblua5.4-0_5.4.0-2_amd64.deb
sudo dpkg -i liblua5.4-0_5.4.0-2_amd64.deb
wget http://mirrors.kernel.org/ubuntu/pool/universe/l/lua5.4/liblua5.4-dev_5.4.0-2_amd64.deb
sudo dpkg -i liblua5.4-dev_5.4.0-2_amd64.deb
```
Alternatively, you can call `cmake` with the flag `-Dlua54:BOOL=ON` to compile with Lua 5.4 - since 5.3 is used by default.

The project can also be built using clang.

### Arch-based distros

```sh
pacman -S libpng sdl2 sdl2_ttf glew lua53 glm tbb
```

## Build on Windows

### MSYS2
Once msys2 has been installed for the first time, you'll have to run the following commands to install essential development packages:
```sh
pacman -Syu
pacman -Su
```

Simply run the following after setting up the initial msys environment:
```sh
pacman -S mingw-w64-x86_64-libpng mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2 mingw-w64-x86_64-glew mingw-w64-x86_64-lua mingw-w64-x86_64-glm mingw-w64-x86_64-zlib

# If you have an intel cpu, you need to execute this.
# This will overwrite the default tbb installation.
pacman -S mingw-w64-x86_64-intel-tbb
```

On 32-bit systems the following is required:
```sh
pacman -S mingw-w64-i686-libpng mingw-w64-i686-SDL2_ttf mingw-w64-i686-SDL2 mingw-w64-i686-glew mingw-w64-i686-lua mingw-w64-i686-glm mingw-w64-i686-zlib

# If you have an intel cpu, you need to execute this.
# This will overwrite the default tbb installation.
pacman -S mingw-w64-i686-intel-tbb
```

Finally; to build you only have to run:
```sh
cmake -G "MinGW Makefiles" -DE3D_BACKEND_OPENGL=1 -DNO_COPY_MODS=1 .
make
```

### Visual Studio
Builds can be built using Visual C compiler. The trick is to create a new solution and place everything there, add src and src\\client as include folders and use NuGet to obtain the required dependencies, then define the macro `windows`.

## Running
In order to run the game you just need to run it via the command line or left-click the executable:
```sh
./SymphonyOfEmpires
```

If the server crashes and the port needs to be re-aquired do the following under *NIX systems:
```sh
fuser -k 1836/tcp
```

# Coding style
4-spaces are used, tabs should be replaced with 4-spaces too. All functions, members and variables follow a
snake_case convention; whereas the object-typenames and types should be done as CamelCase.

## Object declaration
Class members on declarations should be ordered, starting by the private section, followed by the protected section, to
finally conclude on the public section. Each section should first start by the "main-constructor", followed by copy-constructors
and any variant of the constructors; after it, follows the destructor of the object. Then follows the main methods of the
object. Methods don't need to have a specific order. After the methods all the remaining member data is appended. Members
don't need to have a specific order.

In short - all functions for classes should be conformed as this:
```cpp
class Foo {
    // private constructors/destructors
    Foo(const Bar&);

    // followed by private methods
    int add_cmpxchg(int&&);

    // finally by the private data
    int secret_value;
protected:
    // constructor, copy-constructor & variants, destructor
    Foo(const Bar*);

    // methods
    int add_cmpxchg_sse3();

    // members
    int stuff;
public:
    Foo();
    // here should be any variant of constructor function prototypes
    ~Foo();

    // rest of the functions
    explicit int add(int&, int&);

    // and finally the data
    int value;
};
```

## Blocks
Simply put:
```cpp
while(a) {
    if(b) {
        // stuff...
    } else if(c) {
        // stuff...
    }

    // no "single line" or brace-less ifs
    if(d < 0) {
        increment_d(&d);
    }
}
```

## Naming convention
Classes, Structs, Types and Typenames are named in PascalCase, while variables, struct/method members and functions are declared using snake_case. Enum members and macros are declared in SCREAMING_SNAKE_CASE.

```cpp
struct FooBar {
    FooBar() {};
    ~FooBar() {};

    int method() {
        return 0;
    };

    template<typename T1 = int, typename T2 = int>
    int add(T1 a, T2 b) {
        return a + b;
    };
public:
    int member;
    int test_thing;
    bool is_attack;
};
