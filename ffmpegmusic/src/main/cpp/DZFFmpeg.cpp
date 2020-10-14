//
// Created by east on 2020-10-14.
//
#include "DZFFmpeg.h"

DZFFmpeg::DZFFmpeg(JNIPlayerCall *jni_player_call, const char *url) {
    this->jni_player_call = jni_player_call;
    // 复制 url
    this->url = (char *) (malloc(strlen(url) + 1));
    memcpy(this->url, url, strlen(url) + 1);
    pthread_mutex_init(&releaseMutex, NULL);
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

void DZFFmpeg::preparedAudio(THREAD_TYPE thread_type) {
    pthread_mutex_lock(&releaseMutex);

    // av_register_all: 作用是初始化所有组件，只有调用了该函数，才能使用复用器和编解码器（源码）
    av_register_all();
    avformat_network_init();
    int format_open_res = 0;
    int format_find_stream_info_res = 0;
    AVCodecContext *pCodecContext = NULL;
    int codec_context_parameters_res = 0;
    int codec_open = 0;
    AVCodecParameters *pCodecParameters = NULL;
    int audio_stream_index = 0;
    AVCodec *pCodec = 0;
    int swrInitRes = 0;

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

    pCodecParameters = av_format_context->streams[audio_stream_index]->codecpar;
    pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if (!pCodec) {
        LOGE("Can't find audio decoder : %s", url);
        releasePreparedRes(thread_type, FIND_AUDIO_DECODER_ERROR_CODE, "Can't find audio decoder.");
        return;
    }

    pCodecContext = avcodec_alloc_context3(pCodec);
    codec_context_parameters_res = avcodec_parameters_to_context(pCodecContext, pCodecParameters);
    if (codec_context_parameters_res < 0) {
        LOGE("codec parameters to_context error : %s, %s", url,
                av_err2str(codec_context_parameters_res));
        releasePreparedRes(thread_type, codec_context_parameters_res,
                av_err2str(codec_context_parameters_res));
        return;
    }

    codec_open = avcodec_open2(pCodecContext, pCodec, NULL);
    if (codec_open < 0) {
        LOGE("codec open error : %s, %s", url, av_err2str(codec_open));
        releasePreparedRes(thread_type, codec_open, av_err2str(codec_open));
        return;
    }

    if (audio != NULL) {
        delete (audio);
    }

    audio = new DZAudio(audio_stream_index, pCodecContext->sample_rate, jni_player_call);
    audio->packet_queue = new AVPacketQueue(audio->player_status);
    audio->pCodecContext = pCodecContext;
    audio->duration = av_format_context->duration / AV_TIME_BASE;
    audio->timeBase = av_format_context->streams[audio_stream_index]->time_base;
    audio->av_format_context = av_format_context;

    // 处理一些异常的问题
    if (audio->pCodecContext->channels > 0 && audio->pCodecContext->channel_layout == 0) {
        audio->pCodecContext->channel_layout = av_get_default_channel_layout(
                audio->pCodecContext->channels);
    } else if (audio->pCodecContext->channels == 0 && audio->pCodecContext->channel_layout > 0) {
        audio->pCodecContext->channels = av_get_channel_layout_nb_channels(
                audio->pCodecContext->channel_layout);
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
    uint64_t in_ch_layout = audio->pCodecContext->channel_layout;
    enum AVSampleFormat in_sample_fmt = audio->pCodecContext->sample_fmt;
    int in_sample_rate = audio->pCodecContext->sample_rate;
    audio->swrContext = swr_alloc_set_opts(NULL, out_ch_layout, out_sample_fmt,
            out_sample_rate, in_ch_layout, in_sample_fmt,
            in_sample_rate, 0, 0);
    swrInitRes = swr_init(audio->swrContext);
    if (audio->swrContext == NULL || swrInitRes < 0) {
        LOGE("init SwrContext error : %s", av_err2str(swrInitRes));
        releasePreparedRes(thread_type, swrInitRes, av_err2str(swrInitRes));
        return;
    }

    jni_player_call->onCallPrepared(thread_type);

    pthread_mutex_unlock(&releaseMutex);
}

DZFFmpeg::~DZFFmpeg() {

}

void DZFFmpeg::start() {
    if (audio == NULL) {
        LOGE("DZAudio is null , prepared may be misleading");
        return;
    }

    audio->play();

    audio->decodeFrame();
}

void DZFFmpeg::onPause() {
    if (audio != NULL) {
        audio->pause();
    }
}

void DZFFmpeg::onResume() {
    if (audio != NULL) {
        audio->resume();
    }
}

void DZFFmpeg::release() {

    if (audio == NULL) {
        return;
    }

    pthread_mutex_lock(&releaseMutex);

    if (audio->player_status->isExit) {
        return;
    }

    audio->player_status->isExit = true;

    if (audio != NULL) {
        audio->release();
        delete audio;
    }

    if (av_format_context != NULL) {
        avformat_close_input(&av_format_context);
        avformat_free_context(av_format_context);
        av_format_context = NULL;
    }

    jni_player_call = NULL;
    free(url);

    pthread_mutex_unlock(&releaseMutex);

    pthread_mutex_destroy(&releaseMutex);
}

void DZFFmpeg::releasePreparedRes(THREAD_TYPE threadType, int errorCode, const char *errorMsg) {
    pthread_mutex_unlock(&releaseMutex);
    if (av_format_context != NULL) {
        avformat_close_input(&av_format_context);
        avformat_free_context(av_format_context);
        av_format_context = NULL;
    }

    if (jni_player_call != NULL) {
        jni_player_call->onCallError(threadType, errorCode, errorMsg);
    }

    pthread_mutex_destroy(&releaseMutex);
    free(url);
}

void DZFFmpeg::seek(uint64_t seconds) {
    if (audio != NULL) {
        audio->seek(seconds);
    }
}

