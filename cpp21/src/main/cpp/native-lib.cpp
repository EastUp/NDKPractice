#include <jni.h>
#include <string>
#include <android/log.h>
#include <stdexcept>

using namespace std;

class Exception: public out_of_range{
public:
    string msg;
public:
    Exception(string msg):msg(msg),out_of_range(msg){}

    const char* what() {
        return msg.c_str();
    }
};

// NDK 异常总结
// 1. 在 c++ 层如果是自己写的代码或者调用别人的方法，记得要 try 住，如果不 try 在 java 层 try 是没有意义的
// 2. 如果异常需要往外抛给 java 层，一定要按照 java 层抛异常的方式
// 3. 如果是自己写的 NDK 层的代码，最好抛自己写的异常，声明异常
// 4. 如果是做 c++/c ， 或者是帮 c/c++ 写代码，最好抛系统定义好的异常或者继承系统的异常
// 5. 系统异常的体系 exception 基类 https://www.cnblogs.com/QG-whz/p/5136883.html
void c_method() throw(Exception,int){ // 如果是自己写的， 1000 多行
    throw Exception("异常了");
}


extern "C"
JNIEXPORT void JNICALL Java_com_east_cpp21_MainActivity_arraycopy
(JNIEnv* env, jobject jclaz,
        jobject src,jint srcPos,
        jobject dest,jint destPos,
        jint length){

    // 做一系列的判断，是不是数组
    // 做一系列的准备工作，如果有异常了，挂了，程序时会崩掉的
    // bugly so库保存，如果有抛 java 层的异常，是可以 try ,但是很多情况下并不会抛 java 层的异常
    // 如果没抛 java 层的异常，在 java 层 try 了依旧会崩溃
    // src
    // dest
    // 都是 java 数组，需要类型转换 reinterpret_Cast static_cast dynamic_cast(最佳)
    jobjectArray src_array = reinterpret_cast<jobjectArray>(src);
    jobjectArray dest_array = reinterpret_cast<jobjectArray>(dest);
    if(src_array && dest_array){
        __android_log_print(ANDROID_LOG_ERROR,"TAG","转换成功");

        // 设置值
        for(int i = 0; i< length ; i ++){
            jobject  obj = env->GetObjectArrayElement(src_array,srcPos+i);
            // 放到新数组里面
            env->SetObjectArrayElement(dest_array,destPos+i,obj);
        }
        // 完毕
    }

    try {
        c_method();
    }catch (Exception exception){
        // 这个异常可以抛给 java 层，但是要抛成 java 层的异常
        jclass  je = env->FindClass("java/lang/Exception");
        env ->ThrowNew(je,exception.what());
    }


}

