//
// Created by 123 on 2020/10/9.
//

#include "JNICall.h"
#include "ConstDefine.h"

JNICall::JNICall(JavaVM *javaVM, JNIEnv *jniEnv, jobject jPlayerObj) {
    this->javaVM = javaVM;
    this->jniEnv = jniEnv;
    this->jPlayerObj = jPlayerObj;
    createAudioTrack();

    jclass jPlayerClass = jniEnv->GetObjectClass(jPlayerObj);
    jPlayerErrorMid = jniEnv->GetMethodID(jPlayerClass, "onError", "(ILjava/lang/String;)V");
}

void JNICall::createAudioTrack() {
    /*AudioTrack(int streamType, int sampleRateInHz, int channelConfig, int audioFormat,
       int bufferSizeInBytes, int mode)*/
    jclass jAudioTrackClass = jniEnv->FindClass("android/media/AudioTrack");
    jmethodID jAudioTackCMid = jniEnv->GetMethodID(jAudioTrackClass, "<init>", "(IIIIII)V");

    int streamType = 3;
    int sampleRateInHz = AUDIO_SAMPLE_RATE;
    int channelConfig = (0x4 | 0x8);
    int audioFormat = 2;
    int mode = 1;

    // int getMinBufferSize(int sampleRateInHz, int channelConfig, int audioFormat)
    jmethodID getMinBufferSizeMid = jniEnv->GetStaticMethodID(jAudioTrackClass, "getMinBufferSize",
            "(III)I");
    int bufferSizeInBytes = jniEnv->CallStaticIntMethod(jAudioTrackClass, getMinBufferSizeMid,
            sampleRateInHz, channelConfig, audioFormat);
    LOGE("bufferSizeInBytes = %d", bufferSizeInBytes);

    jAudioTrackObj = jniEnv->NewObject(jAudioTrackClass, jAudioTackCMid, streamType,
            sampleRateInHz, channelConfig, audioFormat, bufferSizeInBytes, mode);

    // start  method
    jmethodID playMid = jniEnv->GetMethodID(jAudioTrackClass, "play", "()V");
    jniEnv->CallVoidMethod(jAudioTrackObj, playMid);

    // write method
    jAudioTrackWriteMid = jniEnv->GetMethodID(jAudioTrackClass, "write", "([BII)I");
}

void JNICall::callAudioTrackWrite(jbyteArray audioData, int offsetInBytes, int sizeInBytes) {
    jniEnv->CallIntMethod(jAudioTrackObj, jAudioTrackWriteMid, audioData, offsetInBytes,
                          sizeInBytes);
}

JNICall::~JNICall() {
    jniEnv->DeleteLocalRef(jAudioTrackObj);
}

void JNICall::callPlayerError(int code, char *msg) {
    jstring jMsg = jniEnv->NewStringUTF(msg);
    jniEnv->CallVoidMethod(jPlayerObj, jPlayerErrorMid, code, jMsg);
    jniEnv->DeleteLocalRef(jMsg);
}