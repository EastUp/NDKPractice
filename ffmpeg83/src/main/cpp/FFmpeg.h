//
// Created by 123 on 2020/10/9.
//

#ifndef NDKPRACTICE_FFMPEG_H
#define NDKPRACTICE_FFMPEG_H

#include "JNICall.h"
#include <pthread.h>

// 在 c++ 中采用 c 的这种编译方式
extern "C"{
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
}

class FFmpeg {
public:
    AVFormatContext *pFormatContext = NULL;
    AVCodecContext *pCodecContext = NULL;
    SwrContext *swrContext = NULL;
    uint8_t *resampleOutBuffer = NULL;
    const char* url = NULL;
    JNICall *pJniCall = NULL;

public:
    FFmpeg(JNICall *pJniCall,const char *url);
    ~FFmpeg();

public:
    void play();

    void prepare();

    void prepareAsync();

    void callPlayerJniError(int code,char *msg);

    void release();
};


#endif //NDKPRACTICE_FFMPEG_H
