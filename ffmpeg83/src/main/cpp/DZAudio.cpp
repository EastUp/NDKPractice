//
// Created by 曾辉 on 2019-06-06.
//

#include "DZAudio.h"

DZAudio::DZAudio(int audio_stream_index, int sampleRate, JNIPlayerCall *playerCall) {
    this->audio_stream_index = audio_stream_index;
    this->sampleRate = sampleRate;
    convertOutBuffer = (uint8_t * )
    malloc(sampleRate * 2 * 2);
    this->playerCall = playerCall;
    player_status = new DZPlayStatus();
    pthread_mutex_init(&seekMutex, NULL);
}

DZAudio::~DZAudio() {

}

void *threadDecodePlay(void *data) {
    DZAudio *audio = (DZAudio *) (data);

    audio->initOpenSLES();

    pthread_exit((void *) 1);
}

int DZAudio::resampleAudio() {
    int dataSize = 0;
    AVPacket *avPacket = av_packet_alloc();
    AVFrame *avFrame = av_frame_alloc();
    while (player_status != NULL && !player_status->isExit) {
        // 根据队列中是否有数据来判断是否加载中
        if (packet_queue != NULL && packet_queue->empty()) {
            if (player_status != NULL && player_status->isLoading != true) {
                player_status->isLoading = true;
                if (playerCall != NULL) {
                    playerCall->onCallLoading(THREAD_CHILD, player_status->isLoading);
                }
            }
            continue;
        } else {
            if (player_status != NULL && player_status->isLoading == true) {
                player_status->isLoading = false;
                if (playerCall != NULL) {
                    playerCall->onCallLoading(THREAD_CHILD, player_status->isLoading);
                }
            }
        }

        packet_queue->pop(avPacket);
        // 解码 avcodec_send_packet -> avcodec_receive_frame
        int send_packet_res = avcodec_send_packet(pCodecContext, avPacket);
        if (send_packet_res == 0) {
            int receive_frame_res = avcodec_receive_frame(pCodecContext, avFrame);
            if (receive_frame_res == 0) {
                int nb = swr_convert(swrContext, &convertOutBuffer, avFrame->nb_samples,(
                const uint8_t **) (avFrame->data), avFrame->nb_samples);
                int outChannels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
                dataSize = nb * outChannels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

                currentTime = avFrame->pts * av_q2d(timeBase);
                break;
            }
        }
        // 释放 data 数据，释放 AVPacket 开辟的内存
        av_packet_unref(avPacket);
        av_frame_unref(avFrame);
    }
    av_packet_free(&avPacket);
    av_frame_free(&avFrame);
    return dataSize;
}

void DZAudio::play() {
    pthread_create(&playThreadT, NULL, threadDecodePlay, this);
}

void bufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context) {
    DZAudio *audio = (DZAudio *) context;
    if (audio != NULL) {
        int bufferSize = audio->resampleAudio();
        if (bufferSize > 0) {
            audio->currentTime += bufferSize / ((double) (audio->sampleRate * 2 * 2));

            // 0.5 回调更新一次进度
            if (audio->currentTime - audio->lastTime > 1) {
                audio->lastTime = audio->currentTime;
                if (audio->playerCall != NULL) {
                    audio->playerCall->onCallProgress(THREAD_CHILD, audio->currentTime,
                                                      audio->duration);
                }
            }

            if (audio->duration > 0 && audio->duration <= audio->currentTime) {
                audio->playerCall->onCallComplete(THREAD_CHILD);
            }

            (*bufferQueueItf)->Enqueue(bufferQueueItf, (char *) audio->convertOutBuffer,
                                       bufferSize);
        }
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
            (SLuint32)getSampleRateForOpenSLES(sampleRate),
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource pAudioSrc = {&androidBufferQueue, &formatPcm};
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, mixObj};
    SLDataSink pAudioSnk = {&outputMix, NULL};
    const SLInterfaceID pInterfaceIds[2] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME};
    const SLboolean pInterfaceRequired[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    (*engine)->CreateAudioPlayer(engine, &pPlayer, &pAudioSrc, &pAudioSnk,
                                 2, pInterfaceIds, pInterfaceRequired);
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

    if (packet_queue != NULL) {
        delete (packet_queue);
        packet_queue = NULL;
    }

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


    if (pCodecContext != NULL) {
        avcodec_close(pCodecContext);
        avcodec_free_context(&pCodecContext);
    }
    playerCall = NULL;

    if (player_status != NULL) {
        delete player_status;
        player_status = NULL;
    }

    pthread_mutex_destroy(&seekMutex);
}

void *threadDecodeFrame(void *data) {
    DZAudio *audio = (DZAudio *) (data);
    int readFrameRes = 0;

    while (audio->player_status != NULL && !audio->player_status->isExit) {
        // 提取每一帧的音频流
        AVPacket *av_packet = av_packet_alloc();

        pthread_mutex_lock(&audio->seekMutex);
        readFrameRes = av_read_frame(audio->av_format_context, av_packet);
        pthread_mutex_unlock(&audio->seekMutex);

        if (readFrameRes >= 0) {
            // 必须要是音频流
            if (audio->audio_stream_index == av_packet->stream_index) {
                audio->packet_queue->push(av_packet);
            } else {
                av_packet_free(&av_packet);
            }
        } else {
            av_packet_free(&av_packet);
        }
    }
    pthread_exit((void *) 1);
}

void DZAudio::decodeFrame() {
    pthread_create(&decodeFrameThreadT, 0, threadDecodeFrame, this);
}

void DZAudio::seek(uint64_t seconds) {
    if (duration <= 0) {
        return;
    }

    if (seconds >= 0 && seconds < duration) {
        pthread_mutex_lock(&seekMutex);
        packet_queue->clear();
        lastTime = 0;
        currentTime = 0;
        int64_t rel = seconds * AV_TIME_BASE;
        av_seek_frame(av_format_context, -1, rel, AVSEEK_FLAG_BACKWARD);
        avcodec_flush_buffers(pCodecContext);
        pthread_mutex_unlock(&seekMutex);
    }
}
