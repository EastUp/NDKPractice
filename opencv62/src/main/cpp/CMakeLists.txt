# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(opencv_lib_dir ${CMAKE_SOURCE_DIR}/opencv/armeabi-v7a)
#set(opencv_lib_dir ${CMAKE_SOURCE_DIR}/opencv/${ANDROID_ABI})
#set(opencv_lib_dir ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})

# 加入头文件
include_directories(${CMAKE_SOURCE_DIR}/opencv/include)

add_library(
        opencv_java4
        SHARED
        IMPORTED)

set_target_properties(
        opencv_java4
        PROPERTIES IMPORTED_LOCATION
        ${opencv_lib_dir}/libopencv_java4.so
)



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
        cv_helper.cpp
        )

# 指定编译目标库时，cmake要链接的库
target_link_libraries(

        # 指定目标库，native-lib 是在上面 add_library 中配置的目标库
        native-lib
        # 加入opencv依赖
        opencv_java4
        # Android bitmap.h 操作需要引入的库
        jnigraphics

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})