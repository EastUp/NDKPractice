//
// Created by hcDarren on 2019/6/23.
//
#include "DZVideo.h"

DZVideo::DZVideo(int videoStreamIndex, DZPlayStatus *pPlayStatus, JNIPlayerCall *pPlayerCall,
        DZAudio *pAudio) : DZMedia(videoStreamIndex, pPlayStatus, pPlayerCall) {
    this->pAudio = pAudio;
}

DZVideo::~DZVideo() {
    release();
}

void *threadPlay(void *context) {
    DZVideo *pVideo = (DZVideo *) context;
    AVPacket *pPacket = av_packet_alloc();
    AVFrame *pFrame = av_frame_alloc();
    while (pVideo->pPlayStatus != NULL && !pVideo->pPlayStatus->isExit) {
        // 是不是暂停或者 seek 中
        if (pVideo->pPlayStatus != NULL) {
            if (pVideo->pPlayStatus->isPause || pVideo->pPlayStatus->isSeek) {
                av_usleep(10 * 1000);
                continue;
            }
        }
        // 加锁，防止 seek 时获取到脏数据
        pthread_mutex_lock(&pVideo->seekMutex);
        pVideo->pPacketQueue->pop(pPacket);
        // 是否支持硬解码
        if (pVideo->supportStiffCodec) {
            int bsfSendPacketRes = av_bsf_send_packet(pVideo->pBSFContext, pPacket);
            pthread_mutex_unlock(&pVideo->seekMutex);
            if (bsfSendPacketRes == 0) {
                while (av_bsf_receive_packet(pVideo->pBSFContext, pPacket) == 0) {
                    double sleepTime = pVideo->getFrameSleepTime(pPacket->pts);
                    av_usleep(sleepTime * 1000000);
                    pVideo->pPlayerCall->onCallDecodePacket(pPacket->size, pPacket->data);
                    av_packet_unref(pPacket);
                }
            }
        } else {
            // 解码 avcodec_send_packet -> avcodec_receive_frame
            int send_packet_res = avcodec_send_packet(pVideo->pCodecContext, pPacket);
            if (send_packet_res == 0) {
                int receive_frame_res = avcodec_receive_frame(pVideo->pCodecContext, pFrame);
                pthread_mutex_unlock(&pVideo->seekMutex);
                if (receive_frame_res == 0) {
                    double sleepTime = pVideo->getFrameSleepTime(pFrame->pts);
                    av_usleep(sleepTime * 1000000);

                    if (pVideo->pCodecContext->pix_fmt == AVPixelFormat::AV_PIX_FMT_YUV420P) {
                        // 不需要转可以直用 OpenGLES 去渲染
                        pVideo->pPlayerCall->onCallRenderYUV420P(pVideo->pCodecContext->width,
                                pVideo->pCodecContext->height,
                                pFrame->data[0],
                                pFrame->data[1],
                                pFrame->data[2]);
                    } else {
                        // 需要转换
                        sws_scale(pVideo->pSwsContext, pFrame->data, pFrame->linesize, 0,
                                pFrame->height,
                                pVideo->pFrameYUV420P->data,
                                pVideo->pFrameYUV420P->linesize);
                        // OpenGLES 去渲染
                        pVideo->pPlayerCall->onCallRenderYUV420P(pVideo->pCodecContext->width,
                                pVideo->pCodecContext->height,
                                pVideo->pFrameYUV420P->data[0],
                                pVideo->pFrameYUV420P->data[1],
                                pVideo->pFrameYUV420P->data[2]);
                    }
                }
            } else {
                pthread_mutex_unlock(&pVideo->seekMutex);
            }
            av_frame_unref(pFrame);
        }

        // 释放 data 数据，释放 AVPacket 开辟的内存
        av_packet_unref(pPacket);
    }
    av_packet_free(&pPacket);
    av_frame_free(&pFrame);
    return 0;
}

void DZVideo::play() {
    pthread_t playThreadT;
    pthread_create(&playThreadT, NULL, threadPlay, this);
}

void DZVideo::release() {
    DZMedia::release();

    if (pFrameYUV420P != NULL) {
        av_frame_free(&pFrameYUV420P);
        pFrameYUV420P = NULL;
    }

    if (pFrameBuffer != NULL) {
        free(pFrameBuffer);
        pFrameBuffer = NULL;
    }

    if (pSwsContext != NULL) {
        sws_freeContext(pSwsContext);
        av_free(pSwsContext);
        pSwsContext = NULL;
    }

    if (pBSFContext != NULL) {
        av_bsf_free(&pBSFContext);
        av_free(pBSFContext);
        pBSFContext = NULL;
    }
}

void DZVideo::analysisStream(ThreadType threadType, AVFormatContext *pFormatContext) {
    DZMedia::analysisStream(threadType, pFormatContext);

    pFrameYUV420P = av_frame_alloc();
    int frameBufferSize = av_image_get_buffer_size(AVPixelFormat::AV_PIX_FMT_YUV420P,
            pCodecContext->width,
            pCodecContext->height, 1);
    pFrameBuffer = (uint8_t *)
            malloc(frameBufferSize);
    av_image_fill_arrays(pFrameYUV420P->data, pFrameYUV420P->linesize, pFrameBuffer,
            AV_PIX_FMT_YUV420P, pCodecContext->width, pCodecContext->height, 1);

    pSwsContext = sws_getContext(pCodecContext->width, pCodecContext->height,
            pCodecContext->pix_fmt, pCodecContext->width,
            pCodecContext->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL,
            NULL, NULL);
    if (pSwsContext == NULL) {
        callPlayerJniError(threadType, SWS_GET_CONTEXT_ERROR_CODE, "sws get context error.");
    }

    int num = pFormatContext->streams[streamIndex]->avg_frame_rate.num;
    int den = pFormatContext->streams[streamIndex]->avg_frame_rate.den;
    if (num != 0 && den != 0) {
        defaultDelayTime = 1.0 * den / num;
    }

    const char *codecName = pCodecContext->codec->name;
    supportStiffCodec = pPlayerCall->onCallIsSupportStiffCodec(threadType, codecName);
    if (supportStiffCodec) {
        // 如果支持硬解码
        const AVBitStreamFilter *pBSFilter = NULL;
        if (strcasecmp("h264", codecName) == 0) {
            pBSFilter = av_bsf_get_by_name("h264_mp4toannexb");
        } else if (strcasecmp("h265", codecName) == 0) {
            pBSFilter = av_bsf_get_by_name("hevc_mp4toannexb");
        }

        if (pBSFilter == NULL) {
            supportStiffCodec = false;
            return;
        }

        int bsfAllocRes = av_bsf_alloc(pBSFilter, &pBSFContext);
        if (bsfAllocRes != 0) {
            supportStiffCodec = false;
            return;
        }

        AVCodecParameters *pCodecParameters = pFormatContext->streams[streamIndex]->codecpar;
        int codecParametersCopyRes = avcodec_parameters_copy(pBSFContext->par_in, pCodecParameters);
        if (codecParametersCopyRes < 0) {
            supportStiffCodec = false;
            return;
        }

        int bsfInitRes = av_bsf_init(pBSFContext);
        if (bsfInitRes != 0) {
            supportStiffCodec = false;
            return;
        }

        // 调用 java 层初始化 MediaCodec
        pPlayerCall->onCallInitMediaCodec(threadType, codecName, pCodecContext->width,
                pCodecContext->height, pCodecContext->extradata_size, pCodecContext->extradata_size,
                pCodecContext->extradata, pCodecContext->extradata);

        pBSFContext->time_base_in = timeBase;
    }
}

/**
 * 获取当前视频帧应该休眠的时间
 * @param pFrame 当前视频帧
 * @return 睡眠时间
 */
double DZVideo::getFrameSleepTime(int64_t pts) {
    // 如果 < 0 那么还是用之前的时间
    double times = pts * av_q2d(timeBase);
    if (times > currentTime) {
        currentTime = times;
    }
    // 音频和视频之间的差值
    double diffTime = pAudio->currentTime - currentTime;

    // 第一次控制，0.016s ～ -0.016s
    if (diffTime > 0.016 || diffTime < -0.016) {
        if (diffTime > 0.016) {
            delayTime = delayTime * 2 / 3;
        } else if (diffTime < -0.016) {
            delayTime = delayTime * 3 / 2;
        }

        // 第二次控制，defaultDelayTime * 2 / 3 ～ defaultDelayTime * 3 / 2
        if (delayTime < defaultDelayTime / 2) {
            delayTime = defaultDelayTime * 2 / 3;
        } else if (delayTime > defaultDelayTime * 2) {
            delayTime = defaultDelayTime * 3 / 2;
        }
    }

    // 第三次控制，0～defaultDelayTime * 2
    if (diffTime >= 0.25) {
        delayTime = 0;
    } else if (diffTime <= -0.25) {
        delayTime = defaultDelayTime * 2;
    }

    // 假设1秒钟25帧，不出意外情况，delayTime 是 0.2 , 0.4 , 0.6
    return delayTime;
}
