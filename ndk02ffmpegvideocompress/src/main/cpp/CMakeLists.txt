# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
# 支持gnu++11
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

#FFMpeg配置
#FFmpeg配置目录
# 1. 定义so库和头文件所在目录，方面后面使用
#set(ffmpeg_lib_dir ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})
set(ffmpeg_lib_dir ${CMAKE_SOURCE_DIR}/ffmpeg/armeabi-v7a)
#set(ffmpeg_lib_dir ${CMAKE_SOURCE_DIR}/ffmpeg/${ANDROID_ABI})
set(ffmpeg_head_dir ${CMAKE_SOURCE_DIR}/ffmpeg)

# 2. 添加头文件目录
include_directories(${ffmpeg_head_dir}/include ${ffmpeg_head_dir}/other)

# 3. 添加ffmpeg相关的so库



set(
        # List variable name
        ffmpeg_libs_names_version
        # Values in the list
        avutil-54 swresample-1 swscale-3 avcodec-56 postproc-53  avformat-56 avfilter-5 avdevice-56)

link_directories(${ffmpeg_lib_dir})


#set(
#        # List variable name
#        ffmpeg_libs_names
#        # Values in the list
#        avutil swresample swscale avcodec avformat avfilter avdevice)

#foreach (ffmpeg_lib_name  ${ffmpeg_libs_names})
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

# 查找代码中使用到的系统库
find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# 配置目标so库编译信息
add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp
        # 编译额外的 C 文件
        ffmpeg_filter.c
        ffmpeg_mod.c
        ffmpeg_opt.c
        cmdutils.c
        )

# 指定编译目标库时，cmake要链接的库
target_link_libraries(

        # 指定目标库，native-lib 是在上面 add_library 中配置的目标库
        native-lib

        # 4. 连接 FFmpeg 相关的库
#        ${ffmpeg_libs_names}
        ${ffmpeg_libs_names_version}

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})