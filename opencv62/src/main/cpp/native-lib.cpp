#include <jni.h>
#include <string>
#include <android/log.h>  //系统自带的用<>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include "cv_helper.h"

#define TAG "JNI_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
//使用命名空间
using namespace cv;
using namespace std;

// 静态注册，动态注册（6.0 以上 frameworker NDK 源码） Linux

// 逆世界
extern "C"
JNIEXPORT jobject JNICALL
Java_com_east_opencv62_NDKBitmapUtils_againstWorld(JNIEnv *env, jclass clazz, jobject bitmap) {
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);

    Mat res(src.size(), src.type());
    // 获取图片宽高
    int src_w = src.cols; // 图片的宽
    int src_h = src.rows; // 图片的高
    int mid_h = src_h >> 1; // 逆向世界的一般一般
    int a_h = src_h >> 2; // 原图像的4/1

    // 处理输出图像的下半部分
    for (int row = 0; row < mid_h; ++row) {
        for (int col = 0; col < src_w; ++col) {
            if (src.type() == CV_8UC4)
                res.at<Vec4b>(row + mid_h, col) = src.at<Vec4b>(row + a_h, col);
            else if (src.type() == CV_8UC2)
                res.at<Vec3b>(row + mid_h, col) = src.at<Vec3b>(row + a_h, col);
            else if (src.type() == CV_8UC1)
                res.at<uchar>(row + mid_h, col) = src.at<uchar>(row + a_h, col);
        }
    }

    // 处理输出图像的上半部分
    for (int row = 0; row < mid_h; ++row) {
        for (int col = 0; col < src_w; ++col) {
            if (src.type() == CV_8UC4)
                res.at<Vec4b>(row, col) = src.at<Vec4b>(src_h - row - a_h, col);
            else if (src.type() == CV_8UC2)
                res.at<Vec3b>(row, col) = src.at<Vec3b>(src_h - row - a_h, col);
            else if (src.type() == CV_8UC1)
                res.at<uchar>(row, col) = src.at<uchar>(src_h - row - a_h, col);
        }
    }

    cv_helper::mat2bitmap(env, res, bitmap);
    return bitmap;
}

// 浮雕效果
extern "C"
JNIEXPORT jobject JNICALL
Java_com_east_opencv62_NDKBitmapUtils_anaglyph(JNIEnv *env, jclass clazz, jobject bitmap) {
    // 有立体感，突出了轮廓信息，opencv  卷积
    // [1,0]
    // [0,1]
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);
    Mat res(src.size(), src.type());
    src.copyTo(res);

    // 获取图片宽高
    int src_w = src.cols; // 图片的宽
    int src_h = src.rows; // 图片的高

    for (int row = 0; row < src_h - 1; ++row) {
        for (int col = 0; col < src_w - 1; ++col) {
            if (src.type() == CV_8UC4) {
                Vec4b pixels_p = src.at<Vec4b>(row, col);
                Vec4b pixels_n = src.at<Vec4b>(row + 1, col + 1);
                // bgra
                res.at<Vec4b>(row, col)[0] = saturate_cast<uchar>(pixels_p[0] - pixels_n[0] + 128);
                res.at<Vec4b>(row, col)[1] = saturate_cast<uchar>(pixels_p[1] - pixels_n[1] + 128);
                res.at<Vec4b>(row, col)[2] = saturate_cast<uchar>(pixels_p[2] - pixels_n[2] + 128);
            } else if (src.type() == CV_8UC2) {
                Vec3b pixels_p = src.at<Vec3b>(row, col);
                Vec3b pixels_n = src.at<Vec3b>(row + 1, col + 1);
                // bgr
                res.at<Vec3b>(row, col)[0] = saturate_cast<uchar>(pixels_p[0] - pixels_n[0] + 128);
                res.at<Vec3b>(row, col)[1] = saturate_cast<uchar>(pixels_p[1] - pixels_n[1] + 128);
                res.at<Vec3b>(row, col)[2] = saturate_cast<uchar>(pixels_p[2] - pixels_n[2] + 128);
            } else if (src.type() == CV_8UC1) {
                uchar pixels_p = src.at<uchar>(row, col);
                uchar pixels_n = src.at<uchar>(row + 1, col + 1);
                res.at<uchar>(row, col) = saturate_cast<uchar>(pixels_p - pixels_n + 128);
            }
        }
    }

    jobject newBitmap = cv_helper::createBitmap(env, res.cols, res.rows, res.type());

    cv_helper::mat2bitmap(env, res, newBitmap);
    return newBitmap;
}


// 马赛克效果
extern "C"
JNIEXPORT jobject JNICALL
Java_com_east_opencv62_NDKBitmapUtils_mosaic(JNIEnv *env, jclass clazz, jobject bitmap) {
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);

    // 获取图片宽高
    int src_w = src.cols; // 图片的宽
    int src_h = src.rows; // 图片的高

    // 省略人脸识别
    int row_start = src_h >> 2;
    int row_end = src_h * 3 / 4;
    int col_start = src_w >> 2;
    int col_end = src_w * 3 / 4;

    int size_h = 50;
    int size_w = 50;

    for (int row = row_start; row < row_end; row += size_h) {
        for (int col = col_start; col < col_end; col += size_w) {
            int pixel = src.at<int>(row, col);
            for (int m_row = 0; m_row < size_w; m_row++) {
                for (int m_col = 0; m_col < size_h; m_col++) {
                    src.at<int>(row + m_row, col + m_col) = pixel;
                }
            }
            // 当还没到结束的区域时，剩下的区域也需要修改像素
            if (col < col_end && col + size_w > col_end) {
                size_w = col_end - col - 1;
            } else {
                size_w = 50;
            }
        }
        // 当还没到结束的区域时，剩下的区域也需要修改像素
        if (row < row_end && row + size_h > row_end) {
            size_h = row_end - row - 1;
        } else {
            size_h = 50;
        }
    }

    cv_helper::mat2bitmap(env, src, bitmap);
    return bitmap;
}

// 毛玻璃效果
extern "C"
JNIEXPORT jobject JNICALL
Java_com_east_opencv62_NDKBitmapUtils_groundGlass(JNIEnv *env, jclass clazz, jobject bitmap) {
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);
    // 不同于高斯模糊，毛玻璃（对某个区域取随机像素）
    // 获取图片宽高
    int src_w = src.cols; // 图片的宽
    int src_h = src.rows; // 图片的高
    int size = 30;

    RNG rng(time(NULL));
    for (int row = 0; row < src_h; ++row) {
        for (int col = 0; col < src_w; ++col) {
            int random = rng.uniform(0, size + 1);
            if (row + random >= src_h) {
                if (col + random >= src_w) {
                    src.at<int>(row, col) = src.at<int>(row - random, col - random);
                } else {
                    src.at<int>(row, col) = src.at<int>(row - random, col + random);
                }
            } else {
                if (col + random >= src_w) {
                    src.at<int>(row, col) = src.at<int>(row + random, col - random);
                } else {
                    src.at<int>(row, col) = src.at<int>(row + random, col + random);
                }
            }
        }
    }

    cv_helper::mat2bitmap(env, src, bitmap);
    return bitmap;
}

// 油画效果
extern "C"
JNIEXPORT jobject JNICALL
Java_com_east_opencv62_NDKBitmapUtils_oilPainting(JNIEnv *env, jclass clazz, jobject bitmap) {
    // 油画基于直方统计
    // 1. 每个点需要分成 n*n 小块
    // 2. 统计灰度等级
    // 3. 选择灰度等级中最多的值
    // 4. 找到所有的像素取平均值

//    1、获取图像的灰度(gray)图片
//    2、设计一个小方框（4x4 or 8x8 or 10x10等），统计每个小方框的像素值
//    3、将0-255的灰度值划分成几个等级，并把第二步处理的结果映射到所设置的各个等级中，并计数
//    4、找到每个方框中灰度等级最多的所有的像素，并且求取这些像素的均值
//    5、用统计出来的平均值来替代原来的像素值

    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);
    Mat gray;
    cvtColor(src, gray, COLOR_BGRA2GRAY);
    Mat res(src.size(), src.type());

    int src_w = src.cols; // 图片的宽
    int src_h = src.rows; // 图片的高
    int size = 15; // 15 * 15的矩阵
    int size_row = 15;
    int size_col = 15;
    // 知识不是用来背的 20% ，用来唤醒大家的
    for (int row = 0; row < src_h; ++row) {
        for (int col = 0; col < src_w; ++col) {
            int g[20] = {0}, b_g[20] = {0}, g_g[20] = {0}, r_g[20] = {0};// 255/8
            // 这个位置  64 循环 -> 1 个像素点 ， 高斯模糊 ，想想怎么优化
            for (int o_rows = 0; o_rows < size_row; ++o_rows) {
                for (int o_cols = 0; o_cols < size_col; ++o_cols) {
                    uchar gery = 0;
                    if (row + o_rows >= src_h) {
                        if (col + o_cols >= src_w) {
                            gery = gray.at<uchar>(2*src_h - row - o_rows, 2*src_w - col - o_cols);
                        }else{
                            gery = gray.at<uchar>(2*src_h - row - o_rows, col + o_cols);
                        }
                    }else{
                        if (col + o_cols >= src_w) {
                            gery = gray.at<uchar>(row + o_rows, 2*src_w - col - o_cols);
                        }else{
                            gery = gray.at<uchar>(row + o_rows, col + o_cols);
                        }
                    }
                    uchar index = gery / (255 / (size - 1)); // 254*7/254
                    g[index] += 1;
                    // 等级的像素之和
                    b_g[index] += src.at<Vec4b>(row + o_rows, col + o_cols)[0];
                    g_g[index] += src.at<Vec4b>(row + o_rows, col + o_cols)[1];
                    r_g[index] += src.at<Vec4b>(row + o_rows, col + o_cols)[2];

                }
            }
            // 最大的角标找出来
            int max_index = 0;
            int max = g[0];
            for (int i = 1; i < size; ++i) {
                if (g[max_index] < g[i]) {
                    max_index = i;
                    max = g[i];
                }
            }
            // 会超过 255，超过了的话 会截取最高位
            res.at<Vec4b>(row, col)[0] = b_g[max_index] / max;
            res.at<Vec4b>(row, col)[1] = g_g[max_index] / max;
            res.at<Vec4b>(row, col)[2] = r_g[max_index] / max;
        }
    }
    cv_helper::mat2bitmap(env, res, bitmap);
    return bitmap;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_east_opencv62_NDKBitmapUtils_grayOptimize(JNIEnv *env, jclass type, jobject bitmap) {
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);
    // 高斯模糊，毛玻璃 （对某个区域取随机像素）
    int src_w = src.cols;
    int src_h = src.rows;
    // 知识不是用来背的 20% ，用来唤醒大家的
    for (int rows = 0; rows < src_h; ++rows) {
        for (int cols = 0; cols < src_w; ++cols) {
            Vec4b pixels = src.at<Vec4b>(rows, cols);
            uchar b = pixels[0];
            uchar g = pixels[1];
            uchar r = pixels[2];
            // 好几种算法求灰度
            // uchar gery = (b + g + r) / 3;
            // 运算速度 位移 大于 +- 大于  */  int 大于 float
            // uchar gery = 0.299 * r + 0.587 * g + 0.1114 * b;
            uchar gery = (r + (g << 1) + b) >> 2; // 0.4  sobel  |x|+|y|
            src.at<Vec4b>(rows, cols)[0] = gery;
            src.at<Vec4b>(rows, cols)[1] = gery;
            src.at<Vec4b>(rows, cols)[2] = gery;
        }
    }
    return bitmap;
}


