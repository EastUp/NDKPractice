//
// Created by 123 on 2020/10/9.
//

#ifndef NDKPRACTICE_JNICALL_H
#define NDKPRACTICE_JNICALL_H

#include <jni.h>

class JNICall {
public:
    jobject jAudioTrackObj;
    jmethodID jAudioTrackWriteMid;
    JavaVM *javaVM;
    JNIEnv *jniEnv;
    jmethodID jPlayerErrorMid;
    jobject jPlayerObj;

public:
    JNICall(JavaVM *javaVM,JNIEnv *jniEnv,jobject jplayerObj);
    ~JNICall();

private:
    void createAudioTrack();

public:
    void callAudioTrackWrite(jbyteArray audioData,int offsetInBytes,int sizeInBytes);

    void callPlayerError(int code,char *msg);
};

#endif //NDKPRACTICE_JNICALL_H
