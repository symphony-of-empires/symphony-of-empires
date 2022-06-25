#!/bin/bash

if [ "$1" = "android" ]; then
    export ANDROID_HOME="$HOME/Android/Sdk"
    export ANDROID_NDK_HOME="$ANDROID_HOME/ndk/24.0.8215888"
    export PATH="$ANDROID_NDK_HOME:$ANDROID_HOME/platform-tools:$ANDROID_HOME:$PATH"
    export BUILD_TOOLS="$ANDROID_HOME/build-tools/33.0.0"

    # Android setup
    #apt_install_package "openjdk-17-jdk"
    #apt_install_package "ant"
    #apt_install_package "android-sdk-platform-tools-common"

    mkdir -p build/
    cd build
    if [ ! -d mods ]; then ln -s ../mods mods || exit; fi
    cd ..
fi

# Create build directory
if [ ! -d build/ ]; then
    mkdir -p build/
    cd build
    if [ ! -d mods ]; then ln -s ../mods mods || exit; fi
    cd ..
    echo "Finished setup"
fi

if [ "$1" == "android" ]; then
    cmake \
        -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DANDROID=1 \
        -DE3D_BACKEND_GLES=1 \
        -DAPP_PLATFORM="android-24" \
        -DANDROID_PLATFORM="android-24" \
        -DANDROID_ABI=armeabi-v7a \
        -DANDROID_HOME="$ANDROID_HOME" \
        -DANDROID_NDK_HOME="$ANDROID_NDK_HOME" \
        -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake" \
        -B build/ \
        .
    
else
    cmake \
        -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DE3D_BACKEND_OPENGL=1 \
        -B build/ \
        .
fi

cd build
make -j`nproc` || exit
echo "Launching game"
if [ "$1" = "android" ]; then
    mkdir -p apk

    # Create the keystorage sha256 stuff
    if [ ! -f "keystore.jks" ]; then
        keytool -genkeypair -keystore keystore.jks -alias soe -validity 10000 -keyalg RSA -keysize 2048
    fi

    mkdir -p apk/lib/armeabi-v7a
    cp SymphonyOfEmpires apk/lib/armeabi-v7a/libSymphonyOfEmpires.so || exit
    cp vendor/assimp/bin/libassimp.so apk/lib/armeabi-v7a || exit
    cp vendor/glm/glm/libglm_shared.so apk/lib/armeabi-v7a || exit
    cp vendor/sdl2/libSDL2.so apk/lib/armeabi-v7a || exit
    cp vendor/sdl2_ttf/libSDL2_ttf.so apk/lib/armeabi-v7a || exit

    # Package the APK
    $BUILD_TOOLS/aapt package -f \
        -M ../game/AndroidManifest.xml \
        -I "$ANDROID_HOME/platforms/android-33/android.jar" \
        -F SymphonyOfEmpires.unsigned.apk apk/ || exit
    
    # Sign the JAR (we also need to sign the APK)
    jarsigner \
        -sigalg SHA256withRSA \
        -digestalg SHA256 \
        -keystore keystore.jks \
        -signedjar SymphonyOfEmpires.unaligned.apk \
        SymphonyOfEmpires.unsigned.apk \
        soe || exit

    # Align the APK
    $BUILD_TOOLS/zipalign -f -p 4 \
        SymphonyOfEmpires.unaligned.apk \
        SymphonyOfEmpires.aligned.apk || exit
    
    # Sign the APK
    $BUILD_TOOLS/apksigner sign --ks keystore.jks \
        --ks-key-alias soe \
        --out SymphonyOfEmpires.apk \
        SymphonyOfEmpires.aligned.apk || exit

    $BUILD_TOOLS/aapt list -v SymphonyOfEmpires.apk || exit 
    $ANDROID_HOME/platform-tools/adb install -r $PWD/SymphonyOfEmpires.apk || exit
else
    # Runs the game
    export MESA_GL_VERSION_OVERRIDE=4.4
    export MESA_GLSL_VERSION_OVERRIDE=440
    gdb ./SymphonyOfEmpires
fi
echo "Exiting game"

cd ..
