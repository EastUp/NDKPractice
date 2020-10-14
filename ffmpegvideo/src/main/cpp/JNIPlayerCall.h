//
// Created by 曾辉 on 2019-06-05.
//

#include <jni.h>
#include "android_log.h"

#ifndef NDK_DAY03_JNICALL_H
#define NDK_DAY03_JNICALL_H

enum ThreadType {
    THREAD_MAIN, THREAD_CHILD
};

class JNIPlayerCall {
public:
    JavaVM *java_vm = NULL;
    JNIEnv *jni_env = NULL;
    jobject player_obj;
    jmethodID prepared_mid;
    jmethodID loadingMid;
    jmethodID progressMid;
    jmethodID errorMid;
    jmethodID completeMid;
    jmethodID renderMid;
    jmethodID isSupportStiffCodecMid;
    jmethodID initMediaCodecMid;
    jmethodID decodePacketMid;
public:
    JNIPlayerCall(JavaVM *java_vm, JNIEnv *jni_env, jobject player_obj);

    ~JNIPlayerCall();

    void onCallPrepared(ThreadType thread_type);

    void onCallLoading(ThreadType thread_type, bool loading);

    void onCallProgress(ThreadType threadType, int current, int total);

    void onCallError(ThreadType threadType, int errorCode, const char *errorMsg);

    void onCallComplete(ThreadType threadType);

    void onCallRenderYUV420P(int width, int height, uint8_t *fy, uint8_t *fu, uint8_t *fv);

    bool onCallIsSupportStiffCodec(ThreadType threadType, const char *codecName);

    void onCallInitMediaCodec(ThreadType threadType, const char *mime, int width, int height,
            int csd0Size, int csd1Size, uint8_t *csd0, uint8_t *csd1);

    void onCallDecodePacket(int i, uint8_t *string);
};


#endif //NDK_DAY03_JNICALL_H
