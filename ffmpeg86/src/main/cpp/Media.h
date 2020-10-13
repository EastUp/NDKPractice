//
// Created by 123 on 2020/10/12.
//

#ifndef NDKPRACTICE_MEDIA_H
#define NDKPRACTICE_MEDIA_H

#include "JNICall.h"
#include "PacketQueue.h"
#include "PlayerStatus.h"
#include "ConstDefine.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

/**
 *  整合视频和音频的相同代码
 */
class Media {
public:
    int streamIndex = -1;
    AVCodecContext *pCodecContext = NULL;
    JNICall *pJniCall = NULL;
    PacketQueue *pPacketQueue = NULL;
    PlayerStatus *pPlayerStatus = NULL;

    /**
     *  整个视频时长,单位（微秒）
     */
     int duration = 0;

     /**
      *  记录当前播放时间
      */
     double currentTime = 0;

     /**
      *  上次更新的时间（回调到 java 层）
      */
      double lastUpdateTime = 0;

      /**
       *  时间基
       */
       AVRational timeBase;

public:
    Media(int streamIndex,JNICall *pJniCall,PlayerStatus *pPlayerStatus);

    ~Media();

public:

    virtual void play() = 0;

    void analysisStream(ThreadMode threadMode,AVFormatContext *pFormatContext);

    virtual void privateAnalysisStream(ThreadMode threadMode,AVFormatContext *pFormatContext) = 0;

    virtual void release() = 0;

    void callPlayerJniError(ThreadMode threadMode, int code, char* msg);

private:
    // 音、视频共有的解析流代码
    void publicAnalysisStream(ThreadMode threadMode,AVFormatContext *pFormatContext);

};


#endif //NDKPRACTICE_MEDIA_H
