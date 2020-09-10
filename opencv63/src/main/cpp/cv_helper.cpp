//
// Created by 123 on 2020/9/9.
//

#include "cv_helper.h"
#include <android/bitmap.h>
#include <android/log.h>  //系统自带的用<>

#define TAG "JNI_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

using namespace cv;

int cv_helper::bitmap2mat(JNIEnv *env, jobject &bitmap, cv::Mat &dst) {
    AndroidBitmapInfo bitmapInfo;
    int getInfoRes = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo);
    if (getInfoRes < 0)
        return getInfoRes;

    void *pixels;
    int lockPixelsRes = AndroidBitmap_lockPixels(env, bitmap, &pixels);
    if (lockPixelsRes < 0)
        return lockPixelsRes;

    if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        dst.create(bitmapInfo.height, bitmapInfo.width, CV_8UC4);
        dst.data = reinterpret_cast<uchar *>(pixels);
    } else if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGB_565) {
        dst.create(bitmapInfo.height, bitmapInfo.width, CV_8UC2);
        dst.data = reinterpret_cast<uchar *>(pixels);
    } else
        return -1;

    AndroidBitmap_unlockPixels(env, bitmap);
    return 0;
}

int cv_helper::mat2bitmap(JNIEnv *env, cv::Mat &src, jobject &bitmap) {
    AndroidBitmapInfo bitmapInfo;
    int getInfoRes = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo);
    if (getInfoRes < 0)
        return getInfoRes;

    void *pixels;
    int lockPixelsRes = AndroidBitmap_lockPixels(env, bitmap, &pixels);
    if (lockPixelsRes < 0)
        return lockPixelsRes;

    if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        Mat temp(bitmapInfo.height, bitmapInfo.width, CV_8UC4,pixels);
        if (src.type() == CV_8UC1)
            cvtColor(src, temp, COLOR_GRAY2BGRA);
        else if (src.type() == CV_8UC2)
            cvtColor(src, temp, COLOR_BGR2BGRA);
        else if (src.type() == CV_8UC4)
            src.copyTo(temp);
    }else if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGB_565) {
           Mat temp(bitmapInfo.height, bitmapInfo.width, CV_8UC2,pixels);
            if (src.type() == CV_8UC1)
                cvtColor(src, temp, COLOR_GRAY2BGR565);
            else if(src.type() == CV_8UC2)
                src.copyTo(temp);
            else if (src.type() == CV_8UC4)
                cvtColor(src, temp, COLOR_BGRA2BGR565);
    } else
        return -1;

    AndroidBitmap_unlockPixels(env,bitmap);

    return 0;
}

jobject cv_helper::createBitmap(JNIEnv *env, jint width, jint height, int type) {
    // 根据 type 来获取 Config
    // Bitmap.Config.valueOf("ARGB_8888"); 等价于 Enum.valueOf(Bitmap.Config.class, "ARGB_8888");
    char *config_name;
    if(type == CV_8UC4)
        config_name = "ARGB_8888";
    else if(type == CV_8UC2)
        config_name == "RGB_565";

    jstring configName = env->NewStringUTF(config_name);
    jclass bitmap_config_class = env->FindClass("android/graphics/Bitmap$Config");
    jmethodID create_bitmap_config_mid = env->GetStaticMethodID(bitmap_config_class,"valueOf", "(Ljava/lang/Class;Ljava/lang/String;)Ljava/lang/Enum;");
    jobject config = env->CallStaticObjectMethod(bitmap_config_class,create_bitmap_config_mid,bitmap_config_class,configName);

    // public static Bitmap createBitmap(int width, int height, @NonNull Config config)
    jclass bitmap_class = env->FindClass("android/graphics/Bitmap");
    jmethodID create_bitmap_mid = env->GetStaticMethodID(bitmap_class,"createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jobject bitmap = env->CallStaticObjectMethod(bitmap_class,create_bitmap_mid,width,height,config);
    return bitmap;
}



