//
// Created by 123 on 2020/10/10.
//

/**
 *  使用OpenSLES来播放，直接操作驱动，更加高效
 */
#ifndef NDKPRACTICE_AUDIO_H
#define NDKPRACTICE_AUDIO_H

#include "JNICall.h"
#include "ConstDefine.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>


extern "C"{
#include "libswresample/swresample.h"
#include "libavformat/avformat.h"
}

class Audio {
public:
    AVFormatContext *pFormatContext = NULL;
    AVCodecContext *pCodecContext = NULL;
    SwrContext *pSwrContext = NULL;
    uint8_t *resampleOutBuffer = NULL;
    JNICall *pJniCall = NULL;
    int audioStreamIndex = -1;

public:
    Audio(int audioStreamIndex,JNICall *pJniCall,AVCodecContext *pCodecContext,
          AVFormatContext *pFormatContext,SwrContext *pSwrContext);

    void play();

    void initCreateOpenSLES();

    int resampleAudio();
};


#endif //NDKPRACTICE_AUDIO_H
