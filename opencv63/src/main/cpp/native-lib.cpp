#include <jni.h>
#include <string>
#include <android/log.h>  //系统自带的用<>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include "cv_helper.h"
#include "bitmap_util.h"

#define TAG "JNI_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
//使用命名空间
using namespace cv;
using namespace std;

// 静态注册，动态注册（6.0 以上 frameworker NDK 源码） Linux

// 逆世界
extern "C"
JNIEXPORT jobject JNICALL
Java_com_east_opencv63_OpenCvUtils_rotation(JNIEnv *env, jclass clazz, jobject bitmap) {
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);

    int res_w = src.rows; // 图片的宽
    int res_h = src.cols; // 图片的高

    Mat res(res_h,res_w, src.type());

    // 处理输出图像的下半部分
    for (int row = 0; row < res_h; ++row) {
        for (int col = 0; col < res_w; ++col) {
            if (src.type() == CV_8UC4)
                res.at<Vec4b>(row, col) = src.at<Vec4b>(src.rows - col, row);
            else if (src.type() == CV_8UC2)
                res.at<Vec3b>(row, col) = src.at<Vec3b>(src.rows - col, row);
            else if (src.type() == CV_8UC1)
                res.at<uchar>(row, col) = src.at<uchar>(src.rows - col, row);
        }
    }

    // 创建一个新的bitmap 宽是原来的高，高是原来的宽
    jobject newBitmap = bitmap_util::create_bitmap(env,res_w,res_h,res.type());
    cv_helper::mat2bitmap(env, res, newBitmap);
    return newBitmap;
}


