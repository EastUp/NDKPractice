//
// Created by 123 on 2020/9/11.
//

#ifndef NDKPRACTICE_NATIVE_UTILS
#define NDKPRACTICE_NATIVE_UTILS

#include <jni.h>
#include "cv_helper.h"
#include "opencv2/opencv.hpp"

using namespace cv;

extern "C"
JNIEXPORT void JNICALL
Java_com_east_opencv64_opencv2_Utils_nbitmap2mat(JNIEnv *env, jclass thiz, jobject bitmap,jlong ptr) {

    Mat *mat = reinterpret_cast<Mat*>(ptr);

    cv_helper::bitmap2mat(env,bitmap,*mat);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_east_opencv64_opencv2_Utils_nmat2bitmap(JNIEnv *env, jclass thiz,jlong ptr, jobject bitmap) {

    Mat *mat = reinterpret_cast<Mat*>(ptr);

    cv_helper::mat2bitmap(env,*mat,bitmap);
}

#endif //NDKPRACTICE_NATIVE_UTILS