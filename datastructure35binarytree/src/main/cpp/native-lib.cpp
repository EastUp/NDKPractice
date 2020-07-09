#include <jni.h>
#include <string>
#include <android/log.h>

#define TAG "TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)


extern "C"
JNIEXPORT jstring JNICALL Java_com_east_datastructure35binarytree_MainActivity_stringFromJNI
(JNIEnv* env,jobject jobj){

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

