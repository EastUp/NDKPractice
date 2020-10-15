# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
# 支持gnu++11
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

# 添加 define  -DNO_CRYPTO 解决报错 'openssl/ssl.h' file not found
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNO_CRYPTO")

# 指定源文件目录
aux_source_directory(${CMAKE_SOURCE_DIR} SRC_LIST)
aux_source_directory(${CMAKE_SOURCE_DIR}/rtmp RTMP_LIST)

# 查找代码中使用到的系统库
find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# 配置目标so库编译信息
add_library( # Sets the name of the library.
        live-push

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        # 新建的cpp 文件，需要先在这下面写上，让编译器编译完了记住了才可以删除。
        ${SRC_LIST}
        ${RTMP_LIST}
        )

# 指定编译目标库时，cmake要链接的库
target_link_libraries(

        # 指定目标库，live-push 是在上面 add_library 中配置的目标库
        live-push

        # 链接 android ndk 自带的一些库
        android
        # 链接OpenSLES
        OpenSLES
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})