//
// Created by east on 2020-10-14.
//

#include <jni.h>
#include "android_log.h"

#ifndef NDK_DAY03_JNICALL_H
#define NDK_DAY03_JNICALL_H

enum THREAD_TYPE {
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

public:
    JNIPlayerCall(JavaVM *java_vm, JNIEnv *jni_env, jobject player_obj);

    ~JNIPlayerCall();

    void onCallPrepared(THREAD_TYPE thread_type);

    void onCallLoading(THREAD_TYPE thread_type, bool loading);

    void onCallProgress(THREAD_TYPE threadType, int current, int total);

    void onCallError(THREAD_TYPE threadType, int errorCode, const char* errorMsg);

    void onCallComplete(THREAD_TYPE threadType);
};


#endif //NDK_DAY03_JNICALL_H
