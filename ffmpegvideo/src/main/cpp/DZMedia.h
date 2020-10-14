//
// Created by hcDarren on 2019/6/23.
//

#ifndef NDK_DAY03_DZMEDIA_H
#define NDK_DAY03_DZMEDIA_H

#include "JNIPlayerCall.h"
#include "DZPlayStatus.h"
#include "AVPacketQueue.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
};

/**
 * 基类，公共内容
 */
class DZMedia {
public:
    /**
     * 当前流的角标（音频/视频/字幕）
     */
    int streamIndex;
    /**
     * 解码器上下文
     */
    AVCodecContext *pCodecContext = NULL;
    /**
     * 回调 Java 层的 Call
     */
    JNIPlayerCall *pPlayerCall = NULL;
    /**
     * 播放状态
     */
    DZPlayStatus *pPlayStatus = NULL;
    /**
     * AVPacket 队列
     */
    AVPacketQueue *pPacketQueue = NULL;
    /**
     * 整个视频的时长
     */
    int duration = 0;
    /**
     * 当前播放的时长
     */
    double currentTime = 0;

    /**
     * 上次更新的时间，主要用于控制回调到 Java 层的频率
     */
    double lastUpdateTime = 0;

    /**
     * 时间机
     */
    AVRational timeBase;

    /**
     * seek 时的 mutex
     */
    pthread_mutex_t seekMutex;

public:
    DZMedia(int streamIndex, DZPlayStatus *pPlayStatus, JNIPlayerCall *pPlayerCall);

    ~DZMedia();

public:
    /**
     * 播放方法，纯虚函数
     */
    virtual void play() = 0;

    /**
     * 解析公共的解码器上下文
     */
    virtual void analysisStream(ThreadType threadType, AVFormatContext *pFormatContext);

    /**
     * 准备解析数据过程中出错的回调
     * @param threadType 线程类型
     * @param errorCode 错误码
     * @param errorMsg 错误信息
     */
    void callPlayerJniError(ThreadType threadType, int errorCode, const char *errorMsg);

    /**
     * 释放资源
     */
    void release();

    /**
     * seek到当前时间
     * @param seconds 秒
     */
    virtual void seek(uint64_t seconds);
};


#endif //NDK_DAY03_DZMEDIA_H
