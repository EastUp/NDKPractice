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
#include "PacketQueue.h"
#include "PlayerStatus.h"
#include "Media.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

extern "C"{
#include "libswresample/swresample.h"
#include "libavformat/avformat.h"
}

class Audio {
public:
    AVFormatContext *pFormatContext = NULL;
    SwrContext *pSwrContext = NULL;
    uint8_t *resampleOutBuffer = NULL;
public:
    Audio(int audioStreamIndex,JNICall *pJniCall,AVFormatContext *pFormatContext);
    ~Audio();

    void play();

    void initCreateOpenSLES();

    int resampleAudio();

    void analysisStream(ThreadMode threadMode,AVStream **stream);

    void callPlayerJniError(ThreadMode threadMode, int code, char* msg);

    void release();
};


#endif //NDKPRACTICE_AUDIO_H
