#include <jni.h>
#include "JNICall.h"
#include "FFmpeg.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>

// 在 c++ 中采用 c 的这种编译方式
extern "C" {
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
}

JNICall *pJniCall;
FFmpeg *pFFmpeg;

JavaVM *pJavaVM = NULL;

// 重写 so 被加载时会调用的一个方法
// 小作业，了解下动态注册
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *javaVM,void *reserved){
    pJavaVM = javaVM;
    JNIEnv *env;
    if(javaVM->GetEnv((void **)&env,JNI_VERSION_1_4) != JNI_OK){
        return -1;
    }
    return JNI_VERSION_1_4;
}


extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpeg86_media_JaminPlayer_nPlay(JNIEnv *env, jobject instance) {
    if(pFFmpeg)
        pFFmpeg->play();
}

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpeg86_media_JaminPlayer_nPrepare(JNIEnv *env, jobject instance,
                                               jstring url_) {
    const char *url = env->GetStringUTFChars(url_,0);
    if(pFFmpeg == NULL){
        pJniCall = new JNICall(pJavaVM,env,instance);
        pFFmpeg = new FFmpeg(pJniCall,url);
        pFFmpeg->prepare();
        //    pFFmpeg->prepare();
        //    delete pJniCall;
        //    delete pFFmpeg;
    }
    env->ReleaseStringUTFChars(url_, url);
}

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpeg86_media_JaminPlayer_nPrepareAsync(JNIEnv *env, jobject instance,
                                                  jstring url_) {
    const char *url = env->GetStringUTFChars(url_,0);
    if(pFFmpeg == NULL){
        pJniCall = new JNICall(pJavaVM,env,instance);
        pFFmpeg = new FFmpeg(pJniCall,url);
        pFFmpeg->prepareAsync();
        //    pFFmpeg->prepare();
        //    delete pJniCall;
        //    delete pFFmpeg;
    }
    env->ReleaseStringUTFChars(url_, url);
}

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpeg86_media_JaminPlayer_setSurface(JNIEnv *env, jobject instance
            ,jobject surface) {
    if(pJniCall->jniEnv == env){
        LOGE("env 相等哦");
    }else{
        LOGE("env 不相等哦");
    }
    if(pFFmpeg){
        pFFmpeg->setSurface(env,surface);
    }
}