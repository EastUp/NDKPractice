//
// Created by 曾辉 on 2019-06-05.
//
#include "DZFFmpeg.h"

DZFFmpeg::DZFFmpeg(JNIPlayerCall *jni_player_call, const char *url) {
    this->pPlayerCall = jni_player_call;
    // 复制 url
    this->url = (char *) (malloc(strlen(url) + 1));
    memcpy(this->url, url, strlen(url) + 1);
    pthread_mutex_init(&releaseMutex, NULL);
    pPlayStatus = new DZPlayStatus();
}

void *decodeAudioThread(void *data) {
    DZFFmpeg *fFmpeg = (DZFFmpeg *) data;
    fFmpeg->preparedAudio(THREAD_CHILD);
    pthread_exit(0);
}

void DZFFmpeg::prepared() {
    preparedAudio(THREAD_MAIN);
}

void DZFFmpeg::prepared_async() {
    pthread_create(&preparedThread, NULL, decodeAudioThread, this);
}

void DZFFmpeg::preparedAudio(ThreadType thread_type) {
    pthread_mutex_lock(&releaseMutex);

    // av_register_all: 作用是初始化所有组件，只有调用了该函数，才能使用复用器和编解码器（源码）
    av_register_all();
    avformat_network_init();
    int format_open_res = 0;
    int format_find_stream_info_res = 0;
    int audio_stream_index = 0;

    format_open_res = avformat_open_input(&av_format_context, url, NULL, NULL);
    if (format_open_res < 0) {
        LOGE("Can't open url : %s, %s", url, av_err2str(format_open_res));
        releasePreparedRes(thread_type, format_open_res, av_err2str(format_open_res));
        return;
    }

    format_find_stream_info_res = avformat_find_stream_info(av_format_context, NULL);
    if (format_find_stream_info_res < 0) {
        LOGE("Can't find stream info url : %s, %s", url, av_err2str(format_find_stream_info_res));
        releasePreparedRes(thread_type, format_find_stream_info_res,
                           av_err2str(format_find_stream_info_res));
        return;
    }

    audio_stream_index = av_find_best_stream(av_format_context, AVMEDIA_TYPE_AUDIO, -1, -1, NULL,
                                             0);
    if (audio_stream_index < 0) {
        LOGE("Can't find audio stream info url : %s", url);
        releasePreparedRes(thread_type, FIND_AUDIO_STREAM_ERROR_CODE,
                           "Can't find audio stream info url.");
        return;
    }

    audio = new DZAudio(audio_stream_index, pPlayStatus, pPlayerCall);
    audio->analysisStream(thread_type, av_format_context);

    int videoStreamIndex = av_find_best_stream(av_format_context, AVMEDIA_TYPE_VIDEO, -1, -1, NULL,
                                               0);
    if (videoStreamIndex < 0) {
        LOGE("Can't find video stream info url : %s", url);
        releasePreparedRes(thread_type, FIND_VIDEO_STREAM_ERROR_CODE,
                           "Can't find video stream info url.");
        return;
    }
    pVideo = new DZVideo(videoStreamIndex, pPlayStatus, pPlayerCall, audio);
    pVideo->analysisStream(thread_type, av_format_context);

    pPlayerCall->onCallPrepared(thread_type);
    pthread_mutex_unlock(&releaseMutex);
}

DZFFmpeg::~DZFFmpeg() {

}

void *threadDecodeFrame(void *data) {
    DZFFmpeg *pFFmpeg = (DZFFmpeg *) (data);
    int readFrameRes = 0;

    while (pFFmpeg->pPlayStatus != NULL && !pFFmpeg->pPlayStatus->isExit) {
        // 提取每一帧的音频流
        AVPacket *av_packet = av_packet_alloc();
        readFrameRes = av_read_frame(pFFmpeg->av_format_context, av_packet);
        if (readFrameRes >= 0) {
            // 必须要是音频流
            if (pFFmpeg->audio->streamIndex == av_packet->stream_index) {
                pFFmpeg->audio->pPacketQueue->push(av_packet);
            } else if (pFFmpeg->pVideo->streamIndex == av_packet->stream_index) {
                pFFmpeg->pVideo->pPacketQueue->push(av_packet);
            } else {
                av_packet_free(&av_packet);
            }
        } else {
            av_packet_free(&av_packet);
        }
    }
    pthread_exit((void *) 1);
}

void DZFFmpeg::start() {
    if (audio == NULL) {
        LOGE("DZAudio is null , prepared may be misleading");
        return;
    }
    if (pVideo == NULL) {
        LOGE("DZVideo is null , prepared may be misleading");
        return;
    }
    this->decodeFrame();
    audio->play();
    pVideo->play();
}

void DZFFmpeg::onPause() {
    if (audio != NULL) {
        audio->pause();
    }

    if (pPlayStatus != NULL) {
        pPlayStatus->isPause = true;
    }
}

void DZFFmpeg::onResume() {
    if (audio != NULL) {
        audio->resume();
    }

    if (pPlayStatus != NULL) {
        pPlayStatus->isPause = false;
    }
}

void DZFFmpeg::release() {

    pthread_mutex_lock(&releaseMutex);

    if (audio->pPlayStatus->isExit) {
        return;
    }

    audio->pPlayStatus->isExit = true;

    if (audio != NULL) {
        audio->release();
        delete audio;
        audio = NULL;
    }

    if (pVideo != NULL) {
        pVideo->release();
        delete pVideo;
        pVideo = NULL;
    }

    if (av_format_context != NULL) {
        avformat_close_input(&av_format_context);
        avformat_free_context(av_format_context);
        av_format_context = NULL;
    }

    pPlayerCall = NULL;
    free(url);

    pthread_mutex_unlock(&releaseMutex);
    pthread_mutex_destroy(&releaseMutex);
}

void DZFFmpeg::releasePreparedRes(ThreadType threadType, int errorCode, const char *errorMsg) {
    pthread_mutex_unlock(&releaseMutex);
    if (av_format_context != NULL) {
        avformat_close_input(&av_format_context);
        avformat_free_context(av_format_context);
        av_format_context = NULL;
    }

    if (pPlayerCall != NULL) {
        pPlayerCall->onCallError(threadType, errorCode, errorMsg);
    }

    pthread_mutex_destroy(&releaseMutex);
    free(url);
}

void DZFFmpeg::seek(uint64_t seconds) {
    if (pPlayStatus != NULL) {
        pPlayStatus->isSeek = true;
    }

    if (seconds >= 0) {
        int64_t rel = seconds * AV_TIME_BASE;
        av_seek_frame(av_format_context, -1, rel, AVSEEK_FLAG_BACKWARD);
    }

    if (pVideo != NULL) {
        pVideo->seek(seconds);
    }

    if (audio != NULL) {
        audio->seek(seconds);
    }

    if (pPlayStatus != NULL) {
        pPlayStatus->isSeek = false;
    }
}

void DZFFmpeg::decodeFrame() {
    pthread_t decodeFrameThreadT;
    pthread_create(&decodeFrameThreadT, NULL, threadDecodeFrame, this);
}

