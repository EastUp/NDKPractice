//
// Created by 123 on 2020/10/9.
//

#include "FFmpeg.h"
#include "ConstDefine.h"

FFmpeg::FFmpeg(JNICall *pJniCall, const char *url) {
    this->pJniCall = pJniCall;
    // 如果是在子线程进行的播放，主线程释放掉了url 子线程拿到的也就是空了
    // 解决：重新复制一份，怕外面方法结束销毁了 url
    this->url = (char*)malloc(strlen(url)+1);
    memcpy(this->url, url, strlen(url) + 1);

    pPlayerStatus = new PlayerStatus();
}

FFmpeg::~FFmpeg() {
    release();
}

void *threadReadPacket(void *args){
    FFmpeg *pFFmpeg = (FFmpeg*)args;
    while(pFFmpeg->pAudio->pPlayerStatus != NULL && !pFFmpeg->pAudio->pPlayerStatus->isExit){
        AVPacket *pPacket = av_packet_alloc();
        // 循环从上下文中读取帧到包中
        if (av_read_frame(pFFmpeg->pFormatContext, pPacket) >= 0) {
            if (pPacket->stream_index == pFFmpeg->pAudio->streamIndex) {
                // 读取音频压缩包数据后，将其push 到 队列中
                pFFmpeg->pAudio->pPacketQueue->push(pPacket);
            } else  if (pPacket->stream_index == pFFmpeg->pVideo->streamIndex) {
                // 读取视频压缩包数据后，将其push 到 队列中
                pFFmpeg->pVideo->pPacketQueue->push(pPacket);
            }else{
                // 1.解引用数据 data, 2.销魂 pPacket 结构体内存， 3.pPacket = NULL;
                av_packet_free(&pPacket);
            }
        }else{
            // 1.解引用数据 data, 2.销魂 pPacket 结构体内存， 3.pPacket = NULL;
            av_packet_free(&pPacket);
            // 睡眠一下，尽量不去消耗 cpu 的资源，也可以退出销毁这个线程
            // break;
        }
    }
    return nullptr;
}

void FFmpeg::play() {
    // 一个线程去读取 Packet
    pthread_t readPacketThreadT;
    pthread_create(&readPacketThreadT,NULL,threadReadPacket,this);
    pthread_detach(readPacketThreadT);// 不会阻塞主线程，当线程终止后会自动销毁线程资源

    if(pAudio)
        pAudio->play();

    if(pVideo)
        pVideo->play();
}

void FFmpeg::callPlayerJniError(ThreadMode threadMode,int code, char *msg) {
    // 释放资源
    release();
    // 回调给 java 层调用
    pJniCall->callPlayerError(threadMode,code, msg);
}

void FFmpeg::release() {
    if (pFormatContext != NULL) {
        avformat_close_input(&pFormatContext);
        avformat_free_context(pFormatContext);
        pFormatContext = NULL;
    }

    avformat_network_deinit();

    if(url){
        free(url);
        url == NULL;
    }

    if (pPlayerStatus != NULL) {
        delete (pPlayerStatus);
        pPlayerStatus = NULL;
    }

    if (pAudio) {
        delete (pAudio);
        pAudio = nullptr;
    }

    if (pVideo) {
        delete (pVideo);
        pVideo = nullptr;
    }
}

void FFmpeg::prepare() {
    prepareOpenSLES(THREAD_MAIN);
}

void *threadPrepare(void *arg) {
    FFmpeg *pFFmpeg = (FFmpeg *) arg;
    pFFmpeg->prepareOpenSLES(THREAD_CHILD);
    return nullptr;
}

void FFmpeg::prepareAsync() {
    pthread_t prepareThreadT;
    pthread_create(&prepareThreadT, nullptr, threadPrepare, this);
    pthread_detach(prepareThreadT);// 不会阻塞主线程，当线程终止后会自动销毁线程资源
}

void FFmpeg::prepareOpenSLES(ThreadMode threadMode) {
    // 1.初始化所有组件，只有调用了该函数，才能使用复用器和编解码器（源码）
    av_register_all();
    // 2.初始化网络
    avformat_network_init();

    int formatOpenInputRes = -1;
    int formatFindStreamInfoRes = -1;

    // 3.打开输入
    formatOpenInputRes = avformat_open_input(&pFormatContext, url, nullptr, nullptr);
    if (formatOpenInputRes != 0) {
        // 第一件事，需要回调给 Java层
        // 第二件 事，需要释放资源
        LOGE("format open input error：%s", av_err2str(formatOpenInputRes));
        callPlayerJniError(threadMode,formatOpenInputRes, av_err2str(formatOpenInputRes));
        return;
    }

    // 4.找出输入流的信息
    formatFindStreamInfoRes = avformat_find_stream_info(pFormatContext, nullptr);
    if (formatFindStreamInfoRes < 0) {
        LOGE("format find stream info error: %s", av_err2str(formatFindStreamInfoRes));
        callPlayerJniError(threadMode,formatFindStreamInfoRes, av_err2str(formatFindStreamInfoRes));
        return;
    }

    // 5.查找音频流的 index
    int audioStreamIndex = av_find_best_stream(pFormatContext, AVMediaType::AVMEDIA_TYPE_AUDIO, -1,
            -1,nullptr, 0);
    if (audioStreamIndex < 0) {
        LOGE("format audio stream error");
        callPlayerJniError(threadMode,FIND_STREAM_ERROR_CODE, "format audio stream error");
        return;
    }
    // 获取采样率和通道
    AVStream *audio_stream = pFormatContext->streams[audioStreamIndex];
    LOGE("采样率：%d, 通道数: %d", audio_stream->codecpar->sample_rate, audio_stream->codecpar->channels);

    // 音频
    pAudio = new Audio(audioStreamIndex,pJniCall,pPlayerStatus);
    pAudio->analysisStream(threadMode,pFormatContext);

    // 6.查找视频流的 index
    int videoStreamIndex = av_find_best_stream(pFormatContext, AVMediaType::AVMEDIA_TYPE_VIDEO, -1,
            -1,NULL, 0);
    // 如果没有视频只有音频就注释掉这个判断
    if (videoStreamIndex < 0) {
        LOGE("find video stream error");
        callPlayerJniError(threadMode,FIND_STREAM_ERROR_CODE, "find video stream error");
        return;
    }

    // 视频
    pVideo = new Video(videoStreamIndex,pJniCall,pPlayerStatus,pAudio);
    pVideo->analysisStream(threadMode,pFormatContext);

    // 回调到 Java 告诉他准备好了
    pJniCall->callPlayerPrepared(threadMode);
}

void FFmpeg::setSurface(JNIEnv *env,jobject surface) {
//    pJniCall->jniEnv->NewGlobalRef(surface);
    if(pVideo)
        pVideo->setSurface(env,surface);
}


// 使用AudioTrack播放
void FFmpeg::prepareAudioTrack(ThreadMode threadMode) {
    // 1.初始化所有组件，只有调用了该函数，才能使用复用器和编解码器（源码）
    av_register_all();
    // 2.初始化网络
    avformat_network_init();

    int formatOpenInputRes = -1;
    int formatFindStreamInfoRes = -1;
    int audioStreamIndex = -1;
    AVStream *audio_stream;
    AVCodecParameters *pCodecParameters;
    AVCodec *pCodec = NULL;
    int codecParametersToContextRes = -1;
    int codecOpenRes = -1;
    int index = 0;
    AVPacket *pPacket = NULL;
    AVFrame *pFrame = NULL;

    // 3.打开输入
    formatOpenInputRes = avformat_open_input(&pFormatContext, url, NULL, NULL);
    if (formatOpenInputRes != 0) {
        // 第一件事，需要回调给 Java层
        // 第二件 事，需要释放资源
        LOGE("format open input error：%s", av_err2str(formatOpenInputRes));
        callPlayerJniError(threadMode,formatOpenInputRes, av_err2str(formatOpenInputRes));
        return;
    }

    // 4.找出输入流的信息
    formatFindStreamInfoRes = avformat_find_stream_info(pFormatContext, NULL);
    if (formatFindStreamInfoRes < 0) {
        LOGE("format find stream info error: %s", av_err2str(formatFindStreamInfoRes));
        callPlayerJniError(threadMode,formatFindStreamInfoRes, av_err2str(formatFindStreamInfoRes));
        return;
    }

    // 5.查找音频流的 index
    audioStreamIndex = av_find_best_stream(pFormatContext, AVMediaType::AVMEDIA_TYPE_AUDIO, -1, -1,
                                           NULL, 0);
    if (audioStreamIndex < 0) {
        LOGE("format audio stream error");
        callPlayerJniError(threadMode,FIND_STREAM_ERROR_CODE, "format audio stream error");
        return;
    }
    // 获取采样率和通道
    audio_stream = pFormatContext->streams[audioStreamIndex];
    LOGE("采样率：%d, 通道数: %d", audio_stream->codecpar->sample_rate, audio_stream->codecpar->channels);

    // 6.查找解码
    pCodecParameters = pFormatContext->streams[audioStreamIndex]->codecpar;
    pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if (pCodec == NULL) {
        LOGE("codec find audio decoder error");
        callPlayerJniError(threadMode,CODEC_FIND_DECODER_ERROR_CODE, "codec find audio decoder error");
        return;
    }

    // 7.创建一个解码器的上下文
    AVCodecContext *pCodecContext = avcodec_alloc_context3(pCodec);
    if (pCodecContext == NULL) {
        LOGE("codec alloc context error");
        callPlayerJniError(threadMode,CODEC_ALLOC_CONTEXT_ERROR_CODE, "codec alloc context error");
        return;
    }
    // 8.根据参数值填充Codec上下文参数
    codecParametersToContextRes = avcodec_parameters_to_context(pCodecContext, pCodecParameters);
    if (codecParametersToContextRes < 0) {
        LOGE("codec parameters to context error: %s", av_err2str(codecParametersToContextRes));
        callPlayerJniError(threadMode,codecParametersToContextRes, av_err2str(codecParametersToContextRes));
        return;
    }
    // 9.打开解码器
    codecOpenRes = avcodec_open2(pCodecContext, pCodec, NULL);
    if (codecOpenRes != 0) {
        LOGE("codec audio open error: %s", av_err2str(codecOpenRes));
        callPlayerJniError(threadMode,codecOpenRes, av_err2str(codecOpenRes));
        return;
    }

    // --------------- 重采样 start --------------
    //输出的声道布局（立体声）
    int64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
    //输出采样格式16bit PCM
    enum AVSampleFormat out_sample_fmt = AVSampleFormat::AV_SAMPLE_FMT_S16;
    //输出采样率
    int out_sample_rate = AUDIO_SAMPLE_RATE;
    //获取输入的声道布局
    //根据声道个数获取默认的声道布局（2个声道，默认立体声stereo）
    int64_t in_ch_layout = pCodecContext->channel_layout;
    //输入的采样格式
    enum AVSampleFormat in_sample_fmt = pCodecContext->sample_fmt;
    //输入采样率
    int in_sample_rate = pCodecContext->sample_rate;
    SwrContext *swrContext = swr_alloc_set_opts(NULL, out_ch_layout, out_sample_fmt,
                                    out_sample_rate, in_ch_layout, in_sample_fmt,
                                    in_sample_rate, 0, NULL);
    if (swrContext == NULL) {
        LOGE("swr alloc set opts error");
        callPlayerJniError(threadMode,SWR_ALLOC_SET_OPTS_ERROR_CODE, "swr alloc set opts error");
        // 提示错误
        return;
    }
    int swrInitRes = swr_init(swrContext);
    if (swrInitRes < 0) {
        LOGE("swr context swr init error");
        callPlayerJniError(threadMode,SWR_CONTEXT_INIT_ERROR_CODE, "swr context swr init error");
        return;
    }
    // size 是播放指定的大小，是最终输出的大小
    int outChannels = av_get_channel_layout_nb_channels(out_ch_layout);
    int dataSize = av_samples_get_buffer_size(NULL, outChannels, pCodecParameters->frame_size,
                                              out_sample_fmt, 0);
    uint8_t *resampleOutBuffer = (uint8_t *) malloc(dataSize);
    // --------------- 重采样 end --------------

    if(threadMode == THREAD_CHILD){
        JNIEnv *env;
        if(pJniCall->javaVM->AttachCurrentThread(&env,nullptr) != JNI_OK){
            LOGE("get child thread jniEnv error");
            return;
        }

        pJniCall->createAudioTrack(env);

        jbyteArray jPcmByteArray =env->NewByteArray(dataSize);
        // native 创建 c 数组
        jbyte *jPcmData = env->GetByteArrayElements(jPcmByteArray, NULL);

        pPacket = av_packet_alloc();
        pFrame = av_frame_alloc();
        // 循环从上下文中读取帧到包中
        while (av_read_frame(pFormatContext, pPacket) >= 0) {
            if (pPacket->stream_index == audioStreamIndex) {
                // Packet 包，压缩的数据，解码成 pcm 数据
                int codecSendPacketRes = avcodec_send_packet(pCodecContext, pPacket);
                if (codecSendPacketRes == 0) {
                    int codecReceiveFrameRes = avcodec_receive_frame(pCodecContext, pFrame);
                    if (codecReceiveFrameRes == 0) {
                        // AVPacket -> AVFrame
                        index++;
                        LOGE("解码第 %d 帧", index);

                        // 调用重采样的方法
                        swr_convert(swrContext, &resampleOutBuffer, pFrame->nb_samples,
                                    (const uint8_t **) pFrame->data, pFrame->nb_samples);

                        // write 写到缓冲区 pFrame.data -> javabyte
                        // size 是多大，装 pcm 的数据
                        // 1s 44100 点，2通道， 2字节 44100*2*2
                        // 1帧不是一秒，pFrame->nb_samples点

                        memcpy(jPcmData, resampleOutBuffer, dataSize);
                        // 1 把 c 的数组的数据同步到 jbyteArray,然后不释放native数组
                        env->ReleaseByteArrayElements(jPcmByteArray, jPcmData, JNI_COMMIT);

                        pJniCall->callAudioTrackWrite(env,jPcmByteArray, 0, dataSize);
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
        // 0 把 c 的数组的数据同步到 jbyteArray,然后释放native数组
        env->ReleaseByteArrayElements(jPcmByteArray, jPcmData, 0);
        // 解除 jPcmDataArray 的持有，让 javaGC 回收
        env->DeleteLocalRef(jPcmByteArray);
        free(resampleOutBuffer);
        pJniCall->javaVM->DetachCurrentThread();
    }else{

        pJniCall->createAudioTrack(NULL);

        jbyteArray jPcmByteArray = pJniCall->jniEnv->NewByteArray(dataSize);
        // native 创建 c 数组
        jbyte *jPcmData = pJniCall->jniEnv->GetByteArrayElements(jPcmByteArray, NULL);

        pPacket = av_packet_alloc();
        pFrame = av_frame_alloc();
        // 循环从上下文中读取帧到包中
        while (av_read_frame(pFormatContext, pPacket) >= 0) {
            if (pPacket->stream_index == audioStreamIndex) {
                // Packet 包，压缩的数据，解码成 pcm 数据
                int codecSendPacketRes = avcodec_send_packet(pCodecContext, pPacket);
                if (codecSendPacketRes == 0) {
                    int codecReceiveFrameRes = avcodec_receive_frame(pCodecContext, pFrame);
                    if (codecReceiveFrameRes == 0) {
                        // AVPacket -> AVFrame
                        index++;
                        LOGE("解码第 %d 帧", index);

                        // 调用重采样的方法
                        swr_convert(swrContext, &resampleOutBuffer, pFrame->nb_samples,
                                    (const uint8_t **) pFrame->data, pFrame->nb_samples);

                        // write 写到缓冲区 pFrame.data -> javabyte
                        // size 是多大，装 pcm 的数据
                        // 1s 44100 点，2通道， 2字节 44100*2*2
                        // 1帧不是一秒，pFrame->nb_samples点

                        memcpy(jPcmData, resampleOutBuffer, dataSize);
                        // 1 把 c 的数组的数据同步到 jbyteArray,然后不释放native数组
                        pJniCall->jniEnv->ReleaseByteArrayElements(jPcmByteArray, jPcmData, JNI_COMMIT);
                        pJniCall->callAudioTrackWrite(NULL,jPcmByteArray, 0, dataSize);
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
        // 0 把 c 的数组的数据同步到 jbyteArray,然后释放native数组
        pJniCall->jniEnv->ReleaseByteArrayElements(jPcmByteArray, jPcmData, 0);
        // 解除 jPcmDataArray 的持有，让 javaGC 回收
        pJniCall->jniEnv->DeleteLocalRef(jPcmByteArray);
        free(resampleOutBuffer);
    }
}

