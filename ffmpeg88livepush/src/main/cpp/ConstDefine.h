//
// Created by 123 on 2020/10/9.
//

#ifndef NDKPRACTICE_CONSTDEFINE_H
#define NDKPRACTICE_CONSTDEFINE_H

#include <android/log.h>

#define TAG "JNI_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

#define AUDIO_SAMPLE_RATE 44100

// ---------- 错误码 start ----------
#define INIT_RTMP_CONNECT_ERROR_CODE -0x10
#define INIT_RTMP_CONNECT_STREAM_ERROR_CODE -0x11
// ---------- 错误码 end ----------

#endif //NDKPRACTICE_CONSTDEFINE_H
