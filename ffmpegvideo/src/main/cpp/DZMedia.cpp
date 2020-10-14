//
// Created by hcDarren on 2019/6/23.
//

#include "DZMedia.h"
#include "DZConstDefine.h"

DZMedia::DZMedia(int streamIndex, DZPlayStatus *pPlayStatus, JNIPlayerCall *pPlayerCall) {
    this->streamIndex = streamIndex;
    this->pPlayStatus = pPlayStatus;
    this->pPlayerCall = pPlayerCall;
    pPacketQueue = new AVPacketQueue(pPlayStatus);
    pthread_mutex_init(&seekMutex, NULL);
}

DZMedia::~DZMedia() {
    release();
}

void DZMedia::analysisStream(ThreadType threadType, AVFormatContext *pFormatContext) {
    AVCodecParameters *pCodecParameters = pFormatContext->streams[streamIndex]->codecpar;
    AVCodec *pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if (!pCodec) {
        LOGE("Can't find audio decoder");
        callPlayerJniError(threadType, FIND_AUDIO_DECODER_ERROR_CODE,
                           "Can't find audio decoder.");
        return;
    }

    pCodecContext = avcodec_alloc_context3(pCodec);
    int codecContextParametersRes = avcodec_parameters_to_context(pCodecContext, pCodecParameters);
    if (codecContextParametersRes < 0) {
        LOGE("codec parameters to_context error :%s", av_err2str(codecContextParametersRes));
        callPlayerJniError(threadType, codecContextParametersRes,
                           av_err2str(codecContextParametersRes));
        return;
    }

    int codecOpenRes = avcodec_open2(pCodecContext, pCodec, NULL);
    if (codecOpenRes < 0) {
        LOGE("codec open error : %s", av_err2str(codecOpenRes));
        callPlayerJniError(threadType, codecOpenRes, av_err2str(codecOpenRes));
        return;
    }

    duration = pFormatContext->duration / AV_TIME_BASE;
    timeBase = pFormatContext->streams[streamIndex]->time_base;
}


void DZMedia::callPlayerJniError(ThreadType threadType, int errorCode, const char *errorMsg) {
    release();
    if (pPlayerCall != NULL) {
        pPlayerCall->onCallError(threadType, errorCode, errorMsg);
    }
}

void DZMedia::release() {
    if (pCodecContext != NULL) {
        avcodec_close(pCodecContext);
        avcodec_free_context(&pCodecContext);
        pCodecContext = NULL;
    }

    if (pPacketQueue != NULL) {
        delete (pPacketQueue);
        pPacketQueue = NULL;
    }

    pthread_mutex_destroy(&seekMutex);
}

void DZMedia::seek(uint64_t seconds) {
    if (duration <= 0) {
        return;
    }

    pthread_mutex_lock(&seekMutex);

    if (seconds >= 0 && seconds < duration) {
        pPacketQueue->clear();
        lastUpdateTime = 0;
        currentTime = 0;
        avcodec_flush_buffers(pCodecContext);
    }

    pthread_mutex_unlock(&seekMutex);
}
