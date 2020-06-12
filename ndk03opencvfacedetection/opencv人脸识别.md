# <center>opencv人脸识别<center>
@[TOC](opencv人脸识别)


## 坑：

错误一：

 ```
  java.lang.UnsatisfiedLinkError: dlopen failed: library "libc++_shared.so" not found
 ```
**原因**： 对应的 so 库依赖的 libc++_shared.so 没有找到。

【特意去NDK官网查了下，默认情况下，NDK 构建系统为Android 系统提供的最小 C++ 运行时库 (system/lib/libstdc++.so) 提供 C++ 标头。此外，它随附您可以在自己的应用中使用或链接的替代 C++ 实现。请使用 APP_STL 选择其中一个。 】

解决方案：
如果是 .mk文件:
在 Application.mk 中添加 APP_STL := c++_shared

如果是 cmake文件（看下图）：
在app下的build.gradle中的cmake里面添加 arguments “-DANDROID_STL=c++_shared”

## 资料
《计算机视觉 - 算法与应用》  
[openBR](http://openbiometrics.org/)  
[CImage](https://msdn.microsoft.com/zh-cn/library/ca96yweh)  
[EasyPR](https://gitee.com/easypr/EasyPR)
