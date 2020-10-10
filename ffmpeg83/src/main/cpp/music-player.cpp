#include <jni.h>
#include "JNICall.h"
#include "FFmpeg.h"

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
Java_com_east_ffmpeg83_media_JaminPlayer_nPlay(JNIEnv *env, jobject instance,
                                                       jstring url_) {
    if(pFFmpeg)
        pFFmpeg->play();
}

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpeg83_media_JaminPlayer_nPrepare(JNIEnv *env, jobject instance,
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
Java_com_east_ffmpeg83_media_JaminPlayer_nPrepareAsync(JNIEnv *env, jobject instance,
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