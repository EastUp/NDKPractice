#include <jni.h>
#include <string>
#include <android/log.h>

#include "ArrayList.hpp"

extern "C"

JNIEXPORT jstring JNICALL Java_com_east_cpp20_MainActivity_stringFromJNI
(JNIEnv* env,jobject jobj){
    ArrayList<int> *list = new ArrayList<int>();

    for(int i=0 ; i < 100; ++i){
        list->add(i);
    }

    list->remove(1);
    __android_log_print(ANDROID_LOG_ERROR,"TAG","size = %d",list->size());

    for(int i =0 ; i< list->size(); ++i){
        __android_log_print(ANDROID_LOG_ERROR,"TAG","i = %d",list->get(i));
    }

    delete(list);

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

