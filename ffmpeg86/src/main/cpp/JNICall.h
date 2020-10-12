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
    jobject jAudioTrackObj;
    jmethodID jAudioTrackWriteMid;
    JavaVM *javaVM;
    JNIEnv *jniEnv;
    jmethodID jPlayerErrorMid;
    jmethodID jPlayerPreparedMid;
    jobject jPlayerObj;

public:
    JNICall(JavaVM *javaVM,JNIEnv *jniEnv,jobject jplayerObj);
    ~JNICall();


public:
    void createAudioTrack(JNIEnv *env);

    void callAudioTrackWrite(JNIEnv *env,jbyteArray audioData,int offsetInBytes,int sizeInBytes);

    void callPlayerError(ThreadMode threadMode,int code,char *msg);

    void CallPlayerPrepared(ThreadMode mode);
};

#endif //NDKPRACTICE_JNICALL_H
