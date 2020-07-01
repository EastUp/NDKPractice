#include <jni.h>
#include <string>
#include <android/log.h>  //系统自带的用<>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include "BitmapMatUtils.h"
#include "cardocr.h"
#include <vector>

#define TAG "JNI_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

using namespace std;

extern "C"
JNIEXPORT jstring JNICALL
Java_com_east_ndk31opencvbankocr_BankCardOcr_carOcr(JNIEnv *env, jobject thiz, jobject bitmap) {

    LOGE("Java_com_east_ndk31opencvbankocr_BankCardOcr_carOcr");

    // 1. bitmap -> mat
    Mat mat;
    BitmapMatUtils::bitmap2mat(env,bitmap,mat);

//    LOGE("%d,%d,%d,%d",mat.rows,mat.cols,mat.type(),CV_8UC4);

    // 截取银行卡区域
    // 轮廓增强（梯度增强）
//    Rect card_area;
//    co1::find_card_area(mat,card_area);
//    // 对我们过滤到的银行卡区域进行裁剪
//    LOGE("找到了%d",card_area.width);
//    Mat card_mat(mat,card_area);
//    // 这个方法会创建文件，但是不会创建文件夹
//    imwrite("/storage/emulated/0/ocr/card_n.jpg",card_mat);

    // 截取卡号区域
    Rect card_number_area;
    co1::find_card_number_area(mat,card_number_area);
    Mat card_number_mat(mat,card_number_area);
    imwrite("/storage/emulated/0/ocr/card_number_area.jpg",card_number_mat);

    // 获取数字
    vector<Mat> numbers;
    co1::find_card_numbers(card_number_mat,numbers);

    return env -> NewStringUTF("66223344");
}



