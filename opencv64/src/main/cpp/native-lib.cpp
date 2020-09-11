#include <jni.h>
#include <string>
#include <android/log.h>  //系统自带的用<>
#include <android/bitmap.h>
#include "cv_helper.h"

#define TAG "JNI_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
//使用命名空间
using namespace cv;
using namespace std;

// 静态注册，动态注册（6.0 以上 frameworker NDK 源码） Linux

// 掩膜操作
extern "C"
JNIEXPORT jobject JNICALL
Java_com_east_opencv64_MainActivity_yanmo(JNIEnv *env, jobject thiz, jobject bitmap) {
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);

    Mat dst;
    Mat kernel = (Mat_<int>(3,3) << 0,-1,0,-1,5,-1,0,-1,0);

    filter2D(src,dst,src.depth(),kernel);

    cv_helper::mat2bitmap(env,dst,bitmap);
    return bitmap;
}


// 模糊
extern "C"
JNIEXPORT jobject JNICALL
Java_com_east_opencv64_MainActivity_blur(JNIEnv *env, jobject thiz, jobject bitmap) {
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);

    Mat dst;
    Mat kernel = Mat::ones(15,15,CV_32FC1)/(15*15);

    filter2D(src,dst,src.depth(),kernel);

    cv_helper::mat2bitmap(env,dst,bitmap);
    return bitmap;
}