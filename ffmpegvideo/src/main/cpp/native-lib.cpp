#include <jni.h>
#include <malloc.h>
#include <android/log.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
// 缩放
#include "libswscale/swscale.h"
// 重采样
#include "libswresample/swresample.h"
#include "libavutil/imgutils.h"
}

#include <android/native_window_jni.h>
#include <fcntl.h>
#include <unistd.h>

#include "android_log.h"
#include "JNIPlayerCall.h"
#include "DZFFmpeg.h"

#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_SAMPLES_SIZE_PER_CHANNEL AUDIO_SAMPLE_RATE * 4

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpegvideo_media_render_DVideoView_nativeDecodeVideo(JNIEnv *env, jobject instance,
                                                       jstring videoPath_, jobject surface) {
    const char *videoPath = env->GetStringUTFChars(videoPath_, NULL);

    // 1. 注册组件，就是一些初始化工作
    av_register_all();

    // 2. 打开视频文件
    AVFormatContext *avFormatContext = avformat_alloc_context();
    if (avformat_open_input(&avFormatContext, videoPath, NULL, NULL) != 0) {
        LOGE("打开视频文件失败");
        return;
    }

    // 3. 获取视频信息
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGE("获取视频信息失败");
        return;
    }

    // 4. 查找视频流
    AVCodecContext *avCodecContext = NULL;
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            avCodecContext = avFormatContext->streams[i]->codec;
            break;
        }
    }
    if (avCodecContext == NULL) {
        LOGE("查找视频流失败");
        return;
    }

    // 5. 获取解码器
    AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id);
    if (avCodec == NULL) {
        LOGE("找不到对应的解码器");
        return;
    }

    // 6. 打开解码器
    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
        LOGE("解码器打开识别");
        return;
    }

    // 7. 一帧一帧解码播放
    AVPacket avPacket;
    AVFrame *avFrame = av_frame_alloc();
    AVFrame *rgba_frame = av_frame_alloc();

    //native绘制
    //窗体
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    //设置缓冲区的属性（宽、高、像素格式）
    ANativeWindow_setBuffersGeometry(nativeWindow, avCodecContext->width, avCodecContext->height,
                                     WINDOW_FORMAT_RGBA_8888);
    //绘制时的缓冲区
    ANativeWindow_Buffer window_buffer;
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, avCodecContext->width,
                                            avCodecContext->height, 1);
    uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    // 初始化缓冲区
    // 只有指定了AVFrame的像素格式、画面大小才能真正分配内存
    avpicture_fill((AVPicture *) rgba_frame, buffer, AV_PIX_FMT_RGBA, avCodecContext->width,
                   avCodecContext->height);

    int got_picture = 0;
    // 播放的是 argb8888
    struct SwsContext *sws_ctx = sws_getContext(avCodecContext->width, avCodecContext->height,
                                                avCodecContext->pix_fmt, avCodecContext->width,
                                                avCodecContext->height, AV_PIX_FMT_RGBA,
                                                SWS_BILINEAR, NULL, NULL, NULL);

    while (av_read_frame(avFormatContext, &avPacket) >= 0) {
        //解码AVPacket->AVFrame
        avcodec_decode_video2(avCodecContext, avFrame, &got_picture, &avPacket);

        if (got_picture) {
            ANativeWindow_lock(nativeWindow, &window_buffer, NULL);

            sws_scale(sws_ctx, avFrame->data, avFrame->linesize, 0, avFrame->height,
                      rgba_frame->data, rgba_frame->linesize);

            memcpy(window_buffer.bits, buffer, avCodecContext->width * avCodecContext->height * 4);

            //unlock
            ANativeWindow_unlockAndPost(nativeWindow);
        }

        av_packet_unref(&avPacket);
    }

    av_free(buffer);
    av_frame_free(&avFrame);
    av_frame_free(&rgba_frame);
    avcodec_close(avCodecContext);
    avformat_close_input(&avFormatContext);

    env->ReleaseStringUTFChars(videoPath_, videoPath);
}

/**
 * 创建一个 AudioTrack 对象
 */
JNIEXPORT jobject createAudioTrack(JNIEnv *env) {
    jclass audio_track_class = env->FindClass("android/media/AudioTrack");
    jmethodID construct_mid = env->GetMethodID(audio_track_class, "<init>", "(IIIIII)V");
    int channelConfig = (0x4 | 0x8);
    int audioFormat = 2;
    int sampleRateInHz = AUDIO_SAMPLE_RATE;
    // 调用  AudioTrack.getMinBufferSize 方法
    jmethodID getMinBufferSizeMid = env->GetStaticMethodID(audio_track_class, "getMinBufferSize",
                                                           "(III)I");
    int bufferSizeInBytes = env->CallStaticIntMethod(audio_track_class, getMinBufferSizeMid,
                                                     sampleRateInHz, channelConfig, audioFormat);
    jobject audio_track = env->NewObject(audio_track_class, construct_mid, 3, sampleRateInHz,
                                         channelConfig, audioFormat, bufferSizeInBytes, 1);

    //调用AudioTrack.play方法
    jmethodID audio_track_play_mid = env->GetMethodID(audio_track_class, "play", "()V");
    env->CallVoidMethod(audio_track, audio_track_play_mid);
    return audio_track;
}

/**
 * 解码音频
 */
extern "C" void JNICALL
Java_com_east_ffmpegvideo_media_render_DVideoView_nativeDecodeAudio(JNIEnv *env, jobject instance,
                                                       jstring videoPath_) {
    const char *videoPath = env->GetStringUTFChars(videoPath_, 0);

    // 1. 注册组件，就是一些初始化工作
    av_register_all();

    // 2. 打开视频文件
    AVFormatContext *avFormatContext = avformat_alloc_context();
    if (avformat_open_input(&avFormatContext, videoPath, NULL, NULL) != 0) {
        LOGE("打开视频文件失败");
        return;
    }

    // 3. 获取视频流信息
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGE("获取视频流信息失败");
        return;
    }

    // 4. 查找音频流
    int audio_stream_index = -1;
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
            break;
        }
    }
    if (audio_stream_index == -1) {
        LOGE("查找音频流失败");
        return;
    }

    // 5. 获取解码器
    AVCodecContext *avCodecContext = avFormatContext->streams[audio_stream_index]->codec;
    AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id);
    if (avCodec == NULL) {
        LOGE("%s", "无法获取解码器");
        return;
    }

    // 6. 打开解码器
    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
        LOGE("%s", "无法打开解码器");
        return;
    }


    SwrContext *swrContext = swr_alloc();


    //输入的采样格式
    enum AVSampleFormat in_sample_fmt = avCodecContext->sample_fmt;
    //输出采样格式16bit PCM
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    //输入采样率
    int in_sample_rate = avCodecContext->sample_rate;
    //输出采样率
    int out_sample_rate = AUDIO_SAMPLE_RATE;
    //获取输入的声道布局
    //根据声道个数获取默认的声道布局（2个声道，默认立体声stereo）
    uint64_t in_ch_layout = avCodecContext->channel_layout;
    //输出的声道布局（立体声）
    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
    //输出的声道个数
    int out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);
    // LOGE("->%d",avCodecContext->sample_rate);
    swr_alloc_set_opts(swrContext, out_ch_layout, out_sample_fmt, out_sample_rate, in_ch_layout,
                       in_sample_fmt, in_sample_rate, 0, NULL);

    if (swr_init(swrContext) < 0) {
        LOGE("Failed to initialize the resampling context");
        return;
    }

    uint8_t *out_buffer = (uint8_t *) av_malloc(AUDIO_SAMPLES_SIZE_PER_CHANNEL);

    // 7. 一帧一帧解码播放
    // 创建 android.media.AudioTrack 对象
    jobject audio_track = createAudioTrack(env);
    jclass audio_track_class = env->FindClass("android/media/AudioTrack");
    jmethodID audio_track_write_mid = env->GetMethodID(audio_track_class, "write", "([BII)I");

    // 播放
    //out_buffer缓冲区数据，转成byte数组
    //获取sample的size
    int out_buffer_size = av_samples_get_buffer_size(NULL, out_channel_nb,
                                                     avCodecContext->frame_size, out_sample_fmt, 1);
    jbyteArray audio_sample_array = env->NewByteArray(out_buffer_size);
    jbyte *audio_sample_byte = env->GetByteArrayElements(audio_sample_array, NULL);

    AVPacket avPacket;
    AVFrame *avFrame = av_frame_alloc();
    int got_picture = 0;
    while (av_read_frame(avFormatContext, &avPacket) >= 0) {
        //解码音频类型的Packet
        if (avPacket.stream_index == audio_stream_index) {
            //解码AVPacket->AVFrame
            avcodec_decode_audio4(avCodecContext, avFrame, &got_picture, &avPacket);
            if (got_picture) {
                // 重采样
                /*int swr_convert(struct SwrContext *s, uint8_t **out, int out_count,
                                const uint8_t **in , int in_count);*/
                swr_convert(swrContext, &out_buffer, AUDIO_SAMPLES_SIZE_PER_CHANNEL,
                            (const uint8_t **) avFrame->data, avFrame->nb_samples);

                //out_buffer的数据复制到sampe_bytep
                memcpy(audio_sample_byte, out_buffer, out_buffer_size);

                //同步
                env->ReleaseByteArrayElements(audio_sample_array, audio_sample_byte, JNI_COMMIT);

                //AudioTrack.write PCM数据
                env->CallIntMethod(audio_track, audio_track_write_mid, audio_sample_array, 0,
                                   out_buffer_size);
            }
        }

        av_packet_unref(&avPacket);
    }

    // 释放资源
    env->ReleaseByteArrayElements(audio_sample_array, audio_sample_byte, JNI_ABORT);
    env->DeleteLocalRef(audio_sample_array);
    av_frame_free(&avFrame);
    av_free(out_buffer);
    swr_free(&swrContext);
    avcodec_close(avCodecContext);
    avformat_close_input(&avFormatContext);

    env->ReleaseStringUTFChars(videoPath_, videoPath);
}

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpegvideo_ui_MainActivity_operateDir(JNIEnv *env, jobject instance,
                                                  jstring absolutePath_) {
    const char *absolutePath = env->GetStringUTFChars(absolutePath_, 0);

    LOGE("absolutePath = %s", absolutePath);

    // 打印获取里面的文件
    AVIODirContext *dirContext = NULL;
    AVIODirEntry *dirEntry = NULL;
    int open_res = avio_open_dir(&dirContext, absolutePath, NULL);
    if (open_res < 0) {
        LOGE("打开 %s 文件目录失败!", absolutePath);
        return;
    }

    while (1) {
        int read_res = avio_read_dir(dirContext, &dirEntry);
        if (read_res < 0) {
            LOGE("读取过程中出错！");
            goto __free;
            // return;
        }

        // dirEntry == NULL 代表是最后一项
        if (!dirEntry) {
            break;
        }

        LOGE("%s,%s", dirEntry->name, dirEntry->name);

        avio_free_directory_entry(&dirEntry);
    }
    // 一般不建议 goto 语句，但是实际开发过程中非常方便
    __free:
    avio_close_dir(&dirContext);
    env->ReleaseStringUTFChars(absolutePath_, absolutePath);
}

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpegvideo_ui_NativeMedia_printAudioInfo(JNIEnv *env, jclass j_cls,
                                                     jstring absolutePath_) {
    const char *absolutePath = env->GetStringUTFChars(absolutePath_, 0);

    av_register_all();

    AVFormatContext *avFormatContext = NULL;
    int audio_stream_idx;
    AVStream *audio_stream;

    int open_res = avformat_open_input(&avFormatContext, absolutePath, NULL, NULL);
    if (open_res != 0) {
        LOGE("Can't open file: %s", av_err2str(open_res));
        return;
    }
    int find_stream_info_res = avformat_find_stream_info(avFormatContext, NULL);
    if (find_stream_info_res < 0) {
        LOGE("Find stream info error: %s", av_err2str(find_stream_info_res));
        goto __avformat_close;
    }

    // 获取采样率和通道
    audio_stream_idx = av_find_best_stream(avFormatContext, AVMediaType::AVMEDIA_TYPE_AUDIO, -1, -1,
                                           NULL, 0);
    if (audio_stream_idx < 0) {
        LOGE("Find audio stream info error: %s", av_err2str(find_stream_info_res));
        goto __avformat_close;
    }
    audio_stream = avFormatContext->streams[audio_stream_idx];
    LOGE("采样率：%d, 通道数: %d", audio_stream->codecpar->sample_rate, audio_stream->codecpar->channels);

    __avformat_close:
    avformat_close_input(&avFormatContext);
    env->ReleaseStringUTFChars(absolutePath_, absolutePath);
}

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpegvideo_ui_NativeMedia_extractAudio(JNIEnv *env, jclass type, jstring filePath_) {
    const char *filePath = env->GetStringUTFChars(filePath_, 0);

    av_register_all();

    AVFormatContext *av_format_context = NULL;
    int audio_stream_idx;
    int index = 0;
    AVPacket *avPacket = NULL;

    int open_res = avformat_open_input(&av_format_context, filePath, NULL, NULL);
    if (open_res != 0) {
        LOGE("Can't open file: %s", av_err2str(open_res));
        return;
    }
    int find_stream_info_res = avformat_find_stream_info(av_format_context, NULL);
    if (find_stream_info_res < 0) {
        LOGE("Find stream info error: %s", av_err2str(find_stream_info_res));
        goto __avformat_close;
    }

    // 查找音频流
    audio_stream_idx = av_find_best_stream(av_format_context, AVMediaType::AVMEDIA_TYPE_AUDIO, -1,
                                           -1, NULL, 0);
    if (audio_stream_idx < 0) {
        LOGE("Find audio stream info error: %s", av_err2str(find_stream_info_res));
        goto __avformat_close;
    }

    // 提取每一帧的音频流
    avPacket = av_packet_alloc();
    while (av_read_frame(av_format_context, avPacket) >= 0) {
        if (audio_stream_idx == avPacket->stream_index) {
            // 可以写入文件、截取、重采样、解码等等 avPacket.data
            LOGE("抽取第 %d 帧", index);
            index++;
        }
        // av packet unref
        av_packet_unref(avPacket);
    }

    av_packet_free(&avPacket);

    __avformat_close:
    avformat_close_input(&av_format_context);

    env->ReleaseStringUTFChars(filePath_, filePath);
}

jobject initCreateAudioTrack(JNIEnv *env) {
    jclass jAudioTrackClass = env->FindClass("android/media/AudioTrack");
    jmethodID jAudioTrackCMid = env->GetMethodID(jAudioTrackClass, "<init>", "(IIIIII)V");

    //  public static final int STREAM_MUSIC = 3;
    int streamType = 3;
    int sampleRateInHz = 44100;
    // public static final int CHANNEL_OUT_STEREO = (CHANNEL_OUT_FRONT_LEFT | CHANNEL_OUT_FRONT_RIGHT);
    int channelConfig = (0x4 | 0x8);
    // public static final int ENCODING_PCM_16BIT = 2;
    int audioFormat = 2;
    // getMinBufferSize(int sampleRateInHz, int channelConfig, int audioFormat)
    jmethodID jGetMinBufferSizeMid = env->GetStaticMethodID(jAudioTrackClass, "getMinBufferSize",
                                                            "(III)I");
    int bufferSizeInBytes = env->CallStaticIntMethod(jAudioTrackClass, jGetMinBufferSizeMid,
                                                     sampleRateInHz, channelConfig, audioFormat);
    // public static final int MODE_STREAM = 1;
    int mode = 1;
    jobject jAudioTrack = env->NewObject(jAudioTrackClass, jAudioTrackCMid, streamType,
                                         sampleRateInHz, channelConfig, audioFormat,
                                         bufferSizeInBytes, mode);

    // play()
    jmethodID jPlayMid = env->GetMethodID(jAudioTrackClass, "play", "()V");
    env->CallVoidMethod(jAudioTrack, jPlayMid);

    return jAudioTrack;
}

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpegvideo_ui_MainActivity_extractVideo(JNIEnv *env, jobject obj, jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);

    av_register_all();
    avformat_network_init();

    AVFormatContext *av_format_context = NULL;
    int audio_stream_idx;
    int index = 0;
    AVPacket *avPacket = NULL;
    AVFrame *avFrame = NULL;
    int sendPacketRes = 0;
    int receiveFrameRes = 0;
    AVCodecContext *pCodecContext = NULL;
    AVCodec *pCodec = NULL;
    int codecOpenRes = 0;
    AVCodecParameters *pCodecParameters;
    int codecContextParametersRes = 0;
    jobject audioTrack;
    jclass jAudioTrackClass;
    jmethodID jWriteMid;
    SwrContext *swrContext = NULL;
    uint8_t *resampleOutBuffer = NULL;
    enum AVSampleFormat inSampleFmt;
    enum AVSampleFormat outSampleFmt;
    int inSampleRate;
    int outSampleRate;
    uint64_t inChLayout;
    uint64_t outChLayout;
    int outChannelNb;
    int dataSize;

    int open_res = avformat_open_input(&av_format_context, url, NULL, NULL);
    if (open_res != 0) {
        LOGE("Can't open file: %s", av_err2str(open_res));
        return;
    }
    int find_stream_info_res = avformat_find_stream_info(av_format_context, NULL);
    if (find_stream_info_res < 0) {
        LOGE("Find stream info error: %s", av_err2str(find_stream_info_res));
        goto __avresource_close;
    }

    // 查找视频流
    audio_stream_idx = av_find_best_stream(av_format_context, AVMediaType::AVMEDIA_TYPE_AUDIO, -1,
                                           -1, NULL, 0);
    if (audio_stream_idx < 0) {
        LOGE("Find video stream info error: %s", av_err2str(find_stream_info_res));
        goto __avresource_close;
    }

    // 这个方法过时了
    // pCodecContext = av_format_context->streams[video_stream_idx]->codec;
    pCodecParameters = av_format_context->streams[audio_stream_idx]->codecpar;
    // 查找解码器
    pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if (!pCodec) {
        LOGE("Can't find audio decoder : %s", url);
        goto __avresource_close;
    }

    // 初始化创建 AVCodecContext
    pCodecContext = avcodec_alloc_context3(pCodec);
    codecContextParametersRes = avcodec_parameters_to_context(pCodecContext, pCodecParameters);
    if (codecContextParametersRes < 0) {
        LOGE("codec parameters to_context error : %s, %s", url,
             av_err2str(codecContextParametersRes));
        goto __avresource_close;
    }

    // 打开解码器
    codecOpenRes = avcodec_open2(pCodecContext, pCodec, NULL);
    if (codecOpenRes < 0) {
        LOGE("codec open error : %s, %s", url, av_err2str(codecOpenRes));
        goto __avresource_close;
    }

    // 创建初始化 AudioTrack
    audioTrack = initCreateAudioTrack(env);
    jAudioTrackClass = env->FindClass("android/media/AudioTrack");
    jWriteMid = env->GetMethodID(jAudioTrackClass, "write", "([BII)I");

    // 初始化重采样 ====================== start
    swrContext = swr_alloc();
    //输入的采样格式
    inSampleFmt = pCodecContext->sample_fmt;
    //输出采样格式16bit PCM
    outSampleFmt = AV_SAMPLE_FMT_S16;
    //输入采样率
    inSampleRate = pCodecContext->sample_rate;
    //输出采样率
    outSampleRate = AUDIO_SAMPLE_RATE;
    //获取输入的声道布局
    //根据声道个数获取默认的声道布局（2个声道，默认立体声stereo）
    inChLayout = pCodecContext->channel_layout;
    //输出的声道布局（立体声）
    outChLayout = AV_CH_LAYOUT_STEREO;

    swr_alloc_set_opts(swrContext, outChLayout, outSampleFmt, outSampleRate, inChLayout,
                       inSampleFmt, inSampleRate, 0, NULL);

    resampleOutBuffer = (uint8_t *) av_malloc(AUDIO_SAMPLES_SIZE_PER_CHANNEL);
    outChannelNb = av_get_channel_layout_nb_channels(outChLayout);
    dataSize = av_samples_get_buffer_size(NULL, outChannelNb, pCodecContext->frame_size,
                                          outSampleFmt, 1);

    if (swr_init(swrContext) < 0) {
        LOGE("Failed to initialize the resampling context");
        goto __avresource_close;
    }
    // 初始化重采样 ====================== end

    // 提取每一帧的音频流
    avPacket = av_packet_alloc();
    avFrame = av_frame_alloc();
    while (av_read_frame(av_format_context, avPacket) >= 0) {
        if (audio_stream_idx == avPacket->stream_index) {
            // 可以写入文件、截取、重采样、解码等等 avPacket.data
            sendPacketRes = avcodec_send_packet(pCodecContext, avPacket);
            if (sendPacketRes == 0) {
                receiveFrameRes = avcodec_receive_frame(pCodecContext, avFrame);

                if (receiveFrameRes == 0) {
                    // 往 AudioTrack 对象里面塞数据  avFrame->data -> jbyte;
                    swr_convert(swrContext, &resampleOutBuffer, avFrame->nb_samples,
                                (const uint8_t **) avFrame->data, avFrame->nb_samples);

                    jbyteArray jPcmDataArray = env->NewByteArray(dataSize);
                    jbyte *jPcmData = env->GetByteArrayElements(jPcmDataArray, NULL);
                    memcpy(jPcmData, resampleOutBuffer, dataSize);
                    // 同步刷新到 jbyteArray ，并释放 C/C++ 数组
                    env->ReleaseByteArrayElements(jPcmDataArray, jPcmData, 0);

                    // call java write
                    env->CallIntMethod(audioTrack, jWriteMid, jPcmDataArray, 0, dataSize);

                    // 解除 jPcmDataArray 的持有，让 javaGC 回收
                    env->DeleteLocalRef(jPcmDataArray);
                }
            }
            index++;
        }
        // av packet unref
        av_packet_unref(avPacket);
        av_frame_unref(avFrame);
    }

    // 释放资源 ============== start
    av_packet_free(&avPacket);
    av_frame_free(&avFrame);

    __avresource_close:

    if (swrContext != NULL) {
        swr_free(&swrContext);
    }

    env->DeleteLocalRef(audioTrack);

    if (resampleOutBuffer != NULL) {
        free(resampleOutBuffer);
    }

    if (pCodecContext != NULL) {
        avcodec_close(pCodecContext);
        avcodec_free_context(&pCodecContext);
    }

    if (av_format_context != NULL) {
        avformat_close_input(&av_format_context);
        avformat_free_context(av_format_context);
    }

    env->ReleaseStringUTFChars(url_, url);
    // 释放资源 ============== end
}

static JNIPlayerCall *jni_player_call = NULL;
JavaVM *javaVM;
static DZFFmpeg *fFmpeg = NULL;

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *j_vm, void *reserved) {
    jint result = -1;
    javaVM = j_vm;
    JNIEnv *env;
    if (javaVM->GetEnv((void **) (&env), JNI_VERSION_1_4) != JNI_OK) {
        return result;
    }
    // 可能会动态注册
    return JNI_VERSION_1_4;
}

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpegvideo_media_DarrenPlayer_nPrepare(JNIEnv *env, jobject instance, jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);
    if (fFmpeg == NULL) {
        if (jni_player_call == NULL) {
            jni_player_call = new JNIPlayerCall(javaVM, env, instance);
        }
        fFmpeg = new DZFFmpeg(jni_player_call, url);
        fFmpeg->prepared();
    }
    env->ReleaseStringUTFChars(url_, url);
}

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpegvideo_media_DarrenPlayer_nPrepareAsync(JNIEnv *env, jobject instance, jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);
    if (fFmpeg == NULL) {
        if (jni_player_call == NULL) {
            jni_player_call = new JNIPlayerCall(javaVM, env, instance);
        }

        fFmpeg = new DZFFmpeg(jni_player_call, url);
        fFmpeg->prepared_async();
    }

    env->ReleaseStringUTFChars(url_, url);
}

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpegvideo_media_DarrenPlayer_nStart(JNIEnv *env, jobject instance) {
    if (fFmpeg) {
        fFmpeg->start();
    }
}

/*
FILE *pcmFile;
uint8_t *out_buffer = NULL;

void bufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context) {
    if (!feof(pcmFile)) {
        int size = fread(out_buffer, 1, 44100 * 2 * 2, pcmFile);
        (*bufferQueueItf)->Enqueue(bufferQueueItf, out_buffer, size);
        LOGE("reading");
    } else {
        LOGE("读取完成！");
        fclose(pcmFile);
        free(out_buffer);
    }

    LOGE("bufferQueueCallback");
}*/


/*extern "C"
JNIEXPORT void JNICALL
Java_com_east_ffmpegvideo_ui_MainActivity_opslESPlay(JNIEnv *env, jobject instance,
                                                  jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);

    // TODO
    //读取pcm文件
    pcmFile = fopen(url, "r");
    if (pcmFile == NULL) {
        LOGE("%s", "fopen file error");
        return;
    }
    out_buffer = (uint8_t *) malloc(44100 * 2 * 2);

    SLObjectItf engine_obj;
    SLEngineItf engine;
    SLObjectItf mixObj;
    SLPlayItf slPlayItf;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    SLObjectItf pPlayer;
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean interfaceRequired[1] = {SL_BOOLEAN_FALSE};
    // 创建引擎
    slCreateEngine(&engine_obj, 0, 0, 0, 0, 0);
    (*engine_obj)->Realize(engine_obj, SL_BOOLEAN_FALSE);
    (*engine_obj)->GetInterface(engine_obj, SL_IID_ENGINE, &engine);
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
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource pAudioSrc = {&androidBufferQueue, &formatPcm};
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, mixObj};
    SLDataSink pAudioSnk = {&outputMix, NULL};
    const SLInterfaceID pInterfaceIds[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean pInterfaceRequired[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    (*engine)->CreateAudioPlayer(engine, &pPlayer, &pAudioSrc, &pAudioSnk,
                                 3, pInterfaceIds, pInterfaceRequired);
    (*pPlayer)->Realize(pPlayer, SL_BOOLEAN_FALSE);
    (*pPlayer)->GetInterface(pPlayer, SL_IID_PLAY, &slPlayItf);
    // 设置缓冲
    SLAndroidSimpleBufferQueueItf androidBufferQueueItf;
    (*pPlayer)->GetInterface(pPlayer, SL_IID_BUFFERQUEUE, &androidBufferQueueItf);
    (*androidBufferQueueItf)->RegisterCallback(androidBufferQueueItf, bufferQueueCallback, NULL);
    (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_PLAYING);
    //第六步----------------------------------------
    // 主动调用回调函数开始工作
    bufferQueueCallback(androidBufferQueueItf, NULL);

    env->ReleaseStringUTFChars(url_, url);
}*/

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpegvideo_media_DarrenPlayer_nPause(JNIEnv *env, jobject instance) {
    if (fFmpeg != NULL) {
        fFmpeg->onPause();
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpegvideo_media_DarrenPlayer_nResume(JNIEnv *env, jobject instance) {
    if (fFmpeg != NULL) {
        fFmpeg->onResume();
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpegvideo_media_DarrenPlayer_nStop(JNIEnv *env, jobject instance) {

    if (fFmpeg != NULL) {
        fFmpeg->release();
        delete fFmpeg;
        fFmpeg = NULL;
    }

    if (jni_player_call != NULL) {
        delete jni_player_call;
        jni_player_call = NULL;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpegvideo_media_DarrenPlayer_nSeekTo(JNIEnv *env, jobject instance, jint seconds) {
    LOGE("seconds = %d", seconds);
    // TODO
    if (fFmpeg != NULL) {
        fFmpeg->seek(seconds);
    }
}