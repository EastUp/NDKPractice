//
// Created by 123 on 2020/10/12.
//


#include "Video.h"

Video::Video(int audioStreamIndex, JNICall *pJniCall, PlayerStatus *pPlayerStatus, Audio *pAudio)
        :Media(audioStreamIndex,pJniCall,pPlayerStatus) {
    this->pAudio = pAudio;
}

Video::~Video() {
    release();
}

void *threadVideoPlay(void *args){
    Video *pVideo = (Video*)args;

    // 获取当前线程的 JNIEnv， 通过 JavaVM
    JNIEnv *env;
    if (pVideo->pJniCall->javaVM->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        LOGE("get child thread jniEnv error");
        return nullptr;
    }

    //-------------需要将视频的格式一般为yuv420P转为RBGA8888-------------------------//
    // 1. 获取窗体
    ANativeWindow *pNativeWindow = ANativeWindow_fromSurface(env,pVideo->surface);
    // 2.设置缓冲区的数据
    //通过设置宽高限制缓冲区中的像素数量，而非屏幕的物流显示尺寸。
    //如果缓冲区与物理屏幕的显示尺寸不相符，则实际显示可能会是拉伸，或者被压缩的图像
    ANativeWindow_setBuffersGeometry(pNativeWindow,pVideo->pCodecContext->width,pVideo->pCodecContext->height,
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
                sws_scale(pVideo->pSwsContext,pFrame->data,pFrame->linesize,
                          0,pVideo->pCodecContext->height,pVideo->pRgbaFrame->data,
                          pVideo->pRgbaFrame->linesize);


                // 把数据推到缓冲区
                ANativeWindow_lock(pNativeWindow,&outBuffer,NULL);
                memcpy(outBuffer.bits,pVideo->pFrameBuffer,pVideo->frameSize);
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
    pSwsContext = sws_getContext(pCodecContext->width,pCodecContext->height,pCodecContext->pix_fmt,
                                             pCodecContext->width,pCodecContext->height,AV_PIX_FMT_RGBA,
                                             SWS_BILINEAR,NULL,NULL,NULL);
    pRgbaFrame = av_frame_alloc();
    frameSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA,pCodecContext->width,
                                             pCodecContext->height,1);
    pFrameBuffer = (uint8_t*)malloc(frameSize);
    // 填充
    av_image_fill_arrays(pRgbaFrame->data,pRgbaFrame->linesize,pFrameBuffer,AV_PIX_FMT_RGBA,
                         pCodecContext->width,pCodecContext->height,1);
}

void Video::release() {
    Media::release();

    if(pSwsContext){
        sws_freeContext(pSwsContext);
        free(pSwsContext);
        pSwsContext = nullptr;
    }

    if(pFrameBuffer){
        free(pFrameBuffer);
        pFrameBuffer = nullptr;
    }

    if(pRgbaFrame){
        av_frame_free(&pRgbaFrame);
        pRgbaFrame = nullptr;
    }

    // 注意，pJniCall 需要再 FFmpeg 之后销毁
    if(pJniCall)
        pJniCall->jniEnv->DeleteGlobalRef(surface);
}

void Video::setSurface(jobject surface) {
    this->surface = pJniCall->jniEnv->NewGlobalRef(surface);
}

double Video::getFrameSleepTime(AVFrame *pFrame) {
    return 0;
}

