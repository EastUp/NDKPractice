//
// Created by 123 on 2020/10/9.
//

#include "JNICall.h"
#include "ConstDefine.h"

JNICall::JNICall(JavaVM *javaVM, JNIEnv *jniEnv, jobject jLiveObj) {
    this->javaVM = javaVM;
    this->jniEnv = jniEnv;
    this->jLiveObj = jniEnv->NewGlobalRef(jLiveObj);

    jclass jPlayerClass = jniEnv->GetObjectClass(jLiveObj);
    jConnectErrorMid = jniEnv->GetMethodID(jPlayerClass, "onConnectError", "(ILjava/lang/String;)V");
    jConnectSuccessMid = jniEnv->GetMethodID(jPlayerClass, "onConnectSuccess", "()V");
}

JNICall::~JNICall() {
    jniEnv->DeleteGlobalRef(jLiveObj);
}

void JNICall::callConnectError(ThreadMode threadMode, int code, char *msg) {
    // 子线程(pThread)用不了主线程(native线程)的 jniEnv
    // 子线程是不共享 jniEnv，他们有自己所独有的
    if (threadMode == THREAD_MAIN) {
        jstring jMsg = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jLiveObj, jConnectErrorMid, code, jMsg);
        jniEnv->DeleteLocalRef(jMsg);
    } else {
        // 通过 JavaVM获取当前线程的 JniEnv
        JNIEnv *env;
        if (javaVM->AttachCurrentThread(&env, nullptr) != JNI_OK) {
            LOGE("get child thread jniEnv error");
            return;
        }

        jstring jMsg = env->NewStringUTF(msg);
        env->CallVoidMethod(jLiveObj, jConnectErrorMid, code, jMsg);
        env->DeleteLocalRef(jMsg);

        javaVM->DetachCurrentThread();
    }

}

void JNICall::callConnectSuccess(ThreadMode mode) {
    // 子线程(pThread)用不了主线程(native线程)的 jniEnv
    // 子线程是不共享 jniEnv，他们有自己所独有的
    if (mode == THREAD_MAIN) {
        jniEnv->CallVoidMethod(jLiveObj, jConnectSuccessMid);
    } else {
        // 通过 JavaVM获取当前线程的 JniEnv
        JNIEnv *env;
        if (javaVM->AttachCurrentThread(&env, nullptr) != JNI_OK) {
            LOGE("get child thread jniEnv error");
            return;
        }
        env->CallVoidMethod(jLiveObj, jConnectSuccessMid);
        javaVM->DetachCurrentThread();
    }
}
