# <center>ffmpeg视频专门压缩<center>
@[TOC](ffmpeg视频专门压缩)

## 1.第一步将ffmpeg [编译so库](./ffmpeg编译成so库.md)

## 坑：

错误一：

 ```
 More than one file was found with OS independent path 'lib/armeabi-v7a/libavutil.so'.
 ```
 **原因**：so库在`CMakeLists.txt` 中已经做了关联，然后so库又是放在了`jniLibs/armeabi-v7a`中的,AndroidStudio本身就会自动依赖这个目录下的so库文件，所以会报`重复`的错误。  
 **解决办法**：更换`CMakeLists.txt`中关联的so库的位置


错误二：

 ```
 java.lang.UnsatisfiedLinkError: dalvik.system.PathClassLoader[DexPathList[[zip file "/data/app/com.east.ndk02ffmpegvideocompress-t9z1FQ0KgYcFvw1JArT6mQ==/base.apk"],
 nativeLibraryDirectories=[/data/app/com.east.ndk02ffmpegvideocompress-t9z1FQ0KgYcFvw1JArT6mQ==/lib/arm64, /system/lib64, /vendor/lib64, /product/lib64]]] couldn't find "libnative-lib.so"
 ```
**原因**： 在`build.gradle`中的 cmake 中指定了生成哪种架构的so库，却没指定程序运行时运用哪种架构的so库

```
        externalNativeBuild {
            cmake{
                cppFlags ""
                // 指定生成的so库的架构类型
                abiFilters "armeabi-v7a"
            }
        }
        //解决办法：指定运行哪种架构的so库
        ndk{
            //指定运行哪种架构的so库
            abiFilters "armeabi-v7a"
        }
```

错误三：

```
error: undefined reference to 'av_codec_next(AVCodec const*)'
```

解决方法：
用extern "C"{}把头文件包含起来。  
extern "C"{  
    #include "libavcodec/avcodec.h"  
    #include "libavformat/avformat.h"  
    # include "libavfilter/avfilter.h"  
}