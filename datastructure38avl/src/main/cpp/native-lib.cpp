#include <jni.h>
#include <string>
#include <android/log.h>
#include "AVL.hpp"

#define TAG "TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

using namespace std;


void visit(int key, int value) {
    LOGE("key = %d , value = %d", key, value);
}



extern "C"
JNIEXPORT jstring JNICALL Java_com_east_datastructure38avl_MainActivity_stringFromJNI
        (JNIEnv *env, jobject jobj) {


    AVL<int, int> *avl = new AVL<int, int>();
    avl->put(3, 3);
    avl->put(1, 1);
    avl->put(2, 2);
    avl->put(4, 4);
    avl->put(5, 5);
    avl->put(6, 6);
    avl->put(7, 7);
    avl->put(10, 10);
    avl->put(9, 9);
    avl->put(8, 8);

    avl->remove(6);
    avl->remove(5);

    avl->levelOrderTraverse(visit);


    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

