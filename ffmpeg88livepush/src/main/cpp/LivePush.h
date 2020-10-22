//
// Created by 123 on 2020/10/15.
//

#ifndef NDKPRACTICE_LIVEPUSH_H
#define NDKPRACTICE_LIVEPUSH_H

#include "JNICall.h"
#include "PacketQueue.h"
#include "ConstDefine.h"

extern "C"{
#include "rtmp/rtmp.h"
}

class LivePush {
public:
    JNICall *pJniCall = nullptr;
    char *liveUrl = nullptr;
    PacketQueue *pPacketQueue = nullptr;
    RTMP *pRtmp = nullptr;
    bool isPushing = true;
    uint32_t startTime;
    pthread_t initConnecTid;
public:
    LivePush(JNICall *pJniCall,const char *liveUrl);

    ~LivePush();

public:
    void initConnect();

    void pushSpsPps(jbyte *spsData, jint spsLen, jbyte *ppsData, jint ppsLen);

    void pushVideoData(jbyte *videoData, jint dataLen, jboolean keyFrame);

    void pushAudioData(jbyte *audioData, jint dataLen);

    void stop();
};


#endif //NDKPRACTICE_LIVEPUSH_H
