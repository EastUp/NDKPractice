//
// Created by 曾辉 on 2019-06-05.
//

#include <android/log.h>
#include "JNIPlayerCall.h"

JNIPlayerCall::JNIPlayerCall(JavaVM *java_vm, JNIEnv *jni_env, jobject player_obj) {
    this->java_vm = java_vm;
    this->jni_env = jni_env;
    this->player_obj = jni_env->NewGlobalRef(player_obj);
    // 初始化 prepared_mid
    jclass player_class = jni_env->GetObjectClass(player_obj);
    prepared_mid = jni_env->GetMethodID(player_class, "onPrepared", "()V");
    loadingMid = jni_env->GetMethodID(player_class, "onLoading", "(Z)V");
    progressMid = jni_env->GetMethodID(player_class, "onProgress", "(II)V");
    errorMid = jni_env->GetMethodID(player_class, "onError", "(ILjava/lang/String;)V");
    completeMid = jni_env->GetMethodID(player_class, "onComplete", "()V");
}

void JNIPlayerCall::onCallPrepared(THREAD_TYPE thread_type) {
    if (thread_type == THREAD_MAIN) {
        jni_env->CallVoidMethod(player_obj, prepared_mid);
    } else if (thread_type == THREAD_CHILD) {
        JNIEnv *env;
        if (java_vm->AttachCurrentThread(&env, 0) != JNI_OK) {
            LOGE("get child thread jnienv error.");
            return;
        }

        env->CallVoidMethod(player_obj, prepared_mid);

        java_vm->DetachCurrentThread();
    }
}

JNIPlayerCall::~JNIPlayerCall() {
    jni_env->DeleteGlobalRef(player_obj);
}

void JNIPlayerCall::onCallLoading(THREAD_TYPE thread_type, bool loading) {
    if (thread_type == THREAD_MAIN) {
        jni_env->CallVoidMethod(player_obj, loadingMid, loading);
    } else if (thread_type == THREAD_CHILD) {
        JNIEnv *env;
        if (java_vm->AttachCurrentThread(&env, 0) != JNI_OK) {
            LOGE("get child thread jnienv error.");
            return;
        }

        env->CallVoidMethod(player_obj, loadingMid, loading);

        java_vm->DetachCurrentThread();
    }
}

void JNIPlayerCall::onCallProgress(THREAD_TYPE threadType, int current, int total) {
    if (threadType == THREAD_MAIN) {
        jni_env->CallVoidMethod(player_obj, progressMid, current, total);
    } else if (threadType == THREAD_CHILD) {
        JNIEnv *env;
        if (java_vm->AttachCurrentThread(&env, 0) != JNI_OK) {
            LOGE("get child thread jnienv error.");
            return;
        }

        env->CallVoidMethod(player_obj, progressMid, current, total);

        java_vm->DetachCurrentThread();
    }
}

void JNIPlayerCall::onCallError(THREAD_TYPE threadType, int errorCode, const char *errorMsg) {
    if (threadType == THREAD_MAIN) {
        jstring jErrorMsg = jni_env->NewStringUTF(errorMsg);
        jni_env->CallVoidMethod(player_obj, errorMid, errorCode, jErrorMsg);
        jni_env->DeleteLocalRef(jErrorMsg);
    } else if (threadType == THREAD_CHILD) {
        JNIEnv *env;
        if (java_vm->AttachCurrentThread(&env, 0) != JNI_OK) {
            LOGE("get child thread jnienv error.");
            return;
        }

        jstring jErrorMsg = env->NewStringUTF(errorMsg);
        env->CallVoidMethod(player_obj, errorMid, errorCode, jErrorMsg);
        env->DeleteLocalRef(jErrorMsg);

        java_vm->DetachCurrentThread();
    }
}

void JNIPlayerCall::onCallComplete(THREAD_TYPE threadType) {
    if (threadType == THREAD_MAIN) {
        jni_env->CallVoidMethod(player_obj, completeMid);
    } else if (threadType == THREAD_CHILD) {
        JNIEnv *env;
        if (java_vm->AttachCurrentThread(&env, 0) != JNI_OK) {
            LOGE("get child thread jnienv error.");
            return;
        }
        env->CallVoidMethod(player_obj, completeMid);
        java_vm->DetachCurrentThread();
    }
}
