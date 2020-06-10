#!/bin/bash

#编译出来的库的存放位置，相对于脚本所在目录。
PREFIX=android-build-2

#设置你自己的 NDK 位置
NDK_HOME=D:/AndroidSdk/ndk/21.2.6472646
#设置你自己的平台，下面是 Linux 的
NDK_HOST_PLATFORM=windows-x86_64

COMMON_OPTIONS="\
    --target-os=android \
    --disable-static \
    --enable-shared \
    --enable-small \
    --disable-programs \
    --disable-ffmpeg \
    --disable-ffplay \
    --disable-ffprobe \
    --disable-doc \
    --disable-symver \
    --disable-asm \
    "

function build_android {
    ./configure \
    --libdir=${PREFIX}/libs/armeabi-v7a \
    --incdir=${PREFIX}/includes/armeabi-v7a \
    --pkgconfigdir=${PREFIX}/pkgconfig/armeabi-v7a \
    --arch=arm \
    --cpu=armv7-a \
    --cross-prefix="${NDK_HOME}/toolchains/llvm/prebuilt/${NDK_HOST_PLATFORM}/bin/arm-linux-androideabi-" \
    --sysroot="${NDK_HOME}/platforms/android-19/arch-arm/" \
    --extra-cflags="-march=armv7-a -mfloat-abi=softfp -mfpu=neon" \
    --extra-ldexeflags=-pie \
    ${COMMON_OPTIONS}
    make clean
    make -j8 && sudo make install

};

build_android