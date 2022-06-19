#!/bin/bash

# Obtains the system type
function get_system_type() {
    system_type="generic"
    if [ -f "/etc/arch-release" ]; then
        system_type="arch"
    else
        system_type="debian"
    fi
}
# Call once
get_system_type

# apt check for installed packages
# @param The name of the package
function apt_install_package() {
    pkg_is_ok=$(dpkg-query -W --showformat='${Status}\n' $1 | grep "install ok installed")
    echo Checking for $1: $pkg_is_ok
    if [ "" = "$pkg_is_ok" ]; then
        echo "No $1. Installing..."
        sudo apt install -y $1 || exit
    fi
}

# pacman check for installed packages
# @param The name of the package
function pacman_install_package() {
    if pacman -Qs $1 > /dev/null ; then
        echo "Checking for $1: install ok installed"
    else
        echo "No $1. Installing..."
        pacman -S libpng sdl2 sdl2_ttf glew lua53 glm tbb
    fi
}

# Install packages
sudo echo "sudo is required to install packages"
if [ "$system_type" = "debian" ]; then
    apt_install_package "libpng-dev"
    apt_install_package "libsdl2-dev"
    apt_install_package "libsdl2-ttf-dev"
    apt_install_package "libtbb-dev"
    apt_install_package "libglew-dev"
    apt_install_package "libglm-dev"
    apt_install_package "liblua5.3-dev"
    apt_install_package "libassimp-dev"
elif [ "$system_type" = "arch" ]; then
    pacman_install_package "libpng"
    pacman_install_package "sdl2"
    pacman_install_package "sdl2_ttf"
    pacman_install_package "tbb"
    pacman_install_package "glew"
    pacman_install_package "lua53"
    pacman_install_package "glm"
    pacman_install_package "assimp"
else
    echo "I don't know $system_type"
    exit
fi

# Create build directory
if [ ! -d build/ ]; then
    mkdir -p build/
    cd build
    ln -s ../mods mods || exit
    cd ..
    echo "Finished setup"
fi

cmake -DCMAKE_BUILD_TYPE=Debug -DE3D_BACKEND_OPENGL=1 -B build/ .

cd build
make -j`nproc` || exit
echo "Launching game"
# Runs the game
MESA_GL_VERSION_OVERRIDE=4.4 MESA_GLSL_VERSION_OVERRIDE=440 gdb ./SymphonyOfEmpires
echo "Exiting game"

cd ..
