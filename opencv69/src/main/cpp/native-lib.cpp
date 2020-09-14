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

// hog 特征人脸检测  实现行人检测
extern "C"
JNIEXPORT jobject JNICALL
Java_com_east_opencv69_MainActivity_hogBitmap(JNIEnv *env, jobject thiz, jobject bitmap) {
    Mat src;
    cv_helper::bitmap2mat(env,bitmap,src);

    // 拿 hog 的特征
    // 1. 分成 8*8 的网格 cell 求取直方图 分为 9 个bin，角度范围为 -180~180 之间，对 -180~0 之间的加上 180 取正数，对应的值为梯度值。
    // 2. 对 cell 的 4*4 的块去合并直方图，每次必须要有 1/2 的重叠区域
    // 3. 然后获取直方图的数据

/*    Mat dst,dst_gray;
    resize(src,dst,Size(64,128));
    cvtColor(dst,dst_gray,COLOR_BGRA2GRAY);


//    CV_WRAP HOGDescriptor() : winSize(64,128), blockSize(16,16), blockStride(8,8),
//            cellSize(8,8), nbins(9), derivAperture(1), winSigma(-1),
//            histogramNormType(HOGDescriptor::L2Hys), L2HysThreshold(0.2), gammaCorrection(true),
//            free_coef(-1.f), nlevels(HOGDescriptor::DEFAULT_NLEVELS), signedGradient(false)
//    {}  //创建HOG,参数分别为窗口大小(64,128),块尺寸（16,16），块步长(8,8),cell尺寸(8,8),直方图bin个数9

    HOGDescriptor hogDescriptor;
    vector<float> descriptors;
    vector<Point> locations;

    hogDescriptor.compute(dst_gray,descriptors,Size(),Size(),locations);

    LOGE("descriptors size = %d",descriptors.size()); // 7*15*2*2*9 = 3780*/


    // 实现行人检测
    // 训练样本，直接拿过来用
    Mat bgr; // 只能用 bgr 格式的
    cvtColor(src,bgr,COLOR_BGRA2BGR);
    HOGDescriptor descriptor;
    descriptor.setSVMDetector(descriptor.getDefaultPeopleDetector());
    vector<Rect> foundLocations;
    descriptor.detectMultiScale(bgr,foundLocations,0,Size(12,12));
    for (int i = 0; i < foundLocations.size(); ++i) {
        rectangle(src,foundLocations[i],Scalar(255,0,0),2,LINE_AA);
    }
    return bitmap;
}



// lbp 特征人脸检测
extern "C"
JNIEXPORT jobject JNICALL
Java_com_east_opencv69_MainActivity_lbpBitmap(JNIEnv *env, jobject thiz, jobject bitmap) {
    // 自己手写 两套 3*3 考虑步长和角度
    Mat src;
    cv_helper::bitmap2mat(env,bitmap,src);
    Mat gray;
    cvtColor(src,gray,COLOR_BGRA2GRAY);
    // 3*3 lbp 计算的特征数据
    Mat result = Mat::zeros(Size(src.cols - 2,src.rows - 2),CV_8UC1);

    // 有点类似卷积操作
    for (int row = 0; row < gray.rows - 1; ++row) {
        for (int col = 0; col < gray.cols - 1; ++col) {
            uchar pixels = gray.at<uchar>(row,col); // 中心点
            int rPixels = 0;
            rPixels |= (pixels <= gray.at<uchar>(row-1,col-1)) << 0;
            rPixels |= (pixels <= gray.at<uchar>(row-1,col)) << 1;
            rPixels |= (pixels <= gray.at<uchar>(row-1,col+1)) << 2;
            rPixels |= (pixels <= gray.at<uchar>(row,col-1)) << 7;
            rPixels |= (pixels <= gray.at<uchar>(row,col+1)) << 3;
            rPixels |= (pixels <= gray.at<uchar>(row+1,col-1)) << 6;
            rPixels |= (pixels <= gray.at<uchar>(row+1,col)) << 5;
            rPixels |= (pixels <= gray.at<uchar>(row+1,col+1)) << 4;

            result.at<uchar>(row-1,col-1) = rPixels;
        }
    }
    jobject  resBitmap = cv_helper::createBitmap(env,result.cols,result.rows,src.type());
    cv_helper::mat2bitmap(env,result,resBitmap);
    return resBitmap;
}