#include <jni.h>
#include <string>
#include <android/log.h>
#include "ArrayStack.hpp"
#include "LinkedStack.hpp"

#define TAG "TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

// 递归：
// 1. 肯定有递归到底的结束
// 2. 求解的思路能够拆分很多的小部分，每个小部分求解思路一致
// 3. 打得部分是依赖于每一个小的部分的解决
int sum(int n){
    // n 阶乘 需要借助 n-1 阶乘 *n n-1 阶乘借助 n-1-1
    if(n == 1)
        return 1;
    return n * sum(n-1);
}
// 循环和递归的区别


extern "C"
JNIEXPORT jstring JNICALL Java_com_east_datastructure31recursionandstack_MainActivity_stringFromJNI
(JNIEnv* env,jobject jobj){
    int s = sum(5);
    LOGE("sum = %d",s);

    LinkedStack<int> stack;

    stack.push(1);
    stack.push(2);
/*    stack.push(3);

    LOGE("%d",stack.pop());
    LOGE("%d",stack.pop());

    stack.push(4);

    LOGE("%d",stack.pop());
    LOGE("%d",stack.pop());*/

    for (int i = 0; i < 10; ++i) {
        stack.push(i);
    }

    while(!stack.isEmpty()){
        LOGE("%d",stack.pop());
    }


    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

