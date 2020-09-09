//
// Created by 123 on 2020/9/9.
//

#ifndef NDKPRACTICE_CV_HELPER_H
#define NDKPRACTICE_CV_HELPER_H

#include "opencv2/opencv.hpp"
#include <jni.h>
#include <android/bitmap.h>

class cv_helper{
public:

    /**
     *  Bitmap 转 mat
     */
    static int bitmap2mat(JNIEnv *env,jobject &bitmap,cv::Mat &dst);

    /**
    *  mat 转 bitmap
    */
    static int mat2bitmap(JNIEnv *env,cv::Mat &src,jobject &bitmap);

    /**
     *  创建 Bitmap
     */
    static jobject createBitmap(JNIEnv *env,jint width,jint height, jstring configName);
};

#endif //NDKPRACTICE_CV_HELPER_H
