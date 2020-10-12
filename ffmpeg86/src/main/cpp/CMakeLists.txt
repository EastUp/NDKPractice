# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
# 支持gnu++11
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")


# 2. 添加头文件目录
include_directories(${CMAKE_SOURCE_DIR}/../jniLibs/include)

# 3. 添加ffmpeg相关的so库
## 这个外部动态库的链接为什么没用,官网不推荐使用
link_directories(${CMAKE_SOURCE_DIR}/../jniLibs/armeabi-v7a)

# 指定源文件目录
aux_source_directory(${CMAKE_SOURCE_DIR} SRC_LIST)

set(
        # List variable name
        ffmpeg_libs_names
        # Values in the list
        avutil swresample swscale avcodec postproc avformat avfilter avdevice)


# 查找代码中使用到的系统库
find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# 配置目标so库编译信息
add_library( # Sets the name of the library.
        video-player

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        # 新建的cpp 文件，需要先在这下面写上，让编译器编译完了记住了才可以删除。
        Media.cpp
        ${SRC_LIST}
        )

# 指定编译目标库时，cmake要链接的库
target_link_libraries(

        # 指定目标库，native-lib 是在上面 add_library 中配置的目标库
        video-player

        # 4. 连接 FFmpeg 相关的库
        ${ffmpeg_libs_names}
        # 链接 android ndk 自带的一些库
        android
        # 链接OpenSLES
        OpenSLES
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})