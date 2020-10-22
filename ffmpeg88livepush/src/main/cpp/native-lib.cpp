#include <jni.h>
#include "JNICall.h"
#include "LivePush.h"

LivePush *pLivePush = nullptr;
JNICall *pJniCall = nullptr;
JavaVM *pJavaVM = nullptr;

// 重写 so 被加载时会调用的一个方法
// 小作业，了解下动态注册
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *javaVM, void *reserved) {
    pJavaVM = javaVM;
    JNIEnv *env;
    if (javaVM->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}


extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpeg88livepush_LivePush_nInitConnect(JNIEnv *env, jobject instance,
                                                     jstring liveUrl_) {
    const char *liveUrl = env->GetStringUTFChars(liveUrl_, 0);
    pJniCall = new JNICall(pJavaVM, env, instance);
    pLivePush = new LivePush(pJniCall, liveUrl);
    pLivePush->initConnect();
    env->ReleaseStringUTFChars(liveUrl_, liveUrl);
}

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpeg88livepush_LivePush_nStop(JNIEnv *env, jobject instance) {
    if (pLivePush) {
        pLivePush->stop();
        delete pLivePush;
        pLivePush = nullptr;
    }

    if (pJniCall) {
        delete pJniCall;
        pJniCall = nullptr;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpeg88livepush_LivePush_pushSpsPps(JNIEnv *env, jobject instance,
                                                   jbyteArray spsData_,
                                                   jint spsLen, jbyteArray ppsData_, jint ppsLen) {
    jbyte *spsData = env->GetByteArrayElements(spsData_, NULL);
    jbyte *ppsData = env->GetByteArrayElements(ppsData_, NULL);


    if (pLivePush) {
        pLivePush->pushSpsPps(spsData, spsLen, ppsData, ppsLen);
    }

    env->ReleaseByteArrayElements(spsData_, spsData, 0);
    env->ReleaseByteArrayElements(ppsData_, ppsData, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_east_ffmpeg88livepush_LivePush_pushVideo(JNIEnv *env, jobject thiz, jbyteArray videoData_,
                                                  jint dataLen, jboolean keyFrame) {
    jbyte *videoData = env->GetByteArrayElements(videoData_, NULL);

    if (pLivePush) {
        pLivePush->pushVideoData(videoData,dataLen,keyFrame);
    }

    env->ReleaseByteArrayElements(videoData_, videoData, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_east_ffmpeg88livepush_LivePush_pushAudio(JNIEnv *env, jobject thiz, jbyteArray audioData_,
                                                  jint dataLen) {
    jbyte *audioData = env->GetByteArrayElements(audioData_, NULL);

    if (pLivePush) {
        pLivePush->pushAudioData(audioData,dataLen);
    }

    env->ReleaseByteArrayElements(audioData_, audioData, 0);
}