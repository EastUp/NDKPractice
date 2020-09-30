# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
# 支持gnu++11
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

#FFMpeg配置
#FFmpeg配置目录
# 1. 定义so库和头文件所在目录，方面后面使用
#set(ffmpeg_lib_dir ${CMAKE_SOURCE_DIR}/ffmpeg/${ANDROID_ABI})
set(ffmpeg_lib_dir ${CMAKE_SOURCE_DIR}/ffmpeg/armeabi-v7a)
set(ffmpeg_head_dir ${CMAKE_SOURCE_DIR}/ffmpeg)

# 2. 添加头文件目录
include_directories(${ffmpeg_head_dir}/include ${ffmpeg_head_dir}/other)

# 3. 添加ffmpeg相关的so库



#set(
#        # List variable name
#        ffmpeg_libs_names_version
#        # Values in the list
#        avutil-54 swresample-1 swscale-3 avcodec-56 postproc-53  avformat-56 avfilter-5 avdevice-56)
#
## 这个外部动态库的链接为什么没用,官网不推荐使用
##link_directories(${ffmpeg_lib_dir})
#
#foreach (ffmpeg_lib_name  ${ffmpeg_libs_names_version})
#    add_library(
#            ${ffmpeg_lib_name}
#            SHARED
#            IMPORTED)
#    set_target_properties(
#            ${ffmpeg_lib_name}
#            PROPERTIES
#            IMPORTED_LOCATION
#            ${ffmpeg_lib_dir}/lib${ffmpeg_lib_name}.so)
#endforeach ()


set(
        # List variable name
        ffmpeg_libs_names
        # Values in the list
        avutil swresample swscale avcodec postproc avformat avfilter avdevice avresample)

foreach (ffmpeg_lib_name  ${ffmpeg_libs_names})
    add_library(
            ${ffmpeg_lib_name}
            SHARED
            IMPORTED)
    set_target_properties(
            ${ffmpeg_lib_name}
            PROPERTIES
            IMPORTED_LOCATION
            ${ffmpeg_lib_dir}/lib${ffmpeg_lib_name}.so)
endforeach ()

# 查找代码中使用到的系统库
find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# 配置目标so库编译信息
add_library( # Sets the name of the library.
        ffmpeg-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ffmpeg_lib.c
        # 编译额外的 C 文件
        cmdutils.c
        ffmpeg.c
        ffmpeg_filter.c
        ffmpeg_opt.c
        ffmpeg_thread.c
        ffmpeg_hw.c
        )

# 指定编译目标库时，cmake要链接的库
target_link_libraries(

        # 指定目标库，native-lib 是在上面 add_library 中配置的目标库
        ffmpeg-lib

        # 4. 连接 FFmpeg 相关的库
        ${ffmpeg_libs_names}
#        ${ffmpeg_libs_names_version}
#        # 编解码(最重要的库)
#        avcodec-56
#        # 设备信息
#        avdevice-56
#        # 滤镜特效处理库
#        avfilter-5
#        # 封装格式处理库
#        avformat-56
#        # 工具库(大部分库都需要这个库的支持)
#        avutil-54
#        # 后期处理
#        postproc-53
#        # 音频采样数据格式转换库
#        swresample-1
#        # 视频像素数据格式转换
#        swscale-3
#        # 链接 android ndk 自带的一些库
#        android

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})