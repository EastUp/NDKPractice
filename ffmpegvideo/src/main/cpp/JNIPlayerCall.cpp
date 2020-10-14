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
    renderMid = jni_env->GetMethodID(player_class, "onRenderYUV420P", "(II[B[B[B)V");
    isSupportStiffCodecMid = jni_env->GetMethodID(player_class, "isSupportStiffCodec",
            "(Ljava/lang/String;)Z");
    initMediaCodecMid = jni_env->GetMethodID(player_class, "initMediaCodec",
            "(Ljava/lang/String;II[B[B)V");
    decodePacketMid = jni_env->GetMethodID(player_class, "decodePacket", "(I[B)V");
}

void JNIPlayerCall::onCallPrepared(ThreadType thread_type) {
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

void JNIPlayerCall::onCallLoading(ThreadType threadType, bool loading) {
    if (threadType == THREAD_MAIN) {
        jni_env->CallVoidMethod(player_obj, loadingMid, loading);
    } else if (threadType == THREAD_CHILD) {
        JNIEnv *env;
        if (java_vm->AttachCurrentThread(&env, 0) != JNI_OK) {
            LOGE("get child thread jnienv error.");
            return;
        }

        env->CallVoidMethod(player_obj, loadingMid, loading);

        java_vm->DetachCurrentThread();
    }
}

void JNIPlayerCall::onCallProgress(ThreadType threadType, int current, int total) {
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

void JNIPlayerCall::onCallError(ThreadType threadType, int errorCode, const char *errorMsg) {
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

void JNIPlayerCall::onCallComplete(ThreadType threadType) {
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

bool JNIPlayerCall::onCallIsSupportStiffCodec(ThreadType threadType, const char *codecName) {
    bool support = false;

    if (threadType == THREAD_MAIN) {
        jstring jCodecName = jni_env->NewStringUTF(codecName);
        support = jni_env->CallBooleanMethod(player_obj, isSupportStiffCodecMid, jCodecName);
        jni_env->DeleteLocalRef(jCodecName);
    } else if (threadType == THREAD_CHILD) {
        JNIEnv *env;
        if (java_vm->AttachCurrentThread(&env, 0) != JNI_OK) {
            LOGE("get child thread jnienv error.");
            return false;
        }

        jstring jCodecName = env->NewStringUTF(codecName);
        support = env->CallBooleanMethod(player_obj, isSupportStiffCodecMid, jCodecName);
        env->DeleteLocalRef(jCodecName);

        java_vm->DetachCurrentThread();
    }

    return support;
}

void
JNIPlayerCall::onCallRenderYUV420P(int width, int height, uint8_t *fy, uint8_t *fu, uint8_t *fv) {
    JNIEnv *jniEnv;
    if (java_vm->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
        LOGE("get child thread jnienv error.");
        return;
    }

    jbyteArray y = jniEnv->NewByteArray(width * height);
    jniEnv->SetByteArrayRegion(y, 0, width * height, reinterpret_cast<const jbyte *>(fy));

    jbyteArray u = jniEnv->NewByteArray(width * height / 4);
    jniEnv->SetByteArrayRegion(u, 0, width * height / 4, reinterpret_cast<const jbyte *>(fu));

    jbyteArray v = jniEnv->NewByteArray(width * height / 4);
    jniEnv->SetByteArrayRegion(v, 0, width * height / 4, reinterpret_cast<const jbyte *>(fv));

    jniEnv->CallVoidMethod(player_obj, renderMid, width, height, y, u, v);

    jniEnv->DeleteLocalRef(y);
    jniEnv->DeleteLocalRef(u);
    jniEnv->DeleteLocalRef(v);

    java_vm->DetachCurrentThread();
}

void
JNIPlayerCall::onCallInitMediaCodec(ThreadType threadType, const char *mime, int width, int height,
        int csd0Size, int csd1Size, uint8_t *csd0, uint8_t *csd1) {
    if (threadType == THREAD_MAIN) {
        jstring type = jni_env->NewStringUTF(mime);
        jbyteArray csd0 = jni_env->NewByteArray(csd0Size);
        jni_env->SetByteArrayRegion(csd0, 0, csd0Size, reinterpret_cast<const jbyte *>(csd0));
        jbyteArray csd1 = jni_env->NewByteArray(csd1Size);
        jni_env->SetByteArrayRegion(csd1, 0, csd1Size, reinterpret_cast<const jbyte *>(csd1));

        jni_env->CallVoidMethod(player_obj, initMediaCodecMid, type, width, height, csd0, csd1);

        jni_env->DeleteLocalRef(csd0);
        jni_env->DeleteLocalRef(csd1);
        jni_env->DeleteLocalRef(type);
    } else {
        JNIEnv *jniEnv;
        if (java_vm->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            LOGE("get child thread jnienv error.");
        }

        jstring type = jniEnv->NewStringUTF(mime);
        jbyteArray jCsd0 = jniEnv->NewByteArray(csd0Size);
        jniEnv->SetByteArrayRegion(jCsd0, 0, csd0Size, reinterpret_cast<const jbyte *>(csd0));
        jbyteArray jCsd1 = jniEnv->NewByteArray(csd1Size);
        jniEnv->SetByteArrayRegion(jCsd1, 0, csd1Size, reinterpret_cast<const jbyte *>(csd1));

        jniEnv->CallVoidMethod(player_obj, initMediaCodecMid, type, width, height, jCsd0, jCsd1);

        jniEnv->DeleteLocalRef(jCsd0);
        jniEnv->DeleteLocalRef(jCsd1);
        jniEnv->DeleteLocalRef(type);
        java_vm->DetachCurrentThread();
    }
}

void JNIPlayerCall::onCallDecodePacket(int dataSize, uint8_t *data) {
    JNIEnv *jniEnv;
    if (java_vm->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
        LOGE("get child thread jnienv error.");
    }
    jbyteArray jData = jniEnv->NewByteArray(dataSize);
    jniEnv->SetByteArrayRegion(jData, 0, dataSize, reinterpret_cast<const jbyte *>(data));
    jniEnv->CallVoidMethod(player_obj, decodePacketMid, dataSize, jData);
    jniEnv->DeleteLocalRef(jData);
    java_vm->DetachCurrentThread();
}
