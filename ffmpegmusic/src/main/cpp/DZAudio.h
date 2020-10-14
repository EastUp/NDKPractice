//
// Created by east on 2020-10-14.
//

#ifndef NDK_DAY03_DZAUDIO_H
#define NDK_DAY03_DZAUDIO_H

#include "AVPacketQueue.h"
#include "JNIPlayerCall.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/frame.h"
#include "libswresample/swresample.h"
#include "libavutil/channel_layout.h"
#include "libavutil/rational.h"
};


class DZAudio {
public:
    int audio_stream_index;
    pthread_t playThreadT;
    DZAudio *audio = NULL;
    AVPacketQueue *packet_queue = NULL;
    DZPlayStatus *player_status = NULL;
    AVCodecContext *pCodecContext = NULL;
    uint8_t *convertOutBuffer = NULL;
    SwrContext *swrContext = NULL;
    int sampleRate;
    JNIPlayerCall *playerCall = NULL;
    SLPlayItf slPlayItf = NULL;
    int duration = 0;
    AVRational timeBase;
    double currentTime = 0;
    int lastTime = 0;
    SLObjectItf pPlayer = NULL;
    SLObjectItf mixObj = NULL;
    SLObjectItf engineObj = NULL;
    pthread_t decodeFrameThreadT;
    AVFormatContext *av_format_context;
    pthread_mutex_t seekMutex;
    SLAndroidSimpleBufferQueueItf androidBufferQueueItf;

public:
    DZAudio(int audio_stream_index, int sampleRate,JNIPlayerCall *playerCall);

    ~DZAudio();

    void play();

    int resampleAudio();


    void initOpenSLES();

    int getSampleRateForOpenSLES(int sampleRate);

    void pause();

    void resume();

    void stop();

    void release();

    void decodeFrame();

    void seek(uint64_t seconds);
};


#endif //NDK_DAY03_DZAUDIO_H
