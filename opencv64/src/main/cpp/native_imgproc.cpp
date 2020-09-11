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
Java_com_east_opencv64_opencv2_Imgproc_nFilter2D(JNIEnv *env, jclass thiz, jlong src_ptr,jlong dst_ptr,jlong kernel_ptr) {

    Mat *src_mat = reinterpret_cast<Mat*>(src_ptr);
    Mat *dst_mat = reinterpret_cast<Mat*>(dst_ptr);
    Mat *kernel_mat = reinterpret_cast<Mat*>(kernel_ptr);

    filter2D(*src_mat,*dst_mat,src_mat->depth(),*kernel_mat);
}


#endif //NDKPRACTICE_NATIVE_UTILS