#include <jni.h>
#include <string>
#include <android/log.h>
#include "ArrayQueue.hpp"

#define TAG "TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)


// 汉诺塔
// 1. n-1个盘子借助 C 从 A 挪动到 B 上面
// 2. 直接把 n 盘子从 A 挪动到 C
// 3. n-1 个盘子借助A 从 B 挪动到 C

void hannuota(int n,char start,char help,char end){
    if(n == 1){
        LOGE("把 %d 个盘子从 %c 挪动到 %c",n,start,end);
    }else{
        // 又是 n-1 汉诺塔
        hannuota(n-1,start,end,help);
        LOGE("把 %d 个盘子从 %c 挪动到 %c",n,start,end);
        hannuota(n-1,help,start,end);
    }
}

extern "C"
JNIEXPORT jstring JNICALL Java_com_east_datastructure32queue_MainActivity_stringFromJNI
(JNIEnv* env,jobject jobj){

    // hannuota(3,'A','B','C');

    ArrayQueue<int> queue(9);

    for (int i = 0; i < 10; ++i) {
        queue.enqueue(i);
    }

    while (!queue.isEmpty()){
        LOGE("弹出:%d",queue.dequeue());
    }

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

