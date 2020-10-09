//
// Created by 123 on 2020/10/9.
//

#include "FFmpeg.h"
#include "ConstDefine.h"

FFmpeg::FFmpeg(JNICall *pJniCall, const char *url) {
    this->pJniCall = pJniCall;
    this->url = url;
}

FFmpeg::~FFmpeg() {
    release();
}

void *threadPlay(void *arg) {
    FFmpeg *pFFmpeg = (FFmpeg *) arg;
    pFFmpeg->prepare();
    return NULL;
}

void FFmpeg::play() {
    // 创建一个线程去播放，多线程边解码边播放
//    pthread_t tid;
//    pthread_create(&tid,NULL,threadPlay,this);
//    pthread_detach(tid); // 回收线程
    prepare();
}

void FFmpeg::callPlayerJniError(int code, char *msg) {
    // 释放资源
    release();
    // 回调给 java 层调用
    pJniCall->callPlayerError(code, msg);
}

void FFmpeg::release() {
    if (pCodecContext != NULL) {
        avcodec_close(pCodecContext);
        avcodec_free_context(&pCodecContext);
        pCodecContext = NULL;
    }

    if (pFormatContext != NULL) {
        avformat_close_input(&pFormatContext);
        avformat_free_context(pFormatContext);
        pFormatContext = NULL;
    }

    if (swrContext != NULL) {
        swr_free(&swrContext);
        free(swrContext);
        swrContext = NULL;
    }

    if (resampleOutBuffer != NULL) {
        free(resampleOutBuffer);
        resampleOutBuffer = NULL;
    }
    avformat_network_deinit();
}

void FFmpeg::prepare() {

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
        callPlayerJniError(formatOpenInputRes, av_err2str(formatOpenInputRes));
        return;
    }

    // 4.找出输入流的信息
    formatFindStreamInfoRes = avformat_find_stream_info(pFormatContext, NULL);
    if (formatFindStreamInfoRes < 0) {
        LOGE("format find stream info error: %s", av_err2str(formatFindStreamInfoRes));
        callPlayerJniError(formatFindStreamInfoRes, av_err2str(formatFindStreamInfoRes));
        return;
    }

    // 5.查找音频流的 index
    audioStreamIndex = av_find_best_stream(pFormatContext, AVMediaType::AVMEDIA_TYPE_AUDIO, -1, -1,
                                           NULL, 0);
    if (audioStreamIndex < 0) {
        LOGE("format audio stream error");
        callPlayerJniError(FIND_STREAM_ERROR_CODE, "format audio stream error");
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
        callPlayerJniError(CODEC_FIND_DECODER_ERROR_CODE, "codec find audio decoder error");
        return;
    }

    // 7.创建一个解码器的上下文
    pCodecContext = avcodec_alloc_context3(pCodec);
    if (pCodecContext == NULL) {
        LOGE("codec alloc context error");
        callPlayerJniError(CODEC_ALLOC_CONTEXT_ERROR_CODE, "codec alloc context error");
        return;
    }
    // 8.根据参数值填充Codec上下文参数
    codecParametersToContextRes = avcodec_parameters_to_context(pCodecContext, pCodecParameters);
    if (codecParametersToContextRes < 0) {
        LOGE("codec parameters to context error: %s", av_err2str(codecParametersToContextRes));
        callPlayerJniError(codecParametersToContextRes, av_err2str(codecParametersToContextRes));
        return;
    }
    // 9.打开解码器
    codecOpenRes = avcodec_open2(pCodecContext, pCodec, NULL);
    if (codecOpenRes != 0) {
        LOGE("codec audio open error: %s", av_err2str(codecOpenRes));
        callPlayerJniError(codecOpenRes, av_err2str(codecOpenRes));
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
    swrContext = swr_alloc_set_opts(NULL, out_ch_layout, out_sample_fmt,
                                                out_sample_rate, in_ch_layout, in_sample_fmt,
                                                in_sample_rate, 0, NULL);
    if (swrContext == NULL) {
        LOGE("swr alloc set opts error");
        callPlayerJniError(SWR_ALLOC_SET_OPTS_ERROR_CODE, "swr alloc set opts error");
        // 提示错误
        return;
    }
    int swrInitRes = swr_init(swrContext);
    if (swrInitRes < 0) {
        LOGE("swr context swr init error");
        callPlayerJniError(SWR_CONTEXT_INIT_ERROR_CODE, "swr context swr init error");
        return;
    }
    // size 是播放指定的大小，是最终输出的大小
    int outChannels = av_get_channel_layout_nb_channels(out_ch_layout);
    int dataSize = av_samples_get_buffer_size(NULL, outChannels, pCodecParameters->frame_size,
                                              out_sample_fmt, 0);
    resampleOutBuffer = (uint8_t *) malloc(dataSize);
    // --------------- 重采样 end --------------

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
                    pJniCall->callAudioTrackWrite(jPcmByteArray, 0, dataSize);
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
}

void FFmpeg::prepareAsync() {

}

