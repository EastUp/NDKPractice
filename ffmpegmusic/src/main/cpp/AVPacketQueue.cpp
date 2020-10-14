//
// Created by east on 2020-10-14.
//

#include "AVPacketQueue.h"


int AVPacketQueue::push(AVPacket *av_packet) {

    pthread_mutex_lock(&packet_mutex);

    packet_queue.push(av_packet);

    // LOGE("放入 packet_queue 队列，个数为 %d", packet_queue.size());

    pthread_cond_signal(&packet_cond);

    pthread_mutex_unlock(&packet_mutex);

    return 0;
}


int AVPacketQueue::pop(AVPacket *av_packet) {

    pthread_mutex_lock(&packet_mutex);

    while (play_status != NULL && !play_status->isExit) {
        if (packet_queue.size() > 0) {

            AVPacket *packet = packet_queue.front();
            if (av_packet_ref(av_packet, packet) == 0) {
                packet_queue.pop();
            }
            // 并没有释放 data ，只是解引用了 data
            av_packet_free(&packet);

            // LOGE("从 packet_queue 队列移除，个数为 %d", packet_queue.size());
            break;
        } else {
            pthread_cond_wait(&packet_cond, &packet_mutex);
        }
    }

    pthread_mutex_unlock(&packet_mutex);

    return 0;
}

AVPacketQueue::AVPacketQueue(DZPlayStatus *play_status) {
    pthread_mutex_init(&packet_mutex, 0);
    pthread_cond_init(&packet_cond, 0);
    this->play_status = play_status;
}

AVPacketQueue::~AVPacketQueue() {
    clear();
    pthread_mutex_destroy(&packet_mutex);
    pthread_cond_destroy(&packet_cond);
}

bool AVPacketQueue::empty() {
    return packet_queue.empty();
}

void AVPacketQueue::clear() {
    pthread_cond_signal(&packet_cond);
    pthread_mutex_lock(&packet_mutex);

    while (!packet_queue.empty()){
        AVPacket *avPacket = packet_queue.front();
        packet_queue.pop();
        av_packet_free(&avPacket);
    }

    pthread_mutex_unlock(&packet_mutex);
}


