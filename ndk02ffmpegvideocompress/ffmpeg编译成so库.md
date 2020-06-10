# <center>ffmpeg编译成so库<center>
@[TOC](ffmpeg编译成so库)

## 坑

1. /tests/Makefile: No such file or directory make: *** 没有规则可制作目标“  
   `解决`:先运行 ffmpeg根目录下的`config`文件

## 编译脚本文件

在module目录下有`test1.sh`,`test2.sh`,`test3.sh`,`build_android_clang.sh`,主要是用`test1.sh`。  
注意记得修改 `NDK_PATH`和`HOST_PLATFORM_WIN`

```
#!/usr/bin/env bash

NDK_PATH=D:/AndroidSdk/ndk/21.2.6472646
HOST_PLATFORM_WIN=windows-x86_64
HOST_PLATFORM=$HOST_PLATFORM_WIN
API=29

TOOLCHAINS="$NDK_PATH/toolchains/llvm/prebuilt/$HOST_PLATFORM"
SYSROOT="$NDK_PATH/toolchains/llvm/prebuilt/$HOST_PLATFORM/sysroot"
CFLAG="-D__ANDROID_API__=$API -Os -fPIC -DANDROID "
LDFLAG="-lc -lm -ldl -llog "

PREFIX=android-build

CONFIG_LOG_PATH=${PREFIX}/log
COMMON_OPTIONS=
CONFIGURATION=
```

