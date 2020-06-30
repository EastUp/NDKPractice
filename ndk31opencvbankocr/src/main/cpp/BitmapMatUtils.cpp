//
// Created by 123 on 2020/6/30.
//

#include "BitmapMatUtils.h"
#include <android/bitmap.h>


int BitmapMatUtils::bitmap2mat(JNIEnv *env, jobject bitmap, Mat &mat) {
    // 锁定画布
    void* pixels;

    AndroidBitmap_lockPixels(env,bitmap,&pixels);

    // 获取 bitmap 的信息
    AndroidBitmapInfo bitmapInfo;
    AndroidBitmap_getInfo(env,bitmap,&bitmapInfo);

    // 返回三通道 CV_8UC4-> argb_8888, CV_8UC2->rgb CV_8UC1->黑白
    Mat createMat(bitmapInfo.height,bitmapInfo.width,CV_8UC4);
    if(bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888){ // mat 里面的四颜色通道 CV_8UC4
        Mat temp(bitmapInfo.height,bitmapInfo.width,CV_8UC4,pixels);
        temp.copyTo(createMat);
    }else if(bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGB_565){// mat 里面的三颜色通道 CV_8UC2
        Mat temp(bitmapInfo.height,bitmapInfo.width,CV_8UC2,pixels);
        cvtColor(temp,createMat,COLOR_BGR5652BGRA);
    }

    createMat.copyTo(mat);

    // 解锁画布
    AndroidBitmap_unlockPixels(env,bitmap);

    return 0;

}

int BitmapMatUtils::mat2bitmap(JNIEnv *env, jobject bitmap, Mat &mat) {
    // 锁定画布
    void* pixels;

    AndroidBitmap_lockPixels(env,bitmap,&pixels);

    // 获取 bitmap 的信息
    AndroidBitmapInfo bitmapInfo;
    AndroidBitmap_getInfo(env,bitmap,&bitmapInfo);

    if(bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888){
        Mat temp(bitmapInfo.height,bitmapInfo.width,CV_8UC4,pixels);
        if(mat.type() == CV_8UC4){
            mat.copyTo(temp);
        }else if(mat.type() == CV_8UC2){
            cvtColor(mat,temp,COLOR_BGR5652BGRA);
        }else if(mat.type() == CV_8UC1){
            cvtColor(mat,temp,COLOR_GRAY2BGRA);
        }
    }else if(bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGB_565){
        Mat temp(bitmapInfo.height,bitmapInfo.width,CV_8UC2,pixels);
        if(mat.type() == CV_8UC4){
            cvtColor(mat,temp,COLOR_BGRA2BGR565);
        }else if(mat.type() == CV_8UC2){
            mat.copyTo(temp);
        }else if(mat.type() == CV_8UC1){
            cvtColor(mat,temp,COLOR_GRAY2BGR565);
        }
    }

    // 解锁画布
    AndroidBitmap_unlockPixels(env,bitmap);
    return 0;
}