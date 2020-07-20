#include <jni.h>
#include <string>
#include <android/log.h>
#include "map.hpp"

#define TAG "TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

using namespace std;


void visit(int key, int value, bool isRed) {
    if (isRed)
        LOGE("key = %d , value = %d , color = %s", key, value,"红");
    else
        LOGE("key = %d , value = %d , color = %s", key, value,"黑");
}


extern "C"
JNIEXPORT jstring JNICALL Java_com_east_datastructure40redblacktree_MainActivity_stringFromJNI
        (JNIEnv *env, jobject jobj) {


    map<int, int> *map = new ::map<int, int>();
    map->insert(3, 3);
    map->insert(2, 2);
    map->insert(1, 1);
    map->insert(4, 4);
    map->insert(5, 5);
    map->insert(-5, -5);
    map->insert(-15, -15);
    map->insert(-10, -10);
    map->insert(6, 6);
    map->insert(7, 7);



    map->remove(2);
    map->remove(-5);

    map->levelTraverse(visit);

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

