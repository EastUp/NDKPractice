//
// Created by 123 on 2020/10/12.
//

#include "PacketQueue.h"

PacketQueue::PacketQueue() {
    pPacketQueue = new queue<AVPacket *>();
    pthread_mutex_init(&packetMutex,NULL);
    pthread_cond_init(&packetCond,NULL);
}

PacketQueue::~PacketQueue() {
    if(pPacketQueue){
        clear();
        delete(pPacketQueue);
        pPacketQueue = NULL;
    }
    pthread_mutex_destroy(&packetMutex);
    pthread_cond_destroy(&packetCond);
}

void PacketQueue::push(AVPacket *pPacket) {
    pthread_mutex_lock(&packetMutex);
    pPacketQueue->push(pPacket);
    pthread_cond_signal(&packetCond);
    pthread_mutex_unlock(&packetMutex);
}

AVPacket *PacketQueue::pop() {
    pthread_mutex_lock(&packetMutex);
    while(pPacketQueue->empty()){ // 如果队列是空的，则等待队列中有数据
        pthread_cond_wait(&packetCond,&packetMutex);
    }
    AVPacket *avPacket = pPacketQueue->front();
    pPacketQueue->pop();
    pthread_mutex_unlock(&packetMutex);
    return avPacket;
}

void PacketQueue::clear() {
    // 需要清除队列，还需要清除每个 AVPacket* 的内存数据
    pthread_mutex_lock(&packetMutex);
    while(!pPacketQueue->empty()){
        AVPacket *avPacket = pPacketQueue->front();
        pPacketQueue->pop();
        av_packet_unref(avPacket);
        av_packet_free(&avPacket);
    }
    pthread_mutex_unlock(&packetMutex);
}


