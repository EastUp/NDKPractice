//
// Created by 曾辉 on 2019-06-06.
//
#include "DZAudio.h"
#include "../jniLibs/include/libavcodec/avcodec.h"

DZAudio::DZAudio(int audioStreamIndex, DZPlayStatus *pPlayStatus, JNIPlayerCall *pPlayerCall)
        : DZMedia(audioStreamIndex, pPlayStatus, pPlayerCall) {
}

DZAudio::~DZAudio() {
    release();
}

void DZAudio::analysisStream(ThreadType threadType, AVFormatContext *pFormatContext) {
    DZMedia::analysisStream(threadType, pFormatContext);

    // 处理一些异常的问题
    if (pCodecContext->channels > 0 && pCodecContext->channel_layout == 0) {
        pCodecContext->channel_layout = av_get_default_channel_layout(pCodecContext->channels);
    } else if (pCodecContext->channels == 0 && pCodecContext->channel_layout > 0) {
        pCodecContext->channels = av_get_channel_layout_nb_channels(pCodecContext->channel_layout);
    }

    // 初始化 SwrContext
    /*struct SwrContext *s; 用返回值来接收了，直接传 NULL
    int64_t out_ch_layout; 输出的声道布局
    enum AVSampleFormat out_sample_fmt; 输出的采样位数
    int out_sample_rate; 输出的采样率
    int64_t in_ch_layout; 输入的声道布局
    enum AVSampleFormat in_sample_fmt; 输入的采样位数
    int in_sample_rate; 输入的采样率
    int log_offset;
    void *log_ctx;*/
    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    int out_sample_rate = pCodecContext->sample_rate;
    uint64_t in_ch_layout = pCodecContext->channel_layout;
    enum AVSampleFormat in_sample_fmt = pCodecContext->sample_fmt;
    int in_sample_rate = pCodecContext->sample_rate;
    swrContext = swr_alloc_set_opts(NULL, out_ch_layout, out_sample_fmt,
                                    out_sample_rate, in_ch_layout, in_sample_fmt,
                                    in_sample_rate, 0, 0);
    int swrInitRes = swr_init(swrContext);
    if (swrContext == NULL || swrInitRes < 0) {
        LOGE("init SwrContext error : %s", av_err2str(swrInitRes));
        callPlayerJniError(threadType, swrInitRes, av_err2str(swrInitRes));
        return;
    }

    frameBufferSize = pCodecContext->frame_size * 2 * 2;
    convertOutBuffer = (uint8_t *) malloc(frameBufferSize);
}

void *threadDecodePlay(void *data) {
    DZAudio *audio = (DZAudio *) (data);
    LOGE("audio -> %p", audio);
    audio->initOpenSLES();
    pthread_exit((void *) 1);
}

void DZAudio::resampleAudio() {
    AVPacket *avPacket = av_packet_alloc();
    AVFrame *avFrame = av_frame_alloc();
    while (pPlayStatus != NULL && !pPlayStatus->isExit) {
        // 是不是暂停或者 seek 中
        if (pPlayStatus != NULL) {
            if (pPlayStatus->isPause || pPlayStatus->isSeek) {
                av_usleep(10 * 1000);
                continue;
            }
        }

        // 根据队列中是否有数据来判断是否加载中
        if (pPacketQueue != NULL && pPacketQueue->empty()) {
            if (pPlayStatus != NULL && pPlayStatus->isLoading != true) {
                pPlayStatus->isLoading = true;
                if (pPlayerCall != NULL) {
                    pPlayerCall->onCallLoading(THREAD_CHILD, pPlayStatus->isLoading);
                }
            }
            continue;
        } else {
            if (pPlayStatus != NULL && pPlayStatus->isLoading == true) {
                pPlayStatus->isLoading = false;
                if (pPlayerCall != NULL) {
                    pPlayerCall->onCallLoading(THREAD_CHILD, pPlayStatus->isLoading);
                }
            }
        }

        // 加锁，防止 seek 时获取到脏数据
        pthread_mutex_lock(&seekMutex);
        pPacketQueue->pop(avPacket);

        // 解码 avcodec_send_packet -> avcodec_receive_frame
        int send_packet_res = avcodec_send_packet(pCodecContext, avPacket);
        if (send_packet_res == 0) {
            int receive_frame_res = avcodec_receive_frame(pCodecContext, avFrame);
            pthread_mutex_unlock(&seekMutex);
            if (receive_frame_res == 0) {
                swr_convert(swrContext, &convertOutBuffer, avFrame->nb_samples, (
                        const uint8_t **) (avFrame->data), avFrame->nb_samples);
                int time = avFrame->pts * av_q2d(timeBase);
                if (time > currentTime) {
                    currentTime = time;
                }
                break;
            }
        } else{
            pthread_mutex_unlock(&seekMutex);
        }

        // 释放 data 数据，释放 AVPacket 开辟的内存
        av_packet_unref(avPacket);
        av_frame_unref(avFrame);
    }
    av_packet_free(&avPacket);
    av_frame_free(&avFrame);
}

void DZAudio::play() {
    pthread_create(&playThreadT, NULL, threadDecodePlay, this);
}

void bufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context) {
    DZAudio *audio = (DZAudio *) context;
    if (audio != NULL) {
        audio->resampleAudio();
        audio->currentTime +=
                audio->frameBufferSize / ((double) (audio->pCodecContext->sample_rate * 2 * 2));
        // 0.5 回调更新一次进度
        if (audio->currentTime - audio->lastUpdateTime > 1) {
            audio->lastUpdateTime = audio->currentTime;
            if (audio->pPlayerCall != NULL) {
                audio->pPlayerCall->onCallProgress(THREAD_CHILD, audio->currentTime,
                                                   audio->duration);
            }
        }

        if (audio->duration > 0 && audio->duration <= audio->currentTime) {
            audio->pPlayerCall->onCallComplete(THREAD_CHILD);
        }

        (*bufferQueueItf)->Enqueue(bufferQueueItf, (char *) audio->convertOutBuffer,
                                   audio->frameBufferSize);
    }
}

int DZAudio::getSampleRateForOpenSLES(int sampleRate) {
    return sampleRate * 1000;
}

void DZAudio::initOpenSLES() {
    SLEngineItf engine;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean interfaceRequired[1] = {SL_BOOLEAN_FALSE};
    // 创建引擎
    slCreateEngine(&engineObj, 0, 0, 0, 0, 0);
    (*engineObj)->Realize(engineObj, SL_BOOLEAN_FALSE);
    (*engineObj)->GetInterface(engineObj, SL_IID_ENGINE, &engine);
    // 创建混音器
    (*engine)->CreateOutputMix(engine, &mixObj, 1, ids, interfaceRequired);
    (*mixObj)->Realize(mixObj, SL_BOOLEAN_FALSE);
    (*mixObj)->GetInterface(mixObj, SL_IID_ENVIRONMENTALREVERB, &outputMixEnvironmentalReverb);
    (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
            outputMixEnvironmentalReverb, &reverbSettings);
    // 创建播放器
    SLDataLocator_AndroidBufferQueue androidBufferQueue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
            2};
    SLDataFormat_PCM formatPcm = {
            SL_DATAFORMAT_PCM,
            2,
            (SLuint32)getSampleRateForOpenSLES(pCodecContext->sample_rate),
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource pAudioSrc = {&androidBufferQueue, &formatPcm};
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, mixObj};
    SLDataSink pAudioSnk = {&outputMix, NULL};
    const SLInterfaceID pInterfaceIds[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_PLAYBACKRATE};
    const SLboolean pInterfaceRequired[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    (*engine)->CreateAudioPlayer(engine, &pPlayer, &pAudioSrc, &pAudioSnk,
                                 3, pInterfaceIds, pInterfaceRequired);
    (*pPlayer)->Realize(pPlayer, SL_BOOLEAN_FALSE);
    (*pPlayer)->GetInterface(pPlayer, SL_IID_PLAY, &slPlayItf);
    // 设置缓冲
    (*pPlayer)->GetInterface(pPlayer, SL_IID_BUFFERQUEUE, &androidBufferQueueItf);
    (*androidBufferQueueItf)->RegisterCallback(androidBufferQueueItf, bufferQueueCallback, this);
    (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_PLAYING);

    //第六步----------------------------------------
    // 主动调用回调函数开始工作
    bufferQueueCallback(androidBufferQueueItf, this);
}

void DZAudio::pause() {
    if (slPlayItf != NULL) {
        (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_PAUSED);
    }
}

void DZAudio::resume() {
    if (slPlayItf != NULL) {
        (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_PLAYING);
    }
}

void DZAudio::stop() {
    if (slPlayItf != NULL) {
        (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_STOPPED);
    }
}

void DZAudio::release() {
    stop();

    if (pPlayer != NULL) {
        (*pPlayer)->Destroy(pPlayer);
        pPlayer = NULL;
    }

    if (mixObj != NULL) {
        (*mixObj)->Destroy(mixObj);
        mixObj = NULL;
    }

    if (engineObj != NULL) {
        (*engineObj)->Destroy(engineObj);
        engineObj = NULL;
    }

    free(convertOutBuffer);
    if (swrContext != NULL) {
        swr_free(&swrContext);
        swrContext = NULL;
    }
}
