//
// Created by 曾辉 on 2019-06-06.
//

#ifndef NDK_DAY03_DZAUDIO_H
#define NDK_DAY03_DZAUDIO_H

#include "AVPacketQueue.h"
#include "JNIPlayerCall.h"
#include "DZMedia.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "DZConstDefine.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/frame.h"
#include "libswresample/swresample.h"
#include "libavutil/channel_layout.h"
#include "libavutil/rational.h"
#include "libavutil/time.h"
};

class DZAudio : public DZMedia {
public:
    pthread_t playThreadT;
    uint8_t *convertOutBuffer = NULL;
    SwrContext *swrContext = NULL;
    SLPlayItf slPlayItf = NULL;
    SLObjectItf pPlayer = NULL;
    SLObjectItf mixObj = NULL;
    SLObjectItf engineObj = NULL;
    SLAndroidSimpleBufferQueueItf androidBufferQueueItf;
    int frameBufferSize = 0;


public:
    DZAudio(int audioStreamIndex, DZPlayStatus *pPlayStatus, JNIPlayerCall *pPlayerCall);

    ~DZAudio();

    void play();

    void resampleAudio();

    void initOpenSLES();

    int getSampleRateForOpenSLES(int sampleRate);

    void pause();

    void resume();

    void stop();

    void release();

    void analysisStream(ThreadType threadType, AVFormatContext *pFormatContext);
};


#endif //NDK_DAY03_DZAUDIO_H
