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

// 加载人脸识别的级联分类器
CascadeClassifier cascadeClassifier;

extern "C"
JNIEXPORT void JNICALL
Java_com_east_opencv70_FaceDetection_loadCascade(JNIEnv *env, jobject thiz, jstring file_path) {
    const char *filePath = env->GetStringUTFChars(file_path,0);

    cascadeClassifier.load(filePath);
    LOGE("人脸识别级联分类器加载成功");
    env->ReleaseStringUTFChars(file_path,filePath);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_east_opencv70_FaceDetection_faceDetection(JNIEnv *env, jobject thiz, jlong nativeObj) {
    Mat *src = reinterpret_cast<Mat*>(nativeObj);

    int width = src->cols;
    int height = src->rows;

    Mat gray;
    // 转灰度图，提升运算速度，灰度图所对应的 CV_8UC1 单颜色通道，信息量少 0-255 1u
    cvtColor(*src,gray,COLOR_BGR2GRAY);

    // 检测人脸
    vector<Rect> faces;
    // @param 1.1 会采用上采样、下采样、缩放比例
    // @param 3 会检测多少次才能判断是个人脸
    // @param Size(width/4,height/4) 最小的人脸大小
    // @param Size(width,height) 最大的人脸
    cascadeClassifier.detectMultiScale(gray,faces,1.1,3,0,Size(width/4,height/4),Size(width,height));
    LOGE("人脸size = %d",faces.size());

    if(faces.size() <= 0){
        return;
    }

    // 把脸框出来
    for (int i = 0; i < faces.size(); ++i) {
        rectangle(*src,faces[i],Scalar(255,0,0,255),4,LINE_AA);
    }

    // 与服务端进行对比，是不是我
}