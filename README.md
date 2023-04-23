# Symphony of the Empires
![Screenshot_2022-04-24_00-50-26](https://user-images.githubusercontent.com/39974089/164966091-b1683ebb-d1c5-4b22-b768-cb9c04b96b56.png)

Want to contribute or are interested in the development of the game? See our discord https://discord.gg/44QuvuDmCS

This is an open source RTS game engine; created to be very mod friendly and flexible for everyone's needs.

Features:
* Custom RTS game engine and framework
* Flexible & customizable UI
* Dynamic texture and audio management
* Lua scripting for events
* Support for multiple mods
* Heavy usage of multithreaded code

## Documentation
Documentation can be found at: https://symphony-of-empires.github.io/docs

## Download

If you're looking for the game you can find download links on: [https://symphony-of-empires.com/downloads.html].
Make sure to download the latest version before reporting any bugs.

## Building

Required to build:
* git
* SDL2 and SDL2-ttf
* OpenGL (GL and GLU)
* GLEW
* libassimp
* CMake 3.24

If you lack any of the prerequsites below you can use `cmake -DE3D_VENDORED=1` for building them instead.
Lua will **always** be built, since many package managers do not supply Lua 5.4 and backwards compatibility isn't a thing that should be dealt with.

**Windows**: 
On msys2, simply run the following after setting up the initial msys environment:
```sh
pacman -S mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2 mingw-w64-x86_64-glew mingw-w64-x86_64-glm mingw-w64-x86_64-zlib mingw-w64-x86_64-assimp mingw-w64-x86_64-intel-tbb
```
Or if you have a 32-bit computer:
```sh
pacman -S mingw-w64-i686-SDL2_ttf mingw-w64-i686-SDL2 mingw-w64-i686-glew mingw-w64-i686-glm mingw-w64-i686-zlib mingw-w64-i686-assimp  mingw-w64-i686-intel-tbb
```
For Visual Studio, you have to first get `vcpkg`, and install it
```batch
git clone https://github.com/Microsoft/vcpkg
.\vcpkg\bootstrap-vcpkg.bat  -disableMetrics
```

**Debian-based**: ``sudo apt install -y libsdl2-dev libsdl2-ttf-dev libtbb-dev libglew-dev libglm-dev libassimp-dev``

**Arch-based**: ``pacman -S sdl2 sdl2_ttf glew glm tbb assimp``

**NetBSD**:
```sh
pkgin in SDL2 SDL2_ttf assimp lua54 glew MesaLib glu libatomic threadingbuildingblocks
# The NetBSD libc already contains libdl within it, however libraries expect libdl to be a file
# and they will fail linking if said file is missing, making it a symbolic link is just a workaround
[ -e /usr/lib/libdl.a ] || ln -s /usr/lib/libc.a /usr/lib/libdl.a
[ -e /usr/lib/libdl.so ] || ln -s /usr/lib/libc.so /usr/lib/libdl.so
```

**Yuzu/Ryujinx**: Install devkitpro for aarch64, then ``sudo dkp-pacman --sync switch-dev switch-mesa switch-glad switch-glm switch-sdl2 switch-sdl2_ttf``

If you simply want to play the game on Debian run: ``./quick_build.sh``

## Building

The build with these commands once all the dependencies are met:
```sh
# In Linux, macOS, NetBSD, etc

# Use this for a normal build
mkdir -p build && cd build
cmake -DE3D_BACKEND_OPENGL=1 ..
cmake --build .

# Alternatively use this for a debug build
mkdir -p build && cd build
cmake -DE3D_BACKEND_OPENGL=1 -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
cmake --build .
```

### Visual Studio tutorial (by Erik)

- Clone the repo with `git clone --recursive --depth 1 https://github.com/symphony-of-empires/symphony-of-empires`.
If you need the git history, then you should remove the `--depth 1` flag

- Open the repo in Visual Studio
- Now select the build mode

![image](https://user-images.githubusercontent.com/89939707/233823886-d325484e-faaa-42a1-933c-7b5d89438473.png)

- Let it load and then you should be able to pick configuration up top
- If that doesn't work, then try reconfiguring the cache

![image](https://user-images.githubusercontent.com/89939707/233823839-d6464a2a-af04-49b6-a2f6-e60dda70a76d.png)

- Lastly select `SymphonyOfEmpires.exe` as the startup item

![image](https://user-images.githubusercontent.com/89939707/233823965-7f22a328-a615-42a6-98b2-fca8749c8d70.png)

- Now you can build the project

![image](https://user-images.githubusercontent.com/89939707/233823996-d3e3d05b-3835-4332-a3db-6907b83a1b00.png)


## Running
In order to run the game you just need to run it via the command line or left-click the executable: ``./SymphonyOfEmpires``
If the server crashes and the port needs to be re-aquired do the following under *NIX systems: ``fuser -k 1836/tcp``

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
    if(b) // stuff...
    else if(c) // stuff...

    // yes "single line" or brace-less ifs
    if(d < 0) increment_d(&d);
}
```

## Naming convention
Classes, Structs, Types and Typenames are named in PascalCase, while variables, struct/method members and functions are declared using snake_case. Enum members and macros are declared in SCREAMING_SNAKE_CASE.

```cpp
struct FooBar {
    FooBar() = default;
    ~FooBar() = default;

    int method() {
        return 0;
    }

    template<typename T1 = int, typename T2 = int>
    int add(T1 a, T2 b) {
        return a + b;
    }
public:
    int member;
    int test_thing;
    bool is_attack;
};
