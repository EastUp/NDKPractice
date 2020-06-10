#include <jni.h>
#include <string>
#include <android/log.h>  //系统自带的用<>


extern "C"
JNIEXPORT jstring JNICALL
Java_com_east_ndk02ffmpegvideocompress_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
