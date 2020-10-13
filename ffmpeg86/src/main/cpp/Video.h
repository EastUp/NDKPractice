//
// Created by 123 on 2020/10/12.
//

#ifndef NDKPRACTICE_VIDEO_H
#define NDKPRACTICE_VIDEO_H


#include "Media.h"
#include "Audio.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>

extern "C"{
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavutil/time.h"
}

class Video: public Media {
public:
    SwsContext *pSwsContext = nullptr;
    uint8_t *pFrameBuffer = nullptr;
    int frameSize;
    AVFrame *pRgbaFrame;
    jobject surface;
    Audio *pAudio;
    /**
     * 视频的延时时间
     */
    double delayTime = 0;

    /**
     * 默认情况下最合适的一个延迟时间，动态获取
     */
     double defaultDelayTime = 0.04;
public:
    Video(int audioStreamIndex,JNICall *pJniCall, PlayerStatus *pPlayerStatus,Audio *pAudio);

    ~Video();

public:
    void play();

    void privateAnalysisStream(ThreadMode threadMode,AVFormatContext *pFormatContext);

    void release();

    void setSurface(jobject surface);

    /**
     * 视音频同步，计算获取休眠的时间
     * @param pFrame 当前视频帧
     * @return 休眠时间(s)
     */
    double getFrameSleepTime(AVFrame *pFrame);
};


#endif //NDKPRACTICE_VIDEO_H
