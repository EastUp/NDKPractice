#include <jni.h>
#include "JNICall.h"
#include "FFmpeg.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>

// 在 c++ 中采用 c 的这种编译方式
extern "C" {
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}

JNICall *pJniCall;
FFmpeg *pFFmpeg;

JavaVM *pJavaVM = NULL;

// 重写 so 被加载时会调用的一个方法
// 小作业，了解下动态注册
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *javaVM,void *reserved){
    pJavaVM = javaVM;
    JNIEnv *env;
    if(javaVM->GetEnv((void **)&env,JNI_VERSION_1_4) != JNI_OK){
        return -1;
    }
    return JNI_VERSION_1_4;
}


extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpeg86_media_JaminPlayer_nPlay(JNIEnv *env, jobject instance,
                                                       jstring url_) {
    if(pFFmpeg)
        pFFmpeg->play();
}

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpeg86_media_JaminPlayer_nPrepare(JNIEnv *env, jobject instance,
                                               jstring url_) {
    const char *url = env->GetStringUTFChars(url_,0);
    if(pFFmpeg == NULL){
        pJniCall = new JNICall(pJavaVM,env,instance);
        pFFmpeg = new FFmpeg(pJniCall,url);
        pFFmpeg->prepare();
        //    pFFmpeg->prepare();
        //    delete pJniCall;
        //    delete pFFmpeg;
    }
    env->ReleaseStringUTFChars(url_, url);
}

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpeg86_media_JaminPlayer_nPrepareAsync(JNIEnv *env, jobject instance,
                                                  jstring url_) {
    const char *url = env->GetStringUTFChars(url_,0);
    if(pFFmpeg == NULL){
        pJniCall = new JNICall(pJavaVM,env,instance);
        pFFmpeg = new FFmpeg(pJniCall,url);
        pFFmpeg->prepareAsync();
        //    pFFmpeg->prepare();
        //    delete pJniCall;
        //    delete pFFmpeg;
    }
    env->ReleaseStringUTFChars(url_, url);
}

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpeg86_video_player_MainActivity_decodeVideo(JNIEnv *env, jobject instance,
                                                       jstring url_,jobject surface) {
    const char *url = env->GetStringUTFChars(url_,0);

    // 解码视频，解码音频类似，解码的流程类似，把之前的代码拷过来
    av_register_all();
    avformat_network_init();
    AVFormatContext *pFormatContext = NULL;
    int formatOpenInputRes = 0;
    int formatFindStreamInfoRes = 0;
    int audioStramIndex = -1;
    AVCodecParameters *pCodecParameters;
    AVCodec *pCodec = NULL;
    AVCodecContext *pCodecContext = NULL;
    int codecParametersToContextRes = -1;
    int codecOpenRes = -1;
    int index = 0;
    AVPacket *pPacket = NULL;
    AVFrame *pFrame = NULL;
    formatOpenInputRes = avformat_open_input(&pFormatContext, url, NULL, NULL);


    formatFindStreamInfoRes = avformat_find_stream_info(pFormatContext, NULL);


    // 查找视频流的 index
    audioStramIndex = av_find_best_stream(pFormatContext, AVMediaType::AVMEDIA_TYPE_VIDEO, -1, -1,
                                          NULL, 0);


    // 查找解码
    pCodecParameters = pFormatContext->streams[audioStramIndex]->codecpar;
    pCodec = avcodec_find_decoder(pCodecParameters->codec_id);

    // 打开解码器
    pCodecContext = avcodec_alloc_context3(pCodec);

    codecParametersToContextRes = avcodec_parameters_to_context(pCodecContext, pCodecParameters);


    codecOpenRes = avcodec_open2(pCodecContext, pCodec, NULL);


    //-------------需要将视频的格式一般为yuv420P转为RBGA8888-------------------------//
    // 1. 获取窗体
    ANativeWindow *pNativeWindow = ANativeWindow_fromSurface(env,surface);
    // 2.设置缓冲区的数据
    //通过设置宽高限制缓冲区中的像素数量，而非屏幕的物流显示尺寸。
    //如果缓冲区与物理屏幕的显示尺寸不相符，则实际显示可能会是拉伸，或者被压缩的图像
    ANativeWindow_setBuffersGeometry(pNativeWindow,pCodecContext->width,pCodecContext->height,
            WINDOW_FORMAT_RGBA_8888);
    // Window 绘图缓冲区
    ANativeWindow_Buffer outBuffer;
    // 3.初始化转换上下文
    SwsContext *pSwsContext = sws_getContext(pCodecContext->width,pCodecContext->height,pCodecContext->pix_fmt,
                   pCodecContext->width,pCodecContext->height,AV_PIX_FMT_RGBA,
                   SWS_BILINEAR,NULL,NULL,NULL);
    AVFrame *pRgbaFrame = av_frame_alloc();
    int frameSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA,pCodecContext->width,
            pCodecContext->height,1);
    uint8_t *frameBuffer = (uint8_t*)malloc(frameSize);
    // 填充
    av_image_fill_arrays(pRgbaFrame->data,pRgbaFrame->linesize,frameBuffer,AV_PIX_FMT_RGBA,
                         pCodecContext->width,pCodecContext->height,1);

    pPacket = av_packet_alloc();
    pFrame = av_frame_alloc();
    while (av_read_frame(pFormatContext, pPacket) >= 0) {
        if (pPacket->stream_index == audioStramIndex) {
            // Packet 包，压缩的数据，解码成 pcm 数据
            int codecSendPacketRes = avcodec_send_packet(pCodecContext, pPacket);
            if (codecSendPacketRes == 0) {
                int codecReceiveFrameRes = avcodec_receive_frame(pCodecContext, pFrame);
                if (codecReceiveFrameRes == 0) {
                    // AVPacket -> AVFrame
                    index++;
                    LOGE("解码第 %d 帧", index);
                    // 渲染，显示，OpenGLES (高效，硬件支持)，SurfaceView
                    // 硬件加速和不加速有什么区别？cup 主要是用于计算，gpu 图像支持（硬件）
                    // 这个 pFrame->data , 一般 yuv420P 的，RGBA8888，因此需要转换
                    // 假设拿到了转换后的 RGBA 的 data 数据，如何渲染，把数据推到缓冲区
                    sws_scale(pSwsContext,pFrame->data,pFrame->linesize,
                            0,pCodecContext->height,pRgbaFrame->data,pRgbaFrame->linesize);
                    // 把数据推到缓冲区
                    ANativeWindow_lock(pNativeWindow,&outBuffer,NULL);
                    memcpy(outBuffer.bits,frameBuffer,frameSize);
                    ANativeWindow_unlockAndPost(pNativeWindow);
                }
            }
        }
        // 解引用
        av_packet_unref(pPacket);
        av_frame_unref(pFrame);
    }

    // 1. 解引用数据 data ， 2. 销毁 pPacket 结构体内存  3. pPacket = NULL
    av_packet_free(&pPacket);
    av_frame_free(&pFrame);

    __av_resources_destroy:
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
    avformat_network_deinit();

    env->ReleaseStringUTFChars(url_, url);
}