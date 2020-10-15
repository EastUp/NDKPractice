//
// Created by 123 on 2020/10/9.
//

#ifndef NDKPRACTICE_JNICALL_H
#define NDKPRACTICE_JNICALL_H

#include <jni.h>

enum ThreadMode{
    THREAD_CHILD,THREAD_MAIN
};

class JNICall {
public:
    JavaVM *javaVM;
    JNIEnv *jniEnv;
    jmethodID jConnectErrorMid;
    jmethodID jConnectSuccessMid;
    jobject jLiveObj;

public:
    JNICall(JavaVM *javaVM,JNIEnv *jniEnv,jobject jplayerObj);
    ~JNICall();


public:
    void callConnectError(ThreadMode threadMode,int code,char *msg);

    void callConnectSuccess(ThreadMode mode);
};

#endif //NDKPRACTICE_JNICALL_H
