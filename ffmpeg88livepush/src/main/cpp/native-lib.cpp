#include <jni.h>
#include "JNICall.h"
#include "LivePush.h"

LivePush *pLivePush = nullptr;
JNICall *pJniCall = nullptr;
JavaVM *pJavaVM = nullptr;

// 重写 so 被加载时会调用的一个方法
// 小作业，了解下动态注册
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *javaVM,void *reserved){
    pJavaVM = javaVM;
    JNIEnv *env;
    if(javaVM->GetEnv((void **)&env,JNI_VERSION_1_6) != JNI_OK){
        return -1;
    }
    return JNI_VERSION_1_6;
}


extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpeg88livepush_LivePush_nInitConnect(JNIEnv *env, jobject instance,
                                               jstring liveUrl_) {
    const char *liveUrl = env->GetStringUTFChars(liveUrl_,0);
    pJniCall = new JNICall(pJavaVM,env,instance);
    pLivePush = new LivePush(pJniCall,liveUrl);
    pLivePush->initConnect();
    env->ReleaseStringUTFChars(liveUrl_, liveUrl);
}