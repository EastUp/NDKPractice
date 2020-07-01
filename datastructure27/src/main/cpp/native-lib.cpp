#include <jni.h>
#include <string>
#include <malloc.h>
#include <android/log.h>
#include "LinkedList.hpp"

#define TAG "TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

extern "C"
JNIEXPORT jstring JNICALL Java_com_east_datastructure27_MainActivity_stringFromJNI
        (JNIEnv *env, jobject jobj) {

//    LinkedList<int> linkedList;
//    time_t start = clock();
//    for (int i = 0; i < 50000; ++i) {
//        linkedList.push(i);
//    }
//    time_t end = clock();
//
//    LOGE("耗费时间:%d",(end-start) / CLOCKS_PER_SEC);

    LinkedList<int> linkedList;
    linkedList.push(0);
    linkedList.push(1);
    linkedList.push(2);
    linkedList.push(3);
    linkedList.push(4);
    linkedList.push(5);

    linkedList.insert(0, -1);
    linkedList.insert(linkedList.size(), 3);
    linkedList.insert(2, 22);

    int number = linkedList.remove(0);
    linkedList.remove(linkedList.size()-1);
    linkedList.remove(2);

    for (int i = 0; i < linkedList.size(); ++i) {
        LOGE("%d", linkedList.get(i));
    };

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

