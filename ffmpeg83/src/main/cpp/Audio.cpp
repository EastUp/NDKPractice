//
// Created by 123 on 2020/10/10.
//

#include "Audio.h"
#include <pthread.h>

Audio::Audio(int audioStreamIndex, JNICall *pJniCall, AVCodecContext *pCodecContext,
             AVFormatContext *pFormatContext,SwrContext *pSwrContext) {
    this->audioStreamIndex =audioStreamIndex;
    this->pJniCall = pJniCall;
    this->pCodecContext = pCodecContext;
    this->pFormatContext = pFormatContext;
    this->pSwrContext = pSwrContext;
    this->resampleOutBuffer = (uint8_t *)(malloc(pCodecContext->frame_size * 2 * 2));
}

void *threadPlay(void *args){
    Audio *pAudio = (Audio*)args;
    pAudio->initCreateOpenSLES();
    return nullptr;
}

void Audio::play() {
    // 创建一个县城去播放，多线程边解码边播放
    pthread_t playThreadT;
    pthread_create(&playThreadT,NULL,threadPlay,this);
    int *retval = 0;
    pthread_join(playThreadT,(void **)&retval);
}

int Audio::resampleAudio() {
    int dataSize = 0;
    AVPacket *pPacket = av_packet_alloc();
    AVFrame *pFrame = av_frame_alloc();

    // 循环从上下文中读取帧到包中
    while (av_read_frame(pFormatContext, pPacket) >= 0) {
        if (pPacket->stream_index == audioStreamIndex) {
            // Packet 包，压缩的数据，解码成 pcm 数据
            int codecSendPacketRes = avcodec_send_packet(pCodecContext, pPacket);
            if (codecSendPacketRes == 0) {
                int codecReceiveFrameRes = avcodec_receive_frame(pCodecContext, pFrame);
                if (codecReceiveFrameRes == 0) {
                    // AVPacket -> AVFrame


                    // 调用重采样的方法
                    dataSize = swr_convert(pSwrContext, &resampleOutBuffer, pFrame->nb_samples,
                                (const uint8_t **) pFrame->data, pFrame->nb_samples);
                    LOGE("解码音频帧：%d %d",dataSize,pFrame->nb_samples);

                    dataSize = pFrame->nb_samples * 2 * 2; // 采样率 * 通道数 * 两字节
                    // write 写到缓冲区 pFrame.data -> javabyte
                    // size 是多大，装 pcm 的数据
                    // 1s 44100 点，2通道， 2字节 44100*2*2
                    // 1帧不是一秒，pFrame->nb_samples点
                    break;
                }
            }
        }
        // 解引用
        av_packet_unref(pPacket);
        av_frame_unref(pFrame);
    }

    // 1.解引用数据 data, 2.销魂 pPacket 结构体内存， 3.pPacket = NULL;
    av_packet_free(&pPacket);
    av_frame_free(&pFrame);
    return dataSize;
}

void playerCallback(
        SLAndroidSimpleBufferQueueItf caller,
        void *pContext
){
    Audio *pAudio = (Audio*)pContext;
    int dataSize = pAudio->resampleAudio();
    (*caller)->Enqueue(caller,pAudio->resampleOutBuffer,dataSize);
}

void Audio::initCreateOpenSLES() {
    /*OpenSLES OpenGLES 都是自带的
    XXXES 与 XXX 之间可以说是基本没有区别，区别就是 XXXES 是 XXX 的精简
    而且他们都有一定规则，命名规则 slXXX() , glXXX3f*/
    // 3.1 创建引擎接口对象
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine;
    slCreateEngine(&engineObject,0,NULL,0,NULL,NULL);
    // realize the engine
    (*engineObject)->Realize(engineObject,SL_BOOLEAN_FALSE);
    // get the engine interface, which is needed in order to create other objects
    (*engineObject)->GetInterface(engineObject,SL_IID_ENGINE,&engineEngine);
    // 3.2 设置混音器
    static SLObjectItf outputMixObject = NULL;
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    (*outputMixObject)->Realize(outputMixObject,SL_BOOLEAN_FALSE);
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    (*outputMixObject)->GetInterface(outputMixObject,SL_IID_ENVIRONMENTALREVERB,
            &outputMixEnvironmentalReverb);
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
            outputMixEnvironmentalReverb, &reverbSettings);
    // 3.3 创建播放器
    SLObjectItf pPlayer = NULL;
    SLPlayItf pPlayItf = NULL;
    SLDataLocator_AndroidSimpleBufferQueue simpleBufferQueue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM formatPcm = {SL_DATAFORMAT_PCM,
                                   2, //  2个通道
                                   SL_SAMPLINGRATE_44_1, // 采样率是 44100
                                   SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT, // 左右立体声道
                                   SL_BYTEORDER_LITTLEENDIAN};
    SLDataSource audioSrc = {&simpleBufferQueue, &formatPcm};
    // configure audio sink
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&outputMix, NULL};
    // create audio player
    const SLInterfaceID interfaceIds[3] = {SL_IID_BUFFERQUEUE, SL_IID_MUTESOLO, SL_IID_VOLUME}; // 官网上的SL_IID_SEEK 需要改为 SL_IID_BUFFERQUEUE
    const SLboolean interfaceRequired[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    (*engineEngine)->CreateAudioPlayer(engineEngine, &pPlayer, &audioSrc, &audioSnk,
                                       3, interfaceIds, interfaceRequired);
    (*pPlayer)->Realize(pPlayer,SL_BOOLEAN_FALSE);
    (*pPlayer)->GetInterface(pPlayer,SL_IID_PLAY,&pPlayItf);
    // 3.4 设置缓存队列和回调函数
    static SLAndroidSimpleBufferQueueItf playerBufferQueue;
    (*pPlayer)->GetInterface(pPlayer, SL_IID_BUFFERQUEUE,&playerBufferQueue);
    // 每次回调 this 会被带给 playerCallback 里面的 context
    (*playerBufferQueue)->RegisterCallback(playerBufferQueue, playerCallback, this);
    // 3.5 设置播放状态
    (*pPlayItf)->SetPlayState(pPlayItf, SL_PLAYSTATE_PLAYING);
    // 3.6 调用回调函数
    playerCallback(playerBufferQueue,this);
}