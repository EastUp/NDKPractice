//
// Created by 123 on 2020/10/9.
//

#ifndef NDKPRACTICE_FFMPEG_H
#define NDKPRACTICE_FFMPEG_H

#include "JNICall.h"
#include <pthread.h>
#include "Audio.h"
#include "Video.h"

// 在 c++ 中采用 c 的这种编译方式
extern "C"{
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
}

class FFmpeg {
public:
    AVFormatContext *pFormatContext = NULL;
    char* url = NULL;
    JNICall *pJniCall = NULL;
    PlayerStatus *pPlayerStatus;
    Audio *pAudio = NULL;
    Video *pVideo = NULL;

public:
    FFmpeg(JNICall *pJniCall,const char *url);
    ~FFmpeg();

public:
    void play();

    void prepare();

    void prepareAsync();

    void prepareAudioTrack(ThreadMode threadMode);

    void prepareOpenSLES(ThreadMode threadMode);

    void callPlayerJniError(ThreadMode threadMode,int code,char *msg);

    void release();

    void setSurface(JNIEnv *env,jobject surface);
};


#endif //NDKPRACTICE_FFMPEG_H
