//
// Created by 123 on 2020/10/9.
//

#include "JNICall.h"
#include "ConstDefine.h"

JNICall::JNICall(JavaVM *javaVM, JNIEnv *jniEnv, jobject jPlayerObj) {
    this->javaVM = javaVM;
    this->jniEnv = jniEnv;
    this->jPlayerObj = jniEnv->NewGlobalRef(jPlayerObj);

    jclass jPlayerClass = jniEnv->GetObjectClass(jPlayerObj);
    jPlayerErrorMid = jniEnv->GetMethodID(jPlayerClass, "onError", "(ILjava/lang/String;)V");
    jPlayerPreparedMid = jniEnv->GetMethodID(jPlayerClass, "onPrepared", "()V");
}

void JNICall::createAudioTrack(JNIEnv *env) {
    if (env) {
        /*AudioTrack(int streamType, int sampleRateInHz, int channelConfig, int audioFormat,
      int bufferSizeInBytes, int mode)*/
        jclass jAudioTrackClass = env->FindClass("android/media/AudioTrack");
        jmethodID jAudioTackCMid = env->GetMethodID(jAudioTrackClass, "<init>", "(IIIIII)V");

        int streamType = 3;
        int sampleRateInHz = AUDIO_SAMPLE_RATE;
        int channelConfig = (0x4 | 0x8);
        int audioFormat = 2;
        int mode = 1;

        // int getMinBufferSize(int sampleRateInHz, int channelConfig, int audioFormat)
        jmethodID getMinBufferSizeMid = env->GetStaticMethodID(jAudioTrackClass, "getMinBufferSize",
                                                               "(III)I");
        int bufferSizeInBytes = env->CallStaticIntMethod(jAudioTrackClass, getMinBufferSizeMid,
                                                         sampleRateInHz, channelConfig,
                                                         audioFormat);
        LOGE("bufferSizeInBytes = %d", bufferSizeInBytes);

        jAudioTrackObj = env->NewObject(jAudioTrackClass, jAudioTackCMid, streamType,
                                        sampleRateInHz, channelConfig, audioFormat,
                                        bufferSizeInBytes, mode);

        // start  method
        jmethodID playMid = env->GetMethodID(jAudioTrackClass, "play", "()V");
        env->CallVoidMethod(jAudioTrackObj, playMid);

        // write method
        jAudioTrackWriteMid = env->GetMethodID(jAudioTrackClass, "write", "([BII)I");
    } else {
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
        jmethodID getMinBufferSizeMid = jniEnv->GetStaticMethodID(jAudioTrackClass,
                                                                  "getMinBufferSize",
                                                                  "(III)I");
        int bufferSizeInBytes = jniEnv->CallStaticIntMethod(jAudioTrackClass, getMinBufferSizeMid,
                                                            sampleRateInHz, channelConfig,
                                                            audioFormat);
        LOGE("bufferSizeInBytes = %d", bufferSizeInBytes);

        jAudioTrackObj = jniEnv->NewObject(jAudioTrackClass, jAudioTackCMid, streamType,
                                           sampleRateInHz, channelConfig, audioFormat,
                                           bufferSizeInBytes, mode);

        // start  method
        jmethodID playMid = jniEnv->GetMethodID(jAudioTrackClass, "play", "()V");
        jniEnv->CallVoidMethod(jAudioTrackObj, playMid);

        // write method
        jAudioTrackWriteMid = jniEnv->GetMethodID(jAudioTrackClass, "write", "([BII)I");
    }

}

void JNICall::callAudioTrackWrite(JNIEnv *env, jbyteArray audioData, int offsetInBytes,
                                  int sizeInBytes) {
    if (env)
        env->CallIntMethod(jAudioTrackObj, jAudioTrackWriteMid, audioData, offsetInBytes,
                           sizeInBytes);
    else
        jniEnv->CallIntMethod(jAudioTrackObj, jAudioTrackWriteMid, audioData, offsetInBytes,
                              sizeInBytes);
}

JNICall::~JNICall() {
    jniEnv->DeleteLocalRef(jAudioTrackObj);
    jniEnv->DeleteGlobalRef(jPlayerObj);
}

void JNICall::callPlayerError(ThreadMode threadMode, int code, char *msg) {
    // 子线程(pThread)用不了主线程(native线程)的 jniEnv
    // 子线程是不共享 jniEnv，他们有自己所独有的
    if (threadMode == THREAD_MAIN) {
        jstring jMsg = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jPlayerObj, jPlayerErrorMid, code, jMsg);
        jniEnv->DeleteLocalRef(jMsg);
    } else {
        // 通过 JavaVM获取当前线程的 JniEnv
        JNIEnv *env;
        if (javaVM->AttachCurrentThread(&env, nullptr) != JNI_OK) {
            LOGE("get child thread jniEnv error");
            return;
        }

        jstring jMsg = env->NewStringUTF(msg);
        env->CallVoidMethod(jPlayerObj, jPlayerErrorMid, code, jMsg);
        env->DeleteLocalRef(jMsg);

        javaVM->DetachCurrentThread();
    }

}

void JNICall::callPlayerPrepared(ThreadMode mode) {
    // 子线程(pThread)用不了主线程(native线程)的 jniEnv
    // 子线程是不共享 jniEnv，他们有自己所独有的
    if (mode == THREAD_MAIN) {
        jniEnv->CallVoidMethod(jPlayerObj, jPlayerPreparedMid);
    } else {
        // 通过 JavaVM获取当前线程的 JniEnv
        JNIEnv *env;
        if (javaVM->AttachCurrentThread(&env, nullptr) != JNI_OK) {
            LOGE("get child thread jniEnv error");
            return;
        }
        env->CallVoidMethod(jPlayerObj, jPlayerPreparedMid);
        javaVM->DetachCurrentThread();
    }
}
