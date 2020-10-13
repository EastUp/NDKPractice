//
// Created by 123 on 2020/10/12.
//


#include "Video.h"

Video::Video(int audioStreamIndex, JNICall *pJniCall, PlayerStatus *pPlayerStatus, Audio *pAudio)
        : Media(audioStreamIndex, pJniCall, pPlayerStatus) {
    this->pAudio = pAudio;
}

Video::~Video() {
    release();
}

void *threadVideoPlay(void *args) {
    Video *pVideo = (Video *) args;

    // 获取当前线程的 JNIEnv， 通过 JavaVM
    JNIEnv *env;
    if (pVideo->pJniCall->javaVM->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        LOGE("get child thread jniEnv error");
        return nullptr;
    }

    //-------------需要将视频的格式一般为yuv420P转为RBGA8888-------------------------//
    // 1. 获取窗体
    ANativeWindow *pNativeWindow = ANativeWindow_fromSurface(env, pVideo->surface);
    // 2.设置缓冲区的数据
    //通过设置宽高限制缓冲区中的像素数量，而非屏幕的物流显示尺寸。
    //如果缓冲区与物理屏幕的显示尺寸不相符，则实际显示可能会是拉伸，或者被压缩的图像
    ANativeWindow_setBuffersGeometry(pNativeWindow, pVideo->pCodecContext->width,
                                     pVideo->pCodecContext->height,
                                     WINDOW_FORMAT_RGBA_8888);
    // Window 绘图缓冲区
    ANativeWindow_Buffer outBuffer;

    AVPacket *pPacket = nullptr;
    AVFrame *pFrame = av_frame_alloc();

    while (pVideo->pPlayerStatus != nullptr && !pVideo->pPlayerStatus->isExit) {
        pPacket = pVideo->pPacketQueue->pop();
        // Packet 包，压缩的数据，解码成 pcm 数据
        int codecSendPacketRes = avcodec_send_packet(pVideo->pCodecContext, pPacket);
        if (codecSendPacketRes == 0) {
            int codecReceiveFrameRes = avcodec_receive_frame(pVideo->pCodecContext, pFrame);
            if (codecReceiveFrameRes == 0) {
                // 渲染，显示，OpenGLES (高效，硬件支持)，SurfaceView
                // 硬件加速和不加速有什么区别？cup 主要是用于计算，gpu 图像支持（硬件）
                // 这个 pFrame->data , 一般 yuv420P 的，RGBA8888，因此需要转换
                // 假设拿到了转换后的 RGBA 的 data 数据，如何渲染，把数据推到缓冲区
                sws_scale(pVideo->pSwsContext, pFrame->data, pFrame->linesize,
                          0, pVideo->pCodecContext->height, pVideo->pRgbaFrame->data,
                          pVideo->pRgbaFrame->linesize);

                // 播放之前判断一下要休眠多久
                double frameSleepTime = pVideo->getFrameSleepTime(pPacket,pFrame); // 这是秒
                av_usleep(frameSleepTime * 1000000);// 这是毫秒

                // 把数据推到缓冲区
                ANativeWindow_lock(pNativeWindow, &outBuffer, NULL);
                memcpy(outBuffer.bits, pVideo->pFrameBuffer, pVideo->frameSize);
                ANativeWindow_unlockAndPost(pNativeWindow);
            }
        }
        // 解引用
        av_packet_unref(pPacket);
        av_frame_unref(pFrame);
    }

    // 1.解引用数据 data, 2.销魂 pPacket 结构体内存， 3.pPacket = NULL;
    av_packet_free(&pPacket);
    av_frame_free(&pFrame);

    return nullptr;
}

void Video::play() {
    // 一个线程去解码播放
    pthread_t playThreadT;
    pthread_create(&playThreadT, NULL, threadVideoPlay, this);
    pthread_detach(playThreadT); // 不会阻塞主线程，当线程终止后会自动销毁线程资源
}

void Video::privateAnalysisStream(ThreadMode threadMode, AVFormatContext *pFormatContext) {
    // 初始化转换上下文
    pSwsContext = sws_getContext(pCodecContext->width, pCodecContext->height,
                                 pCodecContext->pix_fmt,
                                 pCodecContext->width, pCodecContext->height, AV_PIX_FMT_RGBA,
                                 SWS_BILINEAR, NULL, NULL, NULL);
    pRgbaFrame = av_frame_alloc();
    frameSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecContext->width,
                                         pCodecContext->height, 1);
    pFrameBuffer = (uint8_t *) malloc(frameSize);
    // 填充
    av_image_fill_arrays(pRgbaFrame->data, pRgbaFrame->linesize, pFrameBuffer, AV_PIX_FMT_RGBA,
                         pCodecContext->width, pCodecContext->height, 1);

    int num = pFormatContext->streams[streamIndex]->avg_frame_rate.num; // 帧率的分子 24
    int den = pFormatContext->streams[streamIndex]->avg_frame_rate.den; // 帧率的分母 1
    if (num != 0 && den != 0) {
        if (num > den)
            defaultDelayTime = 1.0 * den / num;
    }
    LOGE("num=%ld,den=%d,defaultDelayTime=%lf", num, den, defaultDelayTime);
}

void Video::release() {
    Media::release();

    if (pSwsContext) {
        sws_freeContext(pSwsContext);
        free(pSwsContext);
        pSwsContext = nullptr;
    }

    if (pFrameBuffer) {
        free(pFrameBuffer);
        pFrameBuffer = nullptr;
    }

    if (pRgbaFrame) {
        av_frame_free(&pRgbaFrame);
        pRgbaFrame = nullptr;
    }

    videoJNIEnv->DeleteGlobalRef(surface);

    // 注意，pJniCall 需要再 FFmpeg 之后销毁
//    if(pJniCall)
//        pJniCall->jniEnv->DeleteGlobalRef(surface);
}

void Video::setSurface(JNIEnv *env, jobject surface) {
    // 这里不能使用pJniCall->jniEnv->的env，因为和native方法传递过来的不一样，而且不相等
    /**
     *  注释的这行代码会报错
     *   'JNI DETECTED ERROR IN APPLICATION: invalid address...,thread Thread[21,tid=25717,Native,Thread*=0xc2e4a000,peer=0x12e40000,"Thread-3"] using JNIEnv* from thread Thread[1,tid=25650,Runnable,Thread*=0xf1b71e00,peer=0x757719a0,"main"]
     */
//    this->surface = pJniCall->jniEnv->NewGlobalRef(surface);
    this->videoJNIEnv = env;
    this->surface = env->NewGlobalRef(surface);
}

double Video::getFrameSleepTime(AVPacket *pPacket,AVFrame *pFrame) {
    // 这两种方法都能算出当前显示帧的时间
//    double times = av_frame_get_best_effort_timestamp(pFrame) * av_q2d(timeBase);
    double times = pPacket->pts * av_q2d(timeBase);
    if (times > currentTime) {
        currentTime = times;
    }
    // 相差多少秒
    double diffTime = pAudio->currentTime - currentTime; // 负数，音频比视频慢

    // 视频快了就慢点，视频慢了就快点
    // 但是尽量把事件控制在视频的帧率时间范围左右  1/24 0.04 1/30 0.033

    // 第一次控制 0.016s 到 -0.016s
    if (diffTime > 0.016 || diffTime < -0.016) {
        if (diffTime > 0.016) {
            delayTime = delayTime * 2 / 3;
        } else if (diffTime < -0.016) {
            delayTime = delayTime * 3 / 2;
        }
        // 第二次控制 defaultDelayTIme * 2 / 3 到 defaultDelayTime * 3 / 2
        if (delayTime < defaultDelayTime / 2) {
            delayTime = defaultDelayTime * 2 / 3;
        } else if (delayTime > defaultDelayTime * 2) {
            delayTime = defaultDelayTime * 3 / 2;
        }
    }

    // 第三次控制，这基本是异常情况
    if (diffTime >= 0.25)
        delayTime = 0;
    else if (diffTime <= -0.25)
        delayTime = defaultDelayTime * 2;

    LOGE("diffTime = %lf秒,delayTime = %lf秒,\n"
         "pAudio->currentTime = %lf秒,cureentTime = %lf秒,duration = %d秒",diffTime,delayTime,pAudio->currentTime,currentTime,duration/1000/1000);

    return delayTime;
//    return -diffTime;
}

