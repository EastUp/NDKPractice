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

public:
    LivePush(JNICall *pJniCall,const char *liveUrl);

    ~LivePush();

public:
    void initConnect();
};


#endif //NDKPRACTICE_LIVEPUSH_H
