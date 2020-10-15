//
// Created by 123 on 2020/10/12.
//

#ifndef NDKPRACTICE_PACKETQUEUE_H
#define NDKPRACTICE_PACKETQUEUE_H

#include <queue>
#include <pthread.h>

extern "C"{
    #include "rtmp/rtmp.h"
}

using namespace std;

class PacketQueue {
public:
    queue<RTMPPacket *> *pPacketQueue;
    pthread_mutex_t packetMutex;
    pthread_cond_t packetCond;

public:
    PacketQueue();

    ~PacketQueue();

public:
    void push(RTMPPacket *pPacket);

    RTMPPacket *pop();

    /**
     * 清除整个队列
     */
     void clear();
};


#endif //NDKPRACTICE_PACKETQUEUE_H
