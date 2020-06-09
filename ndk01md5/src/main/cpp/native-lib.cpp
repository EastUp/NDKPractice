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
static bool verify = false;
//应用包名
static char *PACKAGE_NAME = "com.east.ndkpractice";
//应用签名
static char *APP_SIGNATURE = "308201dd30820146020101300d06092a864886f70d010105050030373116301406035504030c0d416e64726f69642044656275673110300e06"
                             "0355040a0c07416e64726f6964310b3009060355040613025553301e170d3139303532383032313333385a170d3439303532303032313333385a"
                             "30373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b3009060355040613025553308"
                             "19f300d06092a864886f70d010101050003818d003081890281810084db3be16fc9999e64ba78a72e326584a4dd091423445185a3854e7bc984a7d0d6"
                             "0e31c9c367ffb29dd713eee1cfb11028036949876051e6a2219bf10de00767b698090e93329974df97d4621a9a2c22878fb88600553cfe644a8fa8f037f"
                             "e158f914df890c1eba47fac401a8089b7413240edf7d069230af2cb37594a5b08cd0203010001300d06092a864886f70d01010505000381810023be2d2f5a"
                             "72c1aa8235ef0df403fa7968860b0a8e0137f6418678959575315687fbe11f3ebaf7468db75ae2f98a4cecbf2ed6d0186b1648834d0a1ca22f26d4a0d40193"
                             "4b02c16f5ef99400b924f7c8cb83300c947c2ed1c935c94a6457fad555d0a05933df9bfac4b1c2d0a6eabb1ea9ec62d8e7b2319128b4419d9fab29b1";


/**
 *  参数md5加密
 */
extern "C"
JNIEXPORT jstring JNICALL
Java_com_east_ndkpractice_SignatureUtils_params2Md5(JNIEnv *env, jclass clazz,
                                                         jstring params_) {

    if(!verify){
        return env->NewStringUTF("error signature");
    }

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


/**
 *  签名校验
 */
/**
PackageInfo packageInfo = context.getPackageManager().getPackageInfo(context.getPackageName(), PackageManager.GET_SIGNATURES);
Signature[] signatures = packageInfo.signatures;
return signatures[0].toCharsString();
 */

// C调用Java代码
extern "C"
JNIEXPORT void JNICALL
Java_com_east_ndkpractice_SignatureUtils_signatureVerify(JNIEnv *env, jclass clazz,
                                                         jobject context) {

    // 1.获取包名并验证
    // 1.1获取包名
    jclass jclz = env->GetObjectClass(context);
    jmethodID jmid = env->GetMethodID(jclz,"getPackageName","()Ljava/lang/String;");
    jstring j_package_name = (jstring) env->CallObjectMethod(context,jmid);
    // 1.2比对包名是否一直
    const char *c_package_name = env->GetStringUTFChars(j_package_name,NULL);
    if(strcmp(PACKAGE_NAME,c_package_name) != 0){
        return;
    }

    // 2.获取签名并比对
    // 2.1 获取PackageManager
    jmid = env->GetMethodID(jclz,"getPackageManager","()Landroid/content/pm/PackageManager;");
    jobject  package_manager = env -> CallObjectMethod(context,jmid);
    // 2.2 获取PackageInfo
    jclz = env -> GetObjectClass(package_manager);
    jmid = env -> GetMethodID(jclz,"getPackageInfo","(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    jobject  package_info = env -> CallObjectMethod(package_manager,jmid,j_package_name,0x00000040);
    // 2.3 获取signatures 数组
    jclz = env->GetObjectClass(package_info);
    jfieldID jfieldId = env -> GetFieldID(jclz,"signatures","[Landroid/content/pm/Signature;");
    jobjectArray signatures =  (jobjectArray)env -> GetObjectField(package_info,jfieldId);
    // 2.4 获取 signatures[0]
    jobject signature_first = env ->GetObjectArrayElement(signatures,0);
    // 2.5 调用 signatures 的 toCharsString方法
    jclz = env -> GetObjectClass(signature_first);
    jmid = env -> GetMethodID(jclz,"toCharsString","()Ljava/lang/String;");
    jstring j_signature_str = (jstring)env ->CallObjectMethod(signature_first,jmid);

    // 2.6 对比签名是否一致
    const char * c_signature = env -> GetStringUTFChars(j_signature_str,NULL);
    if(strcmp(c_signature,APP_SIGNATURE) == 0){
        __android_log_print(ANDROID_LOG_ERROR,"TAG","签名校验成功 %s",c_signature);
        verify = true;
    }
}