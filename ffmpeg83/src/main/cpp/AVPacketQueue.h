//
// Created by 曾辉 on 2019-06-06.
//

#ifndef NDK_DAY03_AVPACKETQUEUE_H
#define NDK_DAY03_AVPACKETQUEUE_H

#include <queue>
#include <pthread.h>
#include "DZPlayStatus.h"
#include "android_log.h"
#include <malloc.h>

extern "C" {
#include "libavcodec/avcodec.h"
};

class AVPacketQueue {
public:
    AVPacketQueue(DZPlayStatus *play_status);

    ~AVPacketQueue();

public:
    std::queue<AVPacket *> packet_queue;
    pthread_mutex_t packet_mutex;
    pthread_cond_t packet_cond;
    DZPlayStatus *play_status;

public:
    int push(AVPacket *avPacket);

    int pop(AVPacket *av_packet);

    bool empty();

    void clear();
};


#endif //NDK_DAY03_AVPACKETQUEUE_H
