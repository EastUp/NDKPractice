//
// Created by 123 on 2020/6/30.
//

#ifndef NDKPRACTICE_BITMAPMATUTILS_H
#define NDKPRACTICE_BITMAPMATUTILS_H

#include <jni.h>
#include "opencv2/opencv.hpp"

using namespace cv;

class BitmapMatUtils{
public:
    // 开发项目增强，方法怎么写
    // java 中是想把想要的结果返回
    // c/c++ 结果参数传递，返回值一般返回是否成功
    static int bitmap2mat(JNIEnv* env,jobject bitmap,Mat &mat);

    /**
     *  mat 转 bitmap
     */
    static int mat2bitmap(JNIEnv* env,jobject bitmap,Mat &mat);
};

#endif //NDKPRACTICE_BITMAPMATUTILS_H
