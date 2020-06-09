#include <jni.h>
#include <string>
#include <android/log.h>  //系统自带的用<>
#include "md5.h" //第三方的使用""

using namespace std;

//extern "C" JNIEXPORT jstring JNICALL
//Java_com_east_ndkpractice_MainActivity_stringFromJNI(
//        JNIEnv* env,
//        jobject /* this */) {
//    std::string hello = "Hello from C++";
//    return env->NewStringUTF(hello.c_str());
//}

//额外在开头添加的字符串
static char *EXTRA_SIGNATURE = "EAST";


extern "C"
JNIEXPORT jstring JNICALL
Java_com_east_ndkpractice_SignatureUtils_signatureParams(JNIEnv *env, jclass clazz,
                                                         jstring params_) {
    const char *params = env ->GetStringUTFChars(params_,0);

    //MD5签名 根据自己项目的需要制定规则
    // 1.字符串签名加上前缀
    string signature_str(params);
    signature_str.insert(0,EXTRA_SIGNATURE);

    // 2.后面去掉2位
    signature_str = signature_str.substr(0,signature_str.length()-2);

    // 3.MD5加密 C++ 和 java 大同小异,唯一不同的就是C++需要 自己回收内存
    MD5_CTX *ctx = new MD5_CTX();
    MD5Init(ctx);
    MD5Update(ctx, (unsigned char *) signature_str.c_str(), signature_str.length());
    unsigned char digest[16] = {0};
    MD5Final(digest,ctx);

    // 生成 32 位的字符串
    //md5_str[32]md5_str[33]留一位给/0结束符；
    //否则报数组越界。坑。。。。。。。。。。。。。。。。
    char md5_str[33] = {0};
    for(int i = 0; i < 16; i++){
        // 最终生成 32 位 ，不足的前面补0
        sprintf(md5_str,"%s%02x",md5_str,digest[i]);
    }

    //释放资源
    env->ReleaseStringUTFChars(params_,params);
    return env->NewStringUTF(md5_str);
}