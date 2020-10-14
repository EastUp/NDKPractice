//
// Created by hcDarren on 2019/6/23.
//

#ifndef NDK_DAY03_DZVIDEO_H
#define NDK_DAY03_DZVIDEO_H

#include "DZPlayStatus.h"
#include "JNIPlayerCall.h"
#include "DZMedia.h"
#include "DZConstDefine.h"
#include <pthread.h>
#include "DZAudio.h"

extern "C" {
#include "../jniLibs/include/libavutil/imgutils.h"
#include "../jniLibs/include/libswscale/swscale.h"
#include "../jniLibs/include/libavutil/time.h"
};

class DZVideo : public DZMedia {
public:
    AVFrame *pFrameYUV420P = NULL;
    uint8_t *pFrameBuffer = NULL;
    SwsContext *pSwsContext = NULL;
    DZAudio *pAudio = NULL;
    double delayTime = 0;
    double defaultDelayTime = 0.04;
    bool supportStiffCodec = false;
    AVBSFContext *pBSFContext;
public:
    DZVideo(int videoStreamIndex, DZPlayStatus *pPlayStatus, JNIPlayerCall *pPlayerCall,
            DZAudio *pAudio);

    ~DZVideo();

public:
    void play();

    virtual void analysisStream(ThreadType threadType, AVFormatContext *pFormatContext);

    void release();

    double getFrameSleepTime(int64_t pts);
};


#endif //NDK_DAY03_DZVIDEO_H
