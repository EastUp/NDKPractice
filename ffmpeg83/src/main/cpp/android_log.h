//
// Created by 曾辉 on 2019-06-05.
//
#ifndef NDK_DAY03_ANDROID_LOG_H
#define NDK_DAY03_ANDROID_LOG_H

#include <android/log.h>

#define TAG "JNI_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)


#endif //NDK_DAY03_ANDROID_LOG_H
