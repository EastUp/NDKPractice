#include <jni.h>
#include "JNICall.h"
#include "FFmpeg.h"


JNICall *pJniCall;
FFmpeg *pFFmpeg;

extern "C" JNIEXPORT void JNICALL
Java_com_east_ffmpeg83_media_JaminPlayer_nPlay(JNIEnv *env, jobject instance,
                                                       jstring url_) {
    const char *url = env->GetStringUTFChars(url_,0);
    pJniCall = new JNICall(NULL,env,instance);
    pFFmpeg = new FFmpeg(pJniCall,url);
    pFFmpeg->play();
//    delete pJniCall;
//    delete pFFmpeg;
    env->ReleaseStringUTFChars(url_, url);
}