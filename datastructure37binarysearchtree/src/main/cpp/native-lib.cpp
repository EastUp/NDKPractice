#include <jni.h>
#include <string>
#include <android/log.h>
#include "BST.hpp"

#define TAG "TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

using namespace std;


void visit(int key, int value) {
    LOGE("key = %d , value = %d", key, value);
}



extern "C"
JNIEXPORT jstring JNICALL Java_com_east_datastructure37binarysearchtree_MainActivity_stringFromJNI
        (JNIEnv *env, jobject jobj) {


    BST<int, int> *bst = new BST<int, int>();
    bst->put(2, 2);
    bst->put(-11, -11);
    bst->put(-13, -13);
    bst->put(0, 0);
    bst->put(8, 8);
    bst->put(3, 3);
    bst->put(7, 7);

    bst->remove(2);
    bst->remove(-11);
    bst->remove(-13);
    bst->remove(0);
    bst->remove(8);
    bst->remove(3);
    bst->remove(7);

    bst->infixOrderTraverse(visit);

//    LOGE("%d", bst->contains(100));
//    LOGE("%d", bst->contains(7));

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

