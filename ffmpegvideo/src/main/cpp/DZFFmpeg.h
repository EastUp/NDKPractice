//
// Created by 曾辉 on 2019-06-05.
//

#ifndef NDK_DAY03_DARRENFFMPEG_H
#define NDK_DAY03_DARRENFFMPEG_H

#include <jni.h>
#include "JNIPlayerCall.h"
#include <pthread.h>
#include "android_log.h"
#include "DZAudio.h"
#include "AVPacketQueue.h"
#include "DZConstDefine.h"
#include "DZVideo.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
};

class DZFFmpeg {
public:
    JNIPlayerCall *pPlayerCall = NULL;
    char *url = NULL;
    pthread_t preparedThread;
    AVFormatContext *av_format_context = NULL;
    DZAudio *audio = NULL;
    DZVideo *pVideo = NULL;
    // 跟退出相关，为了防止准备过程中退出，导致异常终止
    pthread_mutex_t releaseMutex;
    DZPlayStatus *pPlayStatus;

public:
    DZFFmpeg(JNIPlayerCall *jni_player_call, const char *url);

    ~DZFFmpeg();

    void prepared();

    void preparedAudio(ThreadType thread_type);

    void prepared_async();

    void start();

    void onPause();

    void onResume();

    void release();

    void releasePreparedRes(ThreadType threadType, int errorCode, const char *errorMsg);

    void seek(uint64_t seconds);

    void decodeFrame();
};


#endif //NDK_DAY03_DARRENFFMPEG_H
