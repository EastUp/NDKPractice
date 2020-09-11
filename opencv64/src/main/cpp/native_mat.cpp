//
// Created by 123 on 2020/9/11.
//

#ifndef NDKPRACTICE_NATIVE_MAT
#define NDKPRACTICE_NATIVE_MAT

#include <jni.h>
#include "cv_helper.h"
#include "opencv2/opencv.hpp"
#include <android/log.h>

using namespace cv;

extern "C"
JNIEXPORT jlong JNICALL
Java_com_east_opencv64_opencv2_Mat_nMatIII(JNIEnv *env, jobject thiz, jint rows, jint cols,
                                           jint value) {
   Mat *mat = new Mat(rows,cols,value);

   return reinterpret_cast<jlong>(mat);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_east_opencv64_opencv2_Mat_nMat(JNIEnv *env, jobject thiz) {
    Mat *mat = new Mat();
    return reinterpret_cast<jlong>(mat);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_east_opencv64_opencv2_Mat_nPutF(JNIEnv *env, jobject thiz,jlong ptr,jint row,jint col,jfloat value) {
    Mat *mat = reinterpret_cast<Mat*>(ptr);
    if(mat->type() == CV_32FC1){
        __android_log_print(ANDROID_LOG_ERROR,"tag","CV_32FC1：%d--%d--%f",row,col,value);
    }else if(mat->type() == CV_8UC4){
        __android_log_print(ANDROID_LOG_ERROR,"tag","CV_8UC4");
    }
    mat->at<float>(row,col) = value;
}

#endif //NDKPRACTICE_NATIVE_MAT